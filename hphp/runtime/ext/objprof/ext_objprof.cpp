/*
   +----------------------------------------------------------------------+
   | HipHop for PHP                                                       |
   +----------------------------------------------------------------------+
   | Copyright (c) 2010-present Facebook, Inc. (http://www.facebook.com)  |
   | Copyright (c) 1997-2010 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
*/

#include "hphp/runtime/ext/extension.h"

/*
 *                       Project ObjProf
 * What is it?
 * Breakdown of allocated memory by object types.
 *
 * How does it work?
 * We traverse all instances of ObjectData* to measure their memory.
 *
 * How do I use it?
 * Call objprof_get_data to trigger the scan.
 */

#include <algorithm>
#include <sstream>
#include <vector>

#include "hphp/runtime/base/array-init.h"
#include "hphp/runtime/base/collections.h"
#include "hphp/runtime/base/object-iterator.h"
#include "hphp/runtime/base/memory-manager-defs.h"
#include "hphp/runtime/base/typed-value.h"
#include "hphp/runtime/ext/asio/ext_static-wait-handle.h"
#include "hphp/runtime/ext/datetime/ext_datetime.h"
#include "hphp/runtime/vm/class.h"
#include "hphp/runtime/vm/memo-cache.h"
#include "hphp/runtime/vm/named-entity-defs.h"
#include "hphp/util/alloc.h"
#include "hphp/util/low-ptr.h"
#include "hphp/runtime/base/implicit-context.h"


namespace HPHP {
size_t asio_object_size(const ObjectData*);

namespace {

TRACE_SET_MOD(objprof)

///////////////////////////////////////////////////////////////////////////////

const StaticString
  s_cpp_stack("cpp_stack"),
  s_cpp_stack_peak("cpp_stack_peak"),
  s_dups("dups"),
  s_refs("refs"),
  s_srefs("srefs"),
  s_path("path"),
  s_paths("paths"),
  s_bytes("bytes"),
  s_bytes_rel("bytes_normalized"),
  s_instances("instances");

struct ObjprofObjectReferral {
  uint64_t refs{0};
  hphp_fast_set<const ObjectData*> sources;
};
struct ObjprofClassReferral {
  uint64_t refs{0};
  hphp_fast_set<Class*> sources;
};

struct ObjprofMetrics {
  uint64_t instances{0};
  uint64_t bytes{0};
  double bytes_rel{0};
};
using PathsToObject = hphp_fast_map<
  ObjectData*, hphp_fast_map<std::string, ObjprofObjectReferral>>;
using PathsToClass = hphp_fast_map<
  std::string, hphp_fast_map<std::string, ObjprofClassReferral>>;

using ObjprofStack = std::vector<std::string>;
using ClassProp = std::pair<std::string, std::string>;

// Stack of pointers used for avoiding back-links when performing a DFS scan
// starting at a root node.
using ObjprofValuePtrStack = std::vector<const void*>;

enum ObjprofFlags {
  DEFAULT = 1,
  USER_TYPES_ONLY = 2,
  PER_PROPERTY = 4
};

struct ObjprofState {
  const ObjectData* source;
  Optional<ObjprofStack> stack;
  Optional<PathsToObject> paths;
  ObjprofValuePtrStack val_stack;
  const hphp_fast_set<std::string>& exclude_classes;
  ObjprofFlags flags;
  // While iterating the heap objprof is retaining pointers to heap objects,
  // any logic that may reenter (or trigger frees) must be deferred.
  std::vector<std::string>& deferred_warnings;
};

std::pair<int, double> tvGetSize(TypedValue tv, ObjprofState& env);

std::pair<int, double> getObjSize(
  const ObjectData* obj, ObjprofState& env,
  hphp_fast_map<ClassProp, ObjprofMetrics>* histogram
);

std::string pathString(const ObjprofStack& stack, const char* sep) {
  assertx(stack.size() < 100000000);

  std::ostringstream os;
  for (size_t i = 0; i < stack.size(); ++i) {
    if (i != 0) os << sep;
    os << stack[i];
  }
  return os.str();
}

bool cycleExists(ObjprofState& env, const void* ptr) {
  auto ptr_begin = env.val_stack.begin();
  auto ptr_end = env.val_stack.end();
  return std::find(ptr_begin, ptr_end, ptr) != ptr_end;
}

void issueWarnings(std::vector<std::string>& deferred_warnings) {
  for (auto const& warning : deferred_warnings) {
    raise_warning(warning);
  }
  deferred_warnings.clear();
}

/**
 * Determines whether the given object is a "root" node. Class instances
 * are considered root nodes, with the exception of classes starting with
 * "HH\\" (collections, wait handles etc)
 */
bool isObjprofRoot(
  const ObjectData* obj,
  ObjprofFlags flags,
  const hphp_fast_set<std::string>& exclude_classes
) {
  using SSWH = c_StaticWaitHandle;

  Class* cls = obj->getVMClass();
  auto cls_name = cls->name()->toCppString();
  // Classes in exclude_classes not considered root
  if (exclude_classes.find(cls_name) != exclude_classes.end()) return false;
  if (obj == *ImplicitContext::emptyCtx) return false;
  // In USER_TYPES_ONLY mode, Classes with "HH\\" prefix not considered root
  if ((flags & ObjprofFlags::USER_TYPES_ONLY) != 0) {
    if (cls_name.compare(0, 3, "HH\\") == 0) return false;
  }
  if (SSWH::NullHandle.bound() && SSWH::NullHandle.isInitNoProfile()) {
    if (obj == SSWH::NullHandle.getNoProfile()->get())  return false;
    if (obj == SSWH::TrueHandle.getNoProfile()->get())  return false;
    if (obj == SSWH::FalseHandle.getNoProfile()->get()) return false;
  }
  return true;
}

/**
 * Measures the size of the array and referenced objects without going
 * into ObjectData* references.
 */
std::pair<int, double> sizeOfArray(
  const ArrayData* ad, ObjprofState& env, Class* cls,
  hphp_fast_map<ClassProp, ObjprofMetrics>* histogram
) {
  if (cycleExists(env, ad)) {
    FTRACE(3, "Cycle found for ArrayData*({})\n", ad);
    return std::make_pair(0, 0);
  }

  if (env.val_stack.size() >= Cfg::Eval::ObjProfMaxNesting) {
    env.deferred_warnings.push_back(
      "objprof: data structure is too deep, pruning traversal"
    );
    return std::make_pair(0, 0);
  }

  FTRACE(3, "\n\nInserting ArrayData*({})\n", ad);
  env.val_stack.push_back(ad);

  int size = 0;
  double sized = 0;
  if (ad->isVecType()) {
    FTRACE(2, "Iterating vec\n");
    if (env.stack) env.stack->push_back("ArrayIndex");

    IterateV(ad, [&] (TypedValue v) {
      auto val_size_pair = tvGetSize(v, env);
      if (histogram) {
        auto histogram_key = std::make_pair(cls->name()->toCppString(), "<index>");
        auto& metrics = (*histogram)[histogram_key];
        metrics.instances += 1;
        metrics.bytes += val_size_pair.first;
        metrics.bytes_rel += val_size_pair.second;
      }
      size += val_size_pair.first;
      sized += val_size_pair.second;
      FTRACE(2, "Value size for item was {}\n", val_size_pair.first);
      return false;
    });

    if (env.stack) env.stack->pop_back();
  } else {
    FTRACE(2, "Iterating mixed array\n");
    IterateKV(ad, [&] (TypedValue k, TypedValue v) {

      std::string k_str;
      std::pair<int, double> key_size_pair;
      switch (k.m_type) {
        case KindOfPersistentString:
        case KindOfString: {
          auto const str = k.m_data.pstr;
          k_str = str->toCppString();
          if (env.stack) {
            env.stack->push_back(std::string("ArrayKeyString:" + k_str));
          }
          key_size_pair = tvGetSize(k, env);
          FTRACE(2, "  Iterating str-key {} with size {}:{}\n",
            str->data(), key_size_pair.first, key_size_pair.second);
          break;
        }
        case KindOfInt64: {
          int64_t num = k.m_data.num;
          k_str = std::to_string(num);
          if (env.stack) {
            env.stack->push_back(std::string("ArrayKeyInt:" + k_str));
          }
          key_size_pair = tvGetSize(k, env);
          FTRACE(2, "  Iterating num-key {} with size {}:{}\n",
            num, key_size_pair.first, key_size_pair.second);
          break;
        }
        case KindOfUninit:
        case KindOfNull:
        case KindOfPersistentVec:
        case KindOfBoolean:
        case KindOfPersistentDict:
        case KindOfDouble:
        case KindOfPersistentKeyset:
        case KindOfObject:
        case KindOfResource:
        case KindOfVec:
        case KindOfDict:
        case KindOfKeyset:
        case KindOfRFunc:
        case KindOfFunc:
        case KindOfClass:
        case KindOfLazyClass:
        case KindOfClsMeth:
        case KindOfRClsMeth:
        case KindOfEnumClassLabel:
          always_assert(false);
      }

      auto val_size_pair = tvGetSize(v, env);
      FTRACE(2, "  Value size for that key was {}:{}\n",
        val_size_pair.first, val_size_pair.second);
      if (histogram) {
        auto const histogram_key = std::make_pair(cls->name()->toCppString(), k_str);
        auto& metrics = (*histogram)[histogram_key];
        metrics.instances += 1;
        metrics.bytes += val_size_pair.first + key_size_pair.first;
        metrics.bytes_rel += val_size_pair.second + key_size_pair.second;
      }
      size += val_size_pair.first + key_size_pair.first;
      sized += val_size_pair.second + key_size_pair.second;

      if (env.stack) env.stack->pop_back();
      return false;
    });
  }

  FTRACE(3, "Popping {} frm stack in sizeOfArray. Stack size before pop {}\n",
    env.val_stack.back(), env.val_stack.size()
  );
  env.val_stack.pop_back();

  return std::make_pair(size, sized);
}

/**
 * Measures the size of the typed value and referenced objects without going
 * into ObjectData* references.
 */
std::pair<int, double> tvGetSize(TypedValue tv, ObjprofState& env) {
  int size = sizeof(tv);
  double sized = size;

  if (!isRealType(tv.m_type)) {
    FTRACE(3, " Skipping tv with invalid type {}\n", tv.m_type);
    return std::make_pair(size, sized);
  }

  if (!isRefcountedType(tv.m_type)) {
    FTRACE(3, " Skipping tv with non refcounted type {}\n", tv.m_type);
    return std::make_pair(size, sized);
  }

  auto const cnt = tv.m_data.pcnt;
  auto const start = tl_heap->find(cnt);
  if (!start) {
    FTRACE(3, " Skipping tv pointing outside heap {}\n", cnt);
    return std::make_pair(size, sized);
  }
  const HeapObject* obj = innerObj(start);
  if (!obj) obj = start;
  if (cnt != obj) {
    FTRACE(3, " Skipping tv pointing to middle of HeapObj {}\n", cnt);
    return std::make_pair(size, sized);
  }
  if (!cnt->checkCount()) {
    FTRACE(3, " Skipping HeapObj with invalid count {}\n", cnt);
    return std::make_pair(size, sized);
  }

  auto add_array_size = [&](ArrayData* arr) {
    auto size_of_array_pair = sizeOfArray(
      arr, env,
      nullptr, /* cls */
      nullptr /* histogram */
    );
    auto arr_ref_count = int{cnt->count()};
    FTRACE(3, " ArrayData tv: at {} with ref count {}\n",
      (void*)arr,
      arr_ref_count
    );
    size += sizeof(*arr);
    size += size_of_array_pair.first;
    assertx(arr_ref_count > 0);
    sized += sizeof(*arr) / (double)arr_ref_count;
    sized += size_of_array_pair.second / (double)(arr_ref_count);
  };

  UNUSED auto const headerName = header_names[int(cnt->kind())];
  switch (cnt->kind()) {
    case HeaderKind::String: {
      assertx(cnt->isRefCounted());
      auto const str = (StringData*)cnt;
      size += str->size();
      auto str_ref_count = int{cnt->count()};
      FTRACE(3, " String tv: {} string at {} ref count: {}\n",
        str->data(),
        (void*)str,
        str_ref_count
      );
      assertx(str_ref_count > 0);
      sized += (str->size() / (double)(str_ref_count));
      break;
    }
    case HeaderKind::Vec:
    case HeaderKind::Dict:
    case HeaderKind::Keyset:
    case HeaderKind::BespokeVec:
    case HeaderKind::BespokeDict:
    case HeaderKind::BespokeKeyset: {
      assertx(cnt->isRefCounted());
      ArrayData* arr = (ArrayData*)cnt;
      add_array_size(arr);
      break;
    }
    case HeaderKind::AsyncFuncWH:
    case HeaderKind::AwaitAllWH:
    case HeaderKind::ConcurrentWH:
    case HeaderKind::WaitHandle:
    case HeaderKind::NativeObject:
    case HeaderKind::Vector:
    case HeaderKind::Map:
    case HeaderKind::Set:
    case HeaderKind::Pair:
    case HeaderKind::ImmVector:
    case HeaderKind::ImmMap:
    case HeaderKind::ImmSet:
    case HeaderKind::Closure:
    case HeaderKind::Object: {
      assertx(cnt->isRefCounted());
      ObjectData* obj_cnt = (ObjectData*)cnt;
      // If its not a root node, recurse into the object to determine its size
      if (!isObjprofRoot(obj_cnt, env.flags, env.exclude_classes)) {
        auto obj_size_pair = getObjSize(obj_cnt, env, nullptr /* histogram */);
        size += obj_size_pair.first;
        auto obj_ref_count = int{cnt->count()};
        FTRACE(3, " ObjectData tv: at {} with ref count {}\n",
          (void*)obj_cnt,
          obj_ref_count
        );
        assertx(obj_ref_count > 0);
        sized += obj_size_pair.second / (double)(obj_ref_count);
      } else if (env.stack && env.paths) {
        // notice we might have multiple OBJ->path->OBJ for same path
        // (e.g. packed array where we omit the index number)
        auto cls = obj_cnt->getVMClass();
        env.stack->push_back(std::string("Object:" + cls->name()->toCppString()));
        auto pathStr = pathString(*env.stack, "->");
        env.stack->pop_back();

        auto& pathsToTv = (*env.paths)[obj_cnt];
        auto& referral = pathsToTv[pathStr];
        if (env.source) {
          referral.sources.insert(env.source);
        }
        referral.refs += 1;

        FTRACE(3, " ObjectData tv: at {} of type {} at path {}, refs {}\n",
          (void*)obj_cnt,
          obj_cnt->getClassName().data(),
          pathStr,
          referral.refs
        );
      }
      break;
    }
    case HeaderKind::RFunc: {
      assertx(cnt->isRefCounted());
      size += sizeof(RFuncData);
      add_array_size(((RFuncData*)cnt)->m_arr);
      break;
    }
    case HeaderKind::Resource: {
      assertx(cnt->isRefCounted());
      auto const resource = (ResourceHdr*)cnt;
      auto resource_size = resource->heapSize();
      size += resource_size;
      auto const count = cnt->count();
      assertx(count > 0);
      sized += resource_size / (double)(count);
      break;
    }
    case HeaderKind::RClsMeth: {
      assertx(cnt->isRefCounted());
      auto const rclsmeth = (RClsMethData*)cnt;
      auto const sz = sizeof(RClsMethData);
      auto ref_count = int{cnt->count()};
      size += sz;
      FTRACE(3, " RClsMeth tv: rclsmeth at {} with ref count {}\n",
             (void*)rclsmeth, ref_count);
      assertx(ref_count > 0);
      sized += sz / (double)ref_count;
      add_array_size(rclsmeth->m_arr);
      break;
    }
    case HeaderKind::AsyncFuncFrame:
    case HeaderKind::NativeData:
    case HeaderKind::ClsMeth:
    case HeaderKind::MemoData:
    case HeaderKind::ClosureHdr:
    case HeaderKind::Cpp:
    case HeaderKind::SmallMalloc:
    case HeaderKind::BigMalloc:
    case HeaderKind::Free:
    case HeaderKind::Hole:
    case HeaderKind::Slab:
      FTRACE(3, " Skipping errant pointer {} to {}\n", cnt, headerName);
      break;
  }

  return std::make_pair(size, sized);
}

std::pair<int, double> getObjSize(
  const ObjectData* obj, ObjprofState& env,
  hphp_fast_map<ClassProp, ObjprofMetrics>* histogram
) {
  Class* cls = obj->getVMClass();

  if (cycleExists(env, obj)) {
    FTRACE(3, "Cycle found for {}*({})\n", obj->getClassName().data(), obj);
    return std::make_pair(0, 0);
  }

  if (env.val_stack.size() >= Cfg::Eval::ObjProfMaxNesting) {
    env.deferred_warnings.push_back(
      "objprof: data structure is too deep, pruning traversal"
    );
    return std::make_pair(0, 0);
  }

  FTRACE(3, "\n\nInserting {}*({})\n", obj->getClassName().data(), obj);
  env.val_stack.push_back(obj);

  FTRACE(1, "Getting object size for type {} at {}\n",
    obj->getClassName().data(),
    obj
  );
  int size;
  if (UNLIKELY(obj->isWaitHandle())) {
    size = asio_object_size(obj);
  } else {
    size = sizeof(ObjectData);
  }
  double sized = size;

  if (env.stack) env.stack->push_back(
    std::string("Object:" + cls->name()->toCppString())
  );

  if (obj->isCollection()) {
    auto const arr = collections::asArray(obj);
    if (arr) {
      auto array_size_pair = sizeOfArray(arr, env, cls, histogram);
      size += array_size_pair.first;
      sized += array_size_pair.second;
    } else {
      assertx(collections::isType(cls, CollectionType::Pair));
      auto pair = static_cast<const c_Pair*>(obj);
      auto elm_size_pair = tvGetSize(*pair->get(0), env);
      size += elm_size_pair.first;
      sized += elm_size_pair.second;
      elm_size_pair = tvGetSize(*pair->get(1), env);
      size += elm_size_pair.first;
      sized += elm_size_pair.second;
    }

    if (env.stack) env.stack->pop_back();
    FTRACE(3, "Popping {} frm stack in getObjSize. Stack size before pop {}\n",
      env.val_stack.back(), env.val_stack.size()
    );
    env.val_stack.pop_back();
    return std::make_pair(size, sized);
  }

  IteratePropMemOrder(
    obj,
    [&](Slot slot, const Class::Prop& prop, tv_rval val) {
      FTRACE(2, "Skipping declared property key {}\n", prop.name->data());

      FTRACE(2, "Counting value for key {}\n", prop.name->data());
      if (env.stack) {
        env.stack->push_back(
          std::string("Property:" + prop.name->toCppString())
        );
      }

      auto val_size_pair = tvGetSize(val.tv(), env);
      if (env.stack) env.stack->pop_back();

      FTRACE(2, "   Summary for key {} with size key=0:0, val={}:{}\n",
        prop.name->data(),
        val_size_pair.first,
        val_size_pair.second
      );

      if (histogram) {
        auto histogram_key = std::make_pair(cls->name()->toCppString(), prop.name->toCppString());
        auto& metrics = (*histogram)[histogram_key];
        metrics.instances += 1;
        metrics.bytes += val_size_pair.first;
        metrics.bytes_rel += val_size_pair.second;
      }

      size += val_size_pair.first;
      sized += val_size_pair.second;
    },
    [&](TypedValue key_tv, TypedValue val) {
      assertx(tvIsString(key_tv) || tvIsInt(key_tv));
      std::pair<int, double> key_size_pair = {0, 0.0};
      std::string k_str;
      if (tvIsString(key_tv)) {
        k_str = key_tv.m_data.pstr->toCppString();
        FTRACE(2, "Counting dynamic string key {}\n", k_str);
        key_size_pair = tvGetSize(key_tv, env);
      } else {
        assertx(isIntType(key_tv.m_type));
        k_str = std::to_string(key_tv.m_data.num);
        FTRACE(2, "Skipping dynamic int key {}\n", k_str);
      }

      FTRACE(2, "Counting value for key {}\n", k_str);
      if (env.stack) env.stack->push_back(std::string("Key:" + k_str));
      auto val_size_pair = tvGetSize(val, env);
      if (env.stack) env.stack->pop_back();

      FTRACE(2, "   Summary for key {} with size key={}:{}, val={}:{}\n",
        k_str,
        key_size_pair.first,
        key_size_pair.second,
        val_size_pair.first,
        val_size_pair.second
      );

      if (histogram) {
        auto histogram_key = std::make_pair(cls->name()->toCppString(), k_str);
        auto& metrics = (*histogram)[histogram_key];
        metrics.instances += 1;
        metrics.bytes += key_size_pair.first + val_size_pair.first;
        metrics.bytes_rel += key_size_pair.second + val_size_pair.second;
      }

      size += key_size_pair.first + val_size_pair.first;
      sized += key_size_pair.second + val_size_pair.second;
    }
  );

  if (env.stack) env.stack->pop_back();
  FTRACE(3, "Popping {} frm stack in getObjSize. Stack size before pop {}\n",
    env.val_stack.back(), env.val_stack.size()
  );
  env.val_stack.pop_back();

  return std::make_pair(size, sized);
}

///////////////////////////////////////////////////////////////////////////////
// Function that inits the scan of the memory and count of class pointers

Array HHVM_FUNCTION(objprof_get_data,
  int64_t flags = ObjprofFlags::DEFAULT,
  const Array& exclude_list = Array()
) {
  hphp_fast_map<ClassProp, ObjprofMetrics> histogram;
  UNUSED auto objprof_props_mode = (flags & ObjprofFlags::PER_PROPERTY) != 0;

  // Create a set of std::strings from the exclude_list provided. This de-dups
  // the exclude_list, and also provides for fast lookup when determining
  // whether a given class is in the exclude_list
  hphp_fast_set<std::string> exclude_classes;
  for (ArrayIter iter(exclude_list); iter; ++iter) {
    exclude_classes.insert(iter.second().toString().data());
  }

  std::vector<std::string> deferred_warnings;
  tl_heap->forEachObject([&](const ObjectData* obj) {
    if (!isObjprofRoot(obj, (ObjprofFlags)flags, exclude_classes)) return;
    if (obj->hasZeroRefs()) return;
    ObjprofState env{
      .source = nullptr,
      .stack = std::nullopt,
      .paths = std::nullopt,
      .val_stack = ObjprofValuePtrStack{},
      .exclude_classes = exclude_classes,
      .flags = (ObjprofFlags)flags,
      .deferred_warnings = deferred_warnings
    };
    auto objsizePair = getObjSize(
      obj, env,
      objprof_props_mode ? &histogram : nullptr
    );

    if (!objprof_props_mode) {
      auto cls = obj->getVMClass();
      auto cls_name = cls->name()->toCppString();
      auto& metrics = histogram[std::make_pair(cls_name, "")];
      metrics.instances += 1;
      metrics.bytes += objsizePair.first;
      metrics.bytes_rel += objsizePair.second;

      FTRACE(1, "ObjectData* at {} ({}) size={}:{}\n",
       obj,
       cls_name,
       objsizePair.first,
       objsizePair.second
      );
    }
  });
  issueWarnings(deferred_warnings);

  // Create response
  DictInit objs(histogram.size());
  for (auto const& it : histogram) {
    auto c = it.first;
    auto cls = c.first;
    auto prop = c.second;
    auto key = cls;
    if (prop != "") {
      key += "::" + c.second;
    }
    auto metrics_val = make_dict_array(
      s_instances, Variant(it.second.instances),
      s_bytes, Variant(it.second.bytes),
      s_bytes_rel, it.second.bytes_rel,
      s_paths, init_null()
    );

    objs.set(StrNR(key), Variant(metrics_val));
  }
  return objs.toArray();
}

namespace {
/* Given an object, this function goes through the class,
*  finds all memoized methods and attributes their footprint
*/
void attributeMemoizedFootprint(const ObjectData* obj, hphp_fast_map<ClassProp, ObjprofMetrics>* histogram,
                                bool objprof_props_mode,
                                hphp_fast_set<const HPHP::MemoCacheBase*>& seen_caches,
                                ObjprofState& env) {
  auto cls = obj->getVMClass();
  auto clsName = cls->name()->toCppString();
  auto method_count = cls->numMethods();

  for (Slot i = 0; i < method_count; ++i) {
      auto const m = cls->getMethod(i);
      if(m->isMemoizeWrapper() && !m->isStatic() && m->cls() == cls) {

        auto memo_pair = cls->memoSlotForFunc(m->getFuncId());
        auto memoslot = obj->memoSlot(memo_pair.first);
        UNUSED auto isShared = memo_pair.second;

        if(memoslot->isCache()) {
          auto cache = memoslot->getCache();
          // if we have already seen this cache, skip to avoid double counting
          if(!cache || (seen_caches.find(cache) != seen_caches.end())) {
            continue;
          }
          std::vector<PerCacheInfo> cache_mem_footprints;
          cache->heapSizesPerCacheEntry(cache_mem_footprints);
          std::string func_name;
          for (auto e : cache_mem_footprints) {
            auto tv_size = tvGetSize(e.cacheEntry, env);
            if(e.funcId == FuncId::Invalid) {
              // this should only happen if cache is non-shared
              assertx(!isShared);
              func_name = m->name()->toCppString();
            }
            else {
              func_name = Func::fromFuncId(e.funcId)->name()->toCppString();
            }
            auto histogram_key = objprof_props_mode ?
                                  std::make_pair(clsName, func_name) :
                                  std::make_pair(clsName, "");
            auto& metrics = (*histogram)[histogram_key];
            // we certainly have visited getObjSize before on this object due to call order
            // which means this object has instance count of at least 1
            // if objprof_props_mode is true, then we'd have the key as "Cls::Func" so increment instances
            // otherwise the key is "Cls", don't increment instances
            if(objprof_props_mode) {
              metrics.instances += 1;
            }
            metrics.bytes += e.keySize + tv_size.first;
            metrics.bytes_rel = metrics.bytes / metrics.instances;
          }
          seen_caches.insert(cache);
        }
        else {
          // value type
          auto tv_size = tvGetSize(*(memoslot->getValue()), env);
          auto histogram_key = objprof_props_mode ?
                              std::make_pair(clsName, m->name()->toCppString()) :
                              std::make_pair(clsName, "");
          auto& metrics = (*histogram)[histogram_key];
          if(objprof_props_mode) {
              metrics.instances += 1;
            }
          metrics.bytes += tv_size.first;
          metrics.bytes_rel = metrics.bytes / metrics.instances;
        }
      }
  }
}
template<class Fn> void iterateRDSRoots(const Fn& fn) {
  auto rds = rds::header();
  if (rds) {
    rds::forEachNormalAlloc(fn);
    rds::forEachLocalAlloc(fn);
  }
  rds::local::iterateRoots(fn);
}

void attributeStaticMemoizedFootprint(hphp_fast_map<ClassProp, ObjprofMetrics>* histogram,
                                      bool objprof_props_mode, ObjprofState& env) {
  /*
  *  To get the static memoized methods cache sizes, we need to iterate RDS roots
  *  and find the static memo caches and values.
  */
  iterateRDSRoots([&](const void* h, UNUSED size_t size, UNUSED type_scan::Index tyindex) {
    // get the offset from rds::tl_base so we can lookup the reverse link
    rds::Handle rdsOffset = rds::ptrToHandle<rds::Mode::NonPersistent>(h);
    auto sym = rds::reverseLinkExact(rdsOffset);
    if (sym.has_value()) {
      match(
      sym.value(),
      [&] (const rds::StaticMemoValue& memo_value) {
        auto tv = reinterpret_cast<const TypedValue*>(h);
        assertx(tv);
        auto func = Func::fromFuncId(memo_value.funcId);
        assertx(func);

        auto clsName = func->cls() ? func->cls()->name()->toCppString() : "NoClass";
        auto histogram_key = objprof_props_mode ?
                              std::make_pair(clsName, func->name()->toCppString()) :
                              std::make_pair(clsName, "Static");
        auto& metrics = (*histogram)[histogram_key];
        auto tv_size = tvGetSize(*tv, env);

        /*
        * Here, we either want per prop data or account for everything at the class level
        * In either case, the instance count will always be 1 (due to static)
        * If we want per prop data, we simply assign it into bytes/bytes_rel
        * If we want class level data, we increment it as there can be many static methods per class
        */
        metrics.instances = 1;
        if(objprof_props_mode) {
          metrics.bytes = tv_size.first;
          metrics.bytes_rel = tv_size.first;

        }
        else {
          metrics.bytes += tv_size.first;
          metrics.bytes_rel += tv_size.first;
        }
      }, // rds::StaticMemoValue end

      [&] (const rds::StaticMemoCache& memo_cache) {
        // Process the StaticMemoCache type, which is a pointer to a MemoCacheBase
        assertx(h);
        auto cacheAddress = reinterpret_cast<const uint64_t*>(h);
        // the pointer stored in the cache address should point to the MemoCacheBase
        auto cache = reinterpret_cast<const MemoCacheBase*>(*cacheAddress);
        std::vector<PerCacheInfo> cacheMemFootprints;
        cache->heapSizesPerCacheEntry(cacheMemFootprints);
        for (auto e : cacheMemFootprints) {
          assertx(memo_cache.funcId != FuncId::Invalid);
          auto cls = Func::fromFuncId(memo_cache.funcId)->cls();
          auto clsName = cls ? cls->name()->toCppString() : "NoClass";
          auto func = Func::fromFuncId(memo_cache.funcId);
          assertx(func);
          auto tv_size = tvGetSize(e.cacheEntry, env);

          auto histogram_key = objprof_props_mode ?
                                std::make_pair(clsName, func->name()->toCppString()) :
                                std::make_pair(clsName, "Static");
          auto& metrics = (*histogram)[histogram_key];
          metrics.instances = 1;
          if(objprof_props_mode) {
            metrics.bytes = tv_size.first + e.keySize;
            metrics.bytes_rel = tv_size.first + e.keySize;
          }
          else {
            // Class level aggregation, increment
            metrics.bytes += tv_size.first + e.keySize;
            metrics.bytes_rel += tv_size.first + e.keySize;
          }
        }
      },
      [&](auto const& param) {
        // ignore other types for now
        // Note: we could potentially process SPropCache here but only when we
        // bind that in rds.cpp inside bindOnLinkImpl
      });
    }
  });
}

void attributeStaticPropFootprint(hphp_fast_map<ClassProp, ObjprofMetrics>* histogram,
                                      bool objprof_props_mode, ObjprofState& env) {
  NamedType::foreach_class([&](Class* cls) {
    if (cls->needsInitSProps()) {
      return;
    }
    auto const static_props = cls->staticProperties();
    auto const n_SProps = cls->numStaticProperties();
    for (Slot i = 0; i < n_SProps; ++i) {
      auto const& prop = static_props[i];
      auto tv = cls->getSPropData(i);
      if (tv == nullptr || tv->m_type == KindOfUninit) {
        continue;
      }
      if (tv->m_data.num == 0) {
        continue;
      }

      auto size_pair = tvGetSize(*tv, env);
      auto histogram_key = objprof_props_mode ?
                           std::make_pair(cls->name()->toCppString(),
                           prop.name->toCppString()) :
                           std::make_pair(cls->name()->toCppString(), "SPropCache");
      auto& metrics = (*histogram)[histogram_key];
      metrics.instances = 1;
      if(objprof_props_mode) {
        metrics.bytes = size_pair.first;
        metrics.bytes_rel = size_pair.first;
      }
      else {
        // Class level aggregation, increment
        metrics.bytes += size_pair.first;
        metrics.bytes_rel += size_pair.first;
      }
    }
  });
}

} // anonymous namespace

///////////////////////////////////////////////////////////////////////////////
/* Function that inits the scan of the memory and count of class pointers
*  If ObjprofFlags::PER_PROPERTY is set, it includes one entry per memoized function,
*  including the number of times it was called, and the total size of its cache.
*  We will overload the ObjProfObjectStats struct to include this information
*  type ObjprofObjectStats = shape(
*      'instances'        => int,   // total number of cache entries for this function
*      'bytes'            => int,   // total sum of all caches for the function
*      ...
*    );
*  If the ObjprofFlags::PER_PROPERTY was not supplied, the outout will summarize
*  all non static memo cache footprints in the object itself, and all static
*  memo cache footprints under ::Static title
*/
Array HHVM_FUNCTION(objprof_get_data_extended,
  int64_t flags = ObjprofFlags::DEFAULT,
  const Array& exclude_list = Array()
) {
  hphp_fast_map<ClassProp, ObjprofMetrics> histogram;
  hphp_fast_set<const HPHP::MemoCacheBase*> seen_caches;
  auto objprof_props_mode = (flags & ObjprofFlags::PER_PROPERTY) != 0;

  // Create a set of std::strings from the exclude_list provided. This de-dups
  // the exclude_list, and also provides for fast lookup when determining
  // whether a given class is in the exclude_list
  hphp_fast_set<std::string> exclude_classes;
  for (ArrayIter iter(exclude_list); iter; ++iter) {
    if(!iter.second().isString()) {
      SystemLib::throwInvalidArgumentExceptionObject(
        std::string("Exclude list must be array of strings"));
    }
    exclude_classes.insert(iter.second().toString().data());
  }

  std::vector<std::string> deferred_warnings;
  tl_heap->forEachObject([&](const ObjectData* obj) {
    if (!isObjprofRoot(obj, (ObjprofFlags)flags, exclude_classes)) return;
    if (obj->hasZeroRefs()) return;
    ObjprofState env{
      .source = nullptr,
      .stack = std::nullopt,
      .paths = std::nullopt,
      .val_stack = ObjprofValuePtrStack{},
      .exclude_classes = exclude_classes,
      .flags = (ObjprofFlags)flags,
      .deferred_warnings = deferred_warnings
    };
    auto objsizePair = getObjSize(
      obj, env,
      objprof_props_mode ? &histogram : nullptr
    );

    if (!objprof_props_mode) {
      auto cls = obj->getVMClass();
      auto& metrics = histogram[std::make_pair(cls->name()->toCppString(), "")];
      metrics.instances += 1;
      metrics.bytes += objsizePair.first;
      metrics.bytes_rel += objsizePair.second;
    }
    // Gather Memoized methods data
    attributeMemoizedFootprint(obj, &histogram, objprof_props_mode, seen_caches, env);
  });
  issueWarnings(deferred_warnings);

  ObjprofState env{
    .source = nullptr,
    .stack = std::nullopt,
    .paths = std::nullopt,
    .val_stack = ObjprofValuePtrStack{},
    .exclude_classes = exclude_classes,
    .flags = (ObjprofFlags)flags,
    .deferred_warnings = deferred_warnings
  };
  // Finished iterating over objects, now gather static memoized functions
  attributeStaticMemoizedFootprint(&histogram, objprof_props_mode, env);
  attributeStaticPropFootprint(&histogram, objprof_props_mode, env);

  // Create response
  DictInit objs(histogram.size() + 1);
  for (auto const& it : histogram) {
    auto c = it.first;
    auto cls = c.first;
    auto prop = c.second;
    auto key = cls;
    if (prop != "") {
      key += "::" + c.second;
    }

    auto metrics_val = make_dict_array(
      s_instances, Variant(it.second.instances),
      s_bytes, Variant(it.second.bytes),
      s_bytes_rel, it.second.bytes_rel,
      s_paths, init_null()
    );

    objs.set(StrNR(key), Variant(metrics_val));
  }

  return objs.toArray();

}

Array HHVM_FUNCTION(objprof_get_paths,
  int64_t flags = ObjprofFlags::DEFAULT,
  const Array& exclude_list = Array()
) {
  hphp_fast_map<ClassProp, ObjprofMetrics> histogram;
  PathsToClass pathsToClass;

  // Create a set of std::strings from the exclude_list provided. This de-dups
  // the exclude_list, and also provides for fast lookup when determining
  // whether a given class is in the exclude_list
  hphp_fast_set<std::string> exclude_classes;
  for (ArrayIter iter(exclude_list); iter; ++iter) {
    exclude_classes.insert(iter.second().toString().data());
  }

  std::vector<std::string> deferred_warnings;
  tl_heap->forEachObject([&](const ObjectData* obj) {
      if (!isObjprofRoot(obj, (ObjprofFlags)flags, exclude_classes)) return;
      if (obj->hasZeroRefs()) return;
      auto cls = obj->getVMClass();
      auto& metrics = histogram[std::make_pair(cls->name()->toCppString(), "")];
      ObjprofState env{
        .source = obj,
        .stack = ObjprofStack{},
        .paths = PathsToObject{},
        .val_stack = ObjprofValuePtrStack{},
        .exclude_classes = exclude_classes,
        .flags = (ObjprofFlags)flags,
        .deferred_warnings = deferred_warnings
      };
      auto objsizePair = getObjSize(
        obj, /* obj */
        env,
        nullptr /* histogram */
      );
      metrics.instances += 1;
      metrics.bytes += objsizePair.first;
      metrics.bytes_rel += objsizePair.second;
      for (auto const& pathsIt : *env.paths) {
        auto cls_vm = pathsIt.first->getVMClass();
        auto& paths = pathsIt.second;
        auto& aggPaths = pathsToClass[cls_vm->name()->toCppString()];
        for (auto const& pathKV : paths) {
          auto& path = pathKV.first;
          auto& referral = pathKV.second;
          auto& aggReferral = aggPaths[path];
          aggReferral.refs += referral.refs;
          aggReferral.sources.insert(obj->getVMClass());
        }
      }

      FTRACE(1, "ObjectData* at {} ({}) size={}:{}\n",
       obj,
       obj->getClassName().data(),
       objsizePair.first,
       objsizePair.second
      );
      assertx(env.stack->size() == 0);
  });
  issueWarnings(deferred_warnings);

  NamedType::foreach_class([&](Class* cls) {
    if (cls->needsInitSProps()) {
      return;
    }
    auto const staticProps = cls->staticProperties();
    auto const nSProps = cls->numStaticProperties();
    for (Slot i = 0; i < nSProps; ++i) {
      auto const& prop = staticProps[i];
      auto tv = cls->getSPropData(i);
      if (tv == nullptr || tv->m_type == KindOfUninit) {
        continue;
      }

      FTRACE(2, "Traversing static prop {}::{}\n",
        cls->name()->data(),
        StrNR(prop.name).data()
      );

      ObjprofState env{
        .source = nullptr,
        .stack = ObjprofStack{},
        .paths = PathsToObject{},
        .val_stack = ObjprofValuePtrStack{},
        .exclude_classes = exclude_classes,
        .flags = (ObjprofFlags)flags,
        .deferred_warnings = deferred_warnings
      };

      auto refname = std::string(
        "ClassProperty:" +
        cls->name()->toCppString() + ":" +
        StrNR(prop.name).data()
      );

      if (tv->m_data.num == 0) {
        continue;
      }

      env.stack->push_back(refname);
      tvGetSize(*tv, env);
      env.stack->pop_back();

      for (auto const& pathsIt : *env.paths) {
        auto cls_vm = pathsIt.first->getVMClass();
        auto& paths = pathsIt.second;
        auto& aggPaths = pathsToClass[cls_vm->name()->toCppString()];
        for (auto const& pathKV : paths) {
          auto& path = pathKV.first;
          auto& referral = pathKV.second;
          auto& aggReferral = aggPaths[path];
          aggReferral.refs += referral.refs;
          aggReferral.sources.insert(cls_vm);
        }
      }
      assertx(env.stack->size() == 0);
    }
  });
  issueWarnings(deferred_warnings);

  // Create response
  DictInit objs(histogram.size());
  for (auto const& it : histogram) {
    auto c = it.first;
    auto clsPaths = pathsToClass[c.first];
    DictInit pathsArr(clsPaths.size());
    for (auto const& pathIt : clsPaths) {
      auto pathStr = pathIt.first;
      auto path_metrics_val = make_dict_array(
        s_refs, pathIt.second.refs
      );

      pathsArr.setValidKey(Variant(pathStr), Variant(path_metrics_val));
    }

    auto metrics_val = make_dict_array(
      s_instances, Variant(it.second.instances),
      s_bytes, Variant(it.second.bytes),
      s_bytes_rel, it.second.bytes_rel,
      s_paths, Variant(pathsArr.toArray())
    );

    objs.set(StringData::Make(c.first.c_str()), Variant(metrics_val));
  }

  return objs.toArray();
}

///////////////////////////////////////////////////////////////////////////////

size_t get_thread_stack_size() {
  auto sp = stack_top_ptr();
  return s_stackLimit + s_stackSize - uintptr_t(sp);
}

size_t get_thread_stack_peak_size() {
  size_t consecutive = 0;
  size_t total = 0;
  uint8_t marker = 0x00;
  uint8_t* cursor = &marker;
  uintptr_t cursor_p = uintptr_t(&marker);
  for (;cursor_p > s_stackLimit; cursor_p--, cursor--) {
    total++;
    if (*cursor == 0x00) {
      if (++consecutive == s_pageSize) {
        return get_thread_stack_size() + total - consecutive;
      }
    } else {
      consecutive = 0;
    }
  }

  return s_stackSize;
}

void HHVM_FUNCTION(thread_mark_stack, void) {
  size_t consecutive = 0;
  uint8_t marker = 0x00;
  uint8_t* cursor = &marker;
  uintptr_t cursor_p = uintptr_t(&marker);
  for (;cursor_p > s_stackLimit; cursor_p--, cursor--) {
    if (*cursor == 0x00) {
      if (++consecutive == s_pageSize) {
        return;
      }
    } else {
      consecutive = 0;
      *cursor = 0x00;
    }
  }
}

Array HHVM_FUNCTION(thread_memory_stats, void) {
  auto stack_size = get_thread_stack_size();
  auto stack_size_peak = get_thread_stack_peak_size();

  auto stats = make_dict_array(
      s_cpp_stack, Variant(stack_size),
      s_cpp_stack_peak, Variant(stack_size_peak)
  );

  return stats;
}

///////////////////////////////////////////////////////////////////////////////

void HHVM_FUNCTION(set_mem_threshold_callback,
  int64_t threshold,
  const Variant& callback
) {
  // In a similar way to fb_setprofile storing in m_setprofileCallback
  g_context->m_memThresholdCallback = callback;

  // Notify MM that surprise flag should be set upon reaching the threshold
  tl_heap->setMemThresholdCallback(threshold);
}

///////////////////////////////////////////////////////////////////////////////

}

struct objprofExtension final : Extension {
  objprofExtension() : Extension("objprof", "1.0", NO_ONCALL_YET) { }

  void moduleRegisterNative() override {
    HHVM_FALIAS(HH\\objprof_get_data, objprof_get_data);
    HHVM_FALIAS(HH\\objprof_get_data_extended, objprof_get_data_extended);
    HHVM_FALIAS(HH\\objprof_get_paths, objprof_get_paths);
    HHVM_FALIAS(HH\\thread_memory_stats, thread_memory_stats);
    HHVM_FALIAS(HH\\thread_mark_stack, thread_mark_stack);
    HHVM_FALIAS(HH\\set_mem_threshold_callback, set_mem_threshold_callback);
    HHVM_RC_INT(OBJPROF_FLAGS_DEFAULT, ObjprofFlags::DEFAULT);
    HHVM_RC_INT(OBJPROF_FLAGS_USER_TYPES_ONLY, ObjprofFlags::USER_TYPES_ONLY);
    HHVM_RC_INT(OBJPROF_FLAGS_PER_PROPERTY, ObjprofFlags::PER_PROPERTY);
  }
} s_objprof_extension;


///////////////////////////////////////////////////////////////////////////////
}

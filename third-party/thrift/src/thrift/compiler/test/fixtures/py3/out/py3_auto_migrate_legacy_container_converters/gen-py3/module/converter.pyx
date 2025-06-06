
#
# Autogenerated by Thrift for thrift/compiler/test/fixtures/py3/src/module.thrift
#
# DO NOT EDIT UNLESS YOU ARE SURE THAT YOU KNOW WHAT YOU ARE DOING
#  @generated
#

from libcpp.memory cimport make_shared, unique_ptr
from cython.operator cimport dereference as deref, address
from libcpp.utility cimport move as cmove
from thrift.py3.types cimport const_pointer_cast
cimport module.thrift_converter as _module_thrift_converter

import module.types as _module_types



cdef shared_ptr[_fbthrift_cbindings.cSimpleException] SimpleException_convert_to_cpp(object inst) except*:
    return make_shared[_fbthrift_cbindings.cSimpleException](
        _module_thrift_converter.SimpleException_convert_to_cpp(inst)
    )
cdef object SimpleException_from_cpp(const shared_ptr[_fbthrift_cbindings.cSimpleException]& c_struct):
    _py_struct = _module_thrift_converter.SimpleException_from_cpp(deref(const_pointer_cast(c_struct)))
    return _py_struct

cdef shared_ptr[_fbthrift_cbindings.cOptionalRefStruct] OptionalRefStruct_convert_to_cpp(object inst) except*:
    return make_shared[_fbthrift_cbindings.cOptionalRefStruct](
        _module_thrift_converter.OptionalRefStruct_convert_to_cpp(inst)
    )
cdef object OptionalRefStruct_from_cpp(const shared_ptr[_fbthrift_cbindings.cOptionalRefStruct]& c_struct):
    _py_struct = _module_thrift_converter.OptionalRefStruct_from_cpp(deref(const_pointer_cast(c_struct)))
    return _py_struct

cdef shared_ptr[_fbthrift_cbindings.cSimpleStruct] SimpleStruct_convert_to_cpp(object inst) except*:
    return make_shared[_fbthrift_cbindings.cSimpleStruct](
        _module_thrift_converter.SimpleStruct_convert_to_cpp(inst)
    )
cdef object SimpleStruct_from_cpp(const shared_ptr[_fbthrift_cbindings.cSimpleStruct]& c_struct):
    _py_struct = _module_thrift_converter.SimpleStruct_from_cpp(deref(const_pointer_cast(c_struct)))
    return _py_struct

cdef shared_ptr[_fbthrift_cbindings.cHiddenTypeFieldsStruct] HiddenTypeFieldsStruct_convert_to_cpp(object inst) except*:
    return make_shared[_fbthrift_cbindings.cHiddenTypeFieldsStruct](
        _module_thrift_converter.HiddenTypeFieldsStruct_convert_to_cpp(inst)
    )
cdef object HiddenTypeFieldsStruct_from_cpp(const shared_ptr[_fbthrift_cbindings.cHiddenTypeFieldsStruct]& c_struct):
    _py_struct = _module_thrift_converter.HiddenTypeFieldsStruct_from_cpp(deref(const_pointer_cast(c_struct)))
    return _py_struct

cdef shared_ptr[_fbthrift_cbindings.cComplexStruct] ComplexStruct_convert_to_cpp(object inst) except*:
    return make_shared[_fbthrift_cbindings.cComplexStruct](
        _module_thrift_converter.ComplexStruct_convert_to_cpp(inst)
    )
cdef object ComplexStruct_from_cpp(const shared_ptr[_fbthrift_cbindings.cComplexStruct]& c_struct):
    _py_struct = _module_thrift_converter.ComplexStruct_from_cpp(deref(const_pointer_cast(c_struct)))
    return _py_struct

cdef shared_ptr[_fbthrift_cbindings.cBinaryUnion] BinaryUnion_convert_to_cpp(object inst) except*:
    return make_shared[_fbthrift_cbindings.cBinaryUnion](
        _module_thrift_converter.BinaryUnion_convert_to_cpp(inst)
    )
cdef object BinaryUnion_from_cpp(const shared_ptr[_fbthrift_cbindings.cBinaryUnion]& c_struct):
    _py_struct = _module_thrift_converter.BinaryUnion_from_cpp(deref(const_pointer_cast(c_struct)))
    return _py_struct

cdef shared_ptr[_fbthrift_cbindings.cBinaryUnionStruct] BinaryUnionStruct_convert_to_cpp(object inst) except*:
    return make_shared[_fbthrift_cbindings.cBinaryUnionStruct](
        _module_thrift_converter.BinaryUnionStruct_convert_to_cpp(inst)
    )
cdef object BinaryUnionStruct_from_cpp(const shared_ptr[_fbthrift_cbindings.cBinaryUnionStruct]& c_struct):
    _py_struct = _module_thrift_converter.BinaryUnionStruct_from_cpp(deref(const_pointer_cast(c_struct)))
    return _py_struct

cdef shared_ptr[_fbthrift_cbindings.cCustomFields] CustomFields_convert_to_cpp(object inst) except*:
    return make_shared[_fbthrift_cbindings.cCustomFields](
        _module_thrift_converter.CustomFields_convert_to_cpp(inst)
    )
cdef object CustomFields_from_cpp(const shared_ptr[_fbthrift_cbindings.cCustomFields]& c_struct):
    _py_struct = _module_thrift_converter.CustomFields_from_cpp(deref(const_pointer_cast(c_struct)))
    return _py_struct

cdef shared_ptr[_fbthrift_cbindings.cCustomTypedefFields] CustomTypedefFields_convert_to_cpp(object inst) except*:
    return make_shared[_fbthrift_cbindings.cCustomTypedefFields](
        _module_thrift_converter.CustomTypedefFields_convert_to_cpp(inst)
    )
cdef object CustomTypedefFields_from_cpp(const shared_ptr[_fbthrift_cbindings.cCustomTypedefFields]& c_struct):
    _py_struct = _module_thrift_converter.CustomTypedefFields_from_cpp(deref(const_pointer_cast(c_struct)))
    return _py_struct

cdef shared_ptr[_fbthrift_cbindings.cAdaptedTypedefFields] AdaptedTypedefFields_convert_to_cpp(object inst) except*:
    return make_shared[_fbthrift_cbindings.cAdaptedTypedefFields](
        _module_thrift_converter.AdaptedTypedefFields_convert_to_cpp(inst)
    )
cdef object AdaptedTypedefFields_from_cpp(const shared_ptr[_fbthrift_cbindings.cAdaptedTypedefFields]& c_struct):
    _py_struct = _module_thrift_converter.AdaptedTypedefFields_from_cpp(deref(const_pointer_cast(c_struct)))
    return _py_struct


cdef vector[cint16_t] List__i16__make_instance(object items) except *:
        cdef vector[cint16_t] c_inst
        if items is None:
            return cmove(c_inst)
        for item in items:
            if not isinstance(item, int):
                raise TypeError(f"{item!r} is not of type int")
            item = <cint16_t> item
            c_inst.push_back(item)
        return cmove(c_inst)

cdef vector[cint32_t] List__i32__make_instance(object items) except *:
        cdef vector[cint32_t] c_inst
        if items is None:
            return cmove(c_inst)
        for item in items:
            if not isinstance(item, int):
                raise TypeError(f"{item!r} is not of type int")
            item = <cint32_t> item
            c_inst.push_back(item)
        return cmove(c_inst)

cdef vector[cint64_t] List__i64__make_instance(object items) except *:
        cdef vector[cint64_t] c_inst
        if items is None:
            return cmove(c_inst)
        for item in items:
            if not isinstance(item, int):
                raise TypeError(f"{item!r} is not of type int")
            item = <cint64_t> item
            c_inst.push_back(item)
        return cmove(c_inst)

cdef vector[string] List__string__make_instance(object items) except *:
        cdef vector[string] c_inst
        if items is None:
            return cmove(c_inst)
        if isinstance(items, str):
            raise TypeError("If you really want to pass a string into a _typing.Sequence[str] field, explicitly convert it first.")
        for item in items:
            if not isinstance(item, str):
                raise TypeError(f"{item!r} is not of type str")
            c_inst.push_back(item.encode('UTF-8'))
        return cmove(c_inst)

cdef vector[_module_cbindings.cSimpleStruct] List__SimpleStruct__make_instance(object items) except *:
        cdef vector[_module_cbindings.cSimpleStruct] c_inst
        if items is None:
            return cmove(c_inst)
        for item in items:
            if not isinstance(item, _module_types.SimpleStruct):
                raise TypeError(f"{item!r} is not of type _module_types.SimpleStruct")
            c_inst.push_back(_module_thrift_converter.SimpleStruct_convert_to_cpp(item))
        return cmove(c_inst)

cdef cset[cint32_t] Set__i32__make_instance(object items) except *:
        cdef cset[cint32_t] c_inst
        cdef cint32_t c_item
        if items is None:
            return cmove(c_inst)
        for item in items:
            if not isinstance(item, int):
                raise TypeError(f"{item!r} is not of type int")
            c_item = <cint32_t> item
            c_inst.insert(c_item)
        return cmove(c_inst)

cdef cset[string] Set__string__make_instance(object items) except *:
        cdef cset[string] c_inst
        cdef string c_item
        if items is None:
            return cmove(c_inst)
        if isinstance(items, str):
            raise TypeError("If you really want to pass a string into a _typing.AbstractSet[str] field, explicitly convert it first.")
        for item in items:
            if not isinstance(item, str):
                raise TypeError(f"{item!r} is not of type str")
            c_item = item.encode('UTF-8')
            c_inst.insert(c_item)
        return cmove(c_inst)

cdef cmap[string,string] Map__string_string__make_instance(object items) except *:
        cdef cmap[string,string] c_inst
        cdef string c_key
        if items is None:
            return cmove(c_inst)
        for key, item in items.items():
            if not isinstance(key, str):
                raise TypeError(f"{key!r} is not of type str")
            c_key = key.encode('UTF-8')
            if not isinstance(item, str):
                raise TypeError(f"{item!r} is not of type str")
    
            c_inst[c_key] = item.encode('UTF-8')
        return cmove(c_inst)

cdef cmap[string,_module_cbindings.cSimpleStruct] Map__string_SimpleStruct__make_instance(object items) except *:
        cdef cmap[string,_module_cbindings.cSimpleStruct] c_inst
        cdef string c_key
        if items is None:
            return cmove(c_inst)
        for key, item in items.items():
            if not isinstance(key, str):
                raise TypeError(f"{key!r} is not of type str")
            c_key = key.encode('UTF-8')
            if not isinstance(item, _module_types.SimpleStruct):
                raise TypeError(f"{item!r} is not of type _module_types.SimpleStruct")
    
            c_inst[c_key] = _module_thrift_converter.SimpleStruct_convert_to_cpp(item)
        return cmove(c_inst)

cdef cmap[string,cint16_t] Map__string_i16__make_instance(object items) except *:
        cdef cmap[string,cint16_t] c_inst
        cdef string c_key
        if items is None:
            return cmove(c_inst)
        for key, item in items.items():
            if not isinstance(key, str):
                raise TypeError(f"{key!r} is not of type str")
            c_key = key.encode('UTF-8')
            if not isinstance(item, int):
                raise TypeError(f"{item!r} is not of type int")
            item = <cint16_t> item
    
            c_inst[c_key] = item
        return cmove(c_inst)

cdef vector[vector[cint32_t]] List__List__i32__make_instance(object items) except *:
        cdef vector[vector[cint32_t]] c_inst
        if items is None:
            return cmove(c_inst)
        for item in items:
            if item is None:
                raise TypeError("None is not of the type _typing.Sequence[int]")
            if not isinstance(item, _module_types.List__i32):
                item = _module_types.List__i32(item)
            c_inst.push_back(List__i32__make_instance(item))
        return cmove(c_inst)

cdef cmap[string,cint32_t] Map__string_i32__make_instance(object items) except *:
        cdef cmap[string,cint32_t] c_inst
        cdef string c_key
        if items is None:
            return cmove(c_inst)
        for key, item in items.items():
            if not isinstance(key, str):
                raise TypeError(f"{key!r} is not of type str")
            c_key = key.encode('UTF-8')
            if not isinstance(item, int):
                raise TypeError(f"{item!r} is not of type int")
            item = <cint32_t> item
    
            c_inst[c_key] = item
        return cmove(c_inst)

cdef cmap[string,cmap[string,cint32_t]] Map__string_Map__string_i32__make_instance(object items) except *:
        cdef cmap[string,cmap[string,cint32_t]] c_inst
        cdef string c_key
        if items is None:
            return cmove(c_inst)
        for key, item in items.items():
            if not isinstance(key, str):
                raise TypeError(f"{key!r} is not of type str")
            c_key = key.encode('UTF-8')
            if item is None:
                raise TypeError("None is not of type _typing.Mapping[str, int]")
            if not isinstance(item, _module_types.Map__string_i32):
                item = _module_types.Map__string_i32(item)
    
            c_inst[c_key] = Map__string_i32__make_instance(item)
        return cmove(c_inst)

cdef vector[cset[string]] List__Set__string__make_instance(object items) except *:
        cdef vector[cset[string]] c_inst
        if items is None:
            return cmove(c_inst)
        for item in items:
            if item is None:
                raise TypeError("None is not of the type _typing.AbstractSet[str]")
            if not isinstance(item, _module_types.Set__string):
                item = _module_types.Set__string(item)
            c_inst.push_back(Set__string__make_instance(item))
        return cmove(c_inst)

cdef cmap[string,vector[_module_cbindings.cSimpleStruct]] Map__string_List__SimpleStruct__make_instance(object items) except *:
        cdef cmap[string,vector[_module_cbindings.cSimpleStruct]] c_inst
        cdef string c_key
        if items is None:
            return cmove(c_inst)
        for key, item in items.items():
            if not isinstance(key, str):
                raise TypeError(f"{key!r} is not of type str")
            c_key = key.encode('UTF-8')
            if item is None:
                raise TypeError("None is not of type _typing.Sequence[_module_types.SimpleStruct]")
            if not isinstance(item, _module_types.List__SimpleStruct):
                item = _module_types.List__SimpleStruct(item)
    
            c_inst[c_key] = List__SimpleStruct__make_instance(item)
        return cmove(c_inst)

cdef vector[vector[string]] List__List__string__make_instance(object items) except *:
        cdef vector[vector[string]] c_inst
        if items is None:
            return cmove(c_inst)
        for item in items:
            if item is None:
                raise TypeError("None is not of the type _typing.Sequence[str]")
            if not isinstance(item, _module_types.List__string):
                item = _module_types.List__string(item)
            c_inst.push_back(List__string__make_instance(item))
        return cmove(c_inst)

cdef vector[cset[cint32_t]] List__Set__i32__make_instance(object items) except *:
        cdef vector[cset[cint32_t]] c_inst
        if items is None:
            return cmove(c_inst)
        for item in items:
            if item is None:
                raise TypeError("None is not of the type _typing.AbstractSet[int]")
            if not isinstance(item, _module_types.Set__i32):
                item = _module_types.Set__i32(item)
            c_inst.push_back(Set__i32__make_instance(item))
        return cmove(c_inst)

cdef vector[cmap[string,string]] List__Map__string_string__make_instance(object items) except *:
        cdef vector[cmap[string,string]] c_inst
        if items is None:
            return cmove(c_inst)
        for item in items:
            if item is None:
                raise TypeError("None is not of the type _typing.Mapping[str, str]")
            if not isinstance(item, _module_types.Map__string_string):
                item = _module_types.Map__string_string(item)
            c_inst.push_back(Map__string_string__make_instance(item))
        return cmove(c_inst)

cdef vector[string] List__binary__make_instance(object items) except *:
        cdef vector[string] c_inst
        if items is None:
            return cmove(c_inst)
        if isinstance(items, str):
            raise TypeError("If you really want to pass a string into a _typing.Sequence[bytes] field, explicitly convert it first.")
        for item in items:
            if not isinstance(item, bytes):
                raise TypeError(f"{item!r} is not of type bytes")
            c_inst.push_back(item)
        return cmove(c_inst)

cdef cset[string] Set__binary__make_instance(object items) except *:
        cdef cset[string] c_inst
        cdef string c_item
        if items is None:
            return cmove(c_inst)
        if isinstance(items, str):
            raise TypeError("If you really want to pass a string into a _typing.AbstractSet[bytes] field, explicitly convert it first.")
        for item in items:
            if not isinstance(item, bytes):
                raise TypeError(f"{item!r} is not of type bytes")
            c_item = item
            c_inst.insert(c_item)
        return cmove(c_inst)

cdef vector[_module_cbindings.cAnEnum] List__AnEnum__make_instance(object items) except *:
        cdef vector[_module_cbindings.cAnEnum] c_inst
        if items is None:
            return cmove(c_inst)
        for item in items:
            if not isinstance(item, _module_types.AnEnum):
                raise TypeError(f"{item!r} is not of type _module_types.AnEnum")
            c_inst.push_back(<_module_cbindings.cAnEnum><int>item)
        return cmove(c_inst)

cdef _module_cbindings._std_unordered_map[cint32_t,cint32_t] _std_unordered_map__Map__i32_i32__make_instance(object items) except *:
        cdef _module_cbindings._std_unordered_map[cint32_t,cint32_t] c_inst
        cdef cint32_t c_key
        if items is None:
            return cmove(c_inst)
        for key, item in items.items():
            if not isinstance(key, int):
                raise TypeError(f"{key!r} is not of type int")
            c_key = <cint32_t> key
            if not isinstance(item, int):
                raise TypeError(f"{item!r} is not of type int")
            item = <cint32_t> item
    
            c_inst[c_key] = item
        return cmove(c_inst)

cdef _module_cbindings._MyType _MyType__List__i32__make_instance(object items) except *:
        cdef _module_cbindings._MyType c_inst
        if items is None:
            return cmove(c_inst)
        for item in items:
            if not isinstance(item, int):
                raise TypeError(f"{item!r} is not of type int")
            item = <cint32_t> item
            c_inst.push_back(item)
        return cmove(c_inst)

cdef _module_cbindings._MyType _MyType__Set__i32__make_instance(object items) except *:
        cdef _module_cbindings._MyType c_inst
        cdef cint32_t c_item
        if items is None:
            return cmove(c_inst)
        for item in items:
            if not isinstance(item, int):
                raise TypeError(f"{item!r} is not of type int")
            c_item = <cint32_t> item
            c_inst.insert(c_item)
        return cmove(c_inst)

cdef _module_cbindings._MyType _MyType__Map__i32_i32__make_instance(object items) except *:
        cdef _module_cbindings._MyType c_inst
        cdef cint32_t c_key
        if items is None:
            return cmove(c_inst)
        for key, item in items.items():
            if not isinstance(key, int):
                raise TypeError(f"{key!r} is not of type int")
            c_key = <cint32_t> key
            if not isinstance(item, int):
                raise TypeError(f"{item!r} is not of type int")
            item = <cint32_t> item
    
            c_inst[c_key] = item
        return cmove(c_inst)

cdef _module_cbindings._py3_simple_AdaptedList _py3_simple_AdaptedList__List__i32__make_instance(object items) except *:
        cdef _module_cbindings._py3_simple_AdaptedList c_inst
        if items is None:
            return cmove(c_inst)
        for item in items:
            if not isinstance(item, int):
                raise TypeError(f"{item!r} is not of type int")
            item = <cint32_t> item
            c_inst.push_back(item)
        return cmove(c_inst)

cdef _module_cbindings._py3_simple_AdaptedSet _py3_simple_AdaptedSet__Set__i32__make_instance(object items) except *:
        cdef _module_cbindings._py3_simple_AdaptedSet c_inst
        cdef cint32_t c_item
        if items is None:
            return cmove(c_inst)
        for item in items:
            if not isinstance(item, int):
                raise TypeError(f"{item!r} is not of type int")
            c_item = <cint32_t> item
            c_inst.insert(c_item)
        return cmove(c_inst)

cdef _module_cbindings._py3_simple_AdaptedMap _py3_simple_AdaptedMap__Map__i32_i32__make_instance(object items) except *:
        cdef _module_cbindings._py3_simple_AdaptedMap c_inst
        cdef cint32_t c_key
        if items is None:
            return cmove(c_inst)
        for key, item in items.items():
            if not isinstance(key, int):
                raise TypeError(f"{key!r} is not of type int")
            c_key = <cint32_t> key
            if not isinstance(item, int):
                raise TypeError(f"{item!r} is not of type int")
            item = <cint32_t> item
    
            c_inst[c_key] = item
        return cmove(c_inst)

cdef cmap[cint32_t,double] Map__i32_double__make_instance(object items) except *:
        cdef cmap[cint32_t,double] c_inst
        cdef cint32_t c_key
        if items is None:
            return cmove(c_inst)
        for key, item in items.items():
            if not isinstance(key, int):
                raise TypeError(f"{key!r} is not of type int")
            c_key = <cint32_t> key
            if not isinstance(item, (float, int)):
                raise TypeError(f"{item!r} is not of type float")
    
            c_inst[c_key] = item
        return cmove(c_inst)

cdef vector[cmap[cint32_t,double]] List__Map__i32_double__make_instance(object items) except *:
        cdef vector[cmap[cint32_t,double]] c_inst
        if items is None:
            return cmove(c_inst)
        for item in items:
            if item is None:
                raise TypeError("None is not of the type _typing.Mapping[int, float]")
            if not isinstance(item, _module_types.Map__i32_double):
                item = _module_types.Map__i32_double(item)
            c_inst.push_back(Map__i32_double__make_instance(item))
        return cmove(c_inst)

cdef cmap[_module_cbindings.cAnEnumRenamed,cint32_t] Map__AnEnumRenamed_i32__make_instance(object items) except *:
        cdef cmap[_module_cbindings.cAnEnumRenamed,cint32_t] c_inst
        cdef _module_cbindings.cAnEnumRenamed c_key
        if items is None:
            return cmove(c_inst)
        for key, item in items.items():
            if not isinstance(key, _module_types.AnEnumRenamed):
                raise TypeError(f"{key!r} is not of type _module_types.AnEnumRenamed")
            c_key = <_module_cbindings.cAnEnumRenamed><int>key
            if not isinstance(item, int):
                raise TypeError(f"{item!r} is not of type int")
            item = <cint32_t> item
    
            c_inst[c_key] = item
        return cmove(c_inst)


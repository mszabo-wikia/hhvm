/*
   +----------------------------------------------------------------------+
   | HipHop for PHP                                                       |
   +----------------------------------------------------------------------+
   | Copyright (c) 2010-present Facebook, Inc. (http://www.facebook.com)  |
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

#ifndef incl_HPHP_OFFLINE_TRANS_DATA_
#define incl_HPHP_OFFLINE_TRANS_DATA_

#include <map>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "hphp/runtime/vm/jit/translator.h"
#include "hphp/runtime/vm/func.h"

#include "hphp/tools/tc-print/perf-events.h"

#define INVALID_ID ((TransID)-1)

namespace HPHP { namespace jit {

struct OfflineCode;
struct RepoWrapper;

typedef char SHA1Str[41];

struct TransAddrRange {
  TCA    start;
  TCA    end;
  TransID transId;
  TransAddrRange(TCA _start, TCA _end, TransID _id) :
      start(_start), end(_end), transId(_id) {}
  bool operator< (const TransAddrRange &other) const {
    return start < other.start;
  }
};

struct OfflineTransData {
  explicit OfflineTransData(const std::string& dumpDir)
    : dumpDir(dumpDir) {
    loadTCHeader();
  }

  uint32_t getNumTrans() const {
    return nTranslations;
  }

  uint32_t getNumFuncs() const {
    return funcIds.size();
  }

  const TransRec* getTransRec(TransID id) const {
    assert(id < translations.size());
    return &translations[id];
  }

  void addTrans(TransRec& transRec) {
    translations.push_back(transRec);
    preds.push_back(TransIDSet());
    succs.push_back(TransIDSet());
  }

  void addControlArc(TransID srcId, TransID destId) {
    assert(srcId  < nTranslations);
    assert(destId < nTranslations);
    succs[srcId].insert(destId);
    preds[destId].insert(srcId);
  }

  // Adds all control arcs among translations for 'selectedFuncId' by
  // disassembling transCode
  void addControlArcs(uint32_t selectedFuncId, OfflineCode *transCode);

  const TransIDSet& getTransPreds(TransID transId) const {
    assert(transId < preds.size());
    return preds[transId];
  }

  const TransIDSet& getTransSuccs(TransID transId) const {
    assert(transId < succs.size());
    return succs[transId];
  }

  // Returns the id of the translation containing the given address,
  // or INVALID_ID if none.
  TransID getTransContaining(TCA addr) const;

  // Returns the id of the translation starting at the given address,
  // or INVALID_ID if none.
  TransID getTransStartingAt(TCA startAddr) {
    Addr2TransMap::iterator it = addr2TransMap.find(startAddr);
    if (it == addr2TransMap.end()) return INVALID_ID;
    return it->second;
  }

  const char * getRepoSchema() {
    return repoSchema;
  }

  TCA getMainBase() const {
    return aBase;
  }

  TCA getMainFrontier() const {
    return aFrontier;
  }

  TCA getColdBase() const {
    return coldBase;
  }

  TCA getColdFrontier() const {
    return coldFrontier;
  }

  TCA getFrozenBase() const {
    return frozenBase;
  }

  TCA getFrozenFrontier() const {
    return frozenFrontier;
  }

  // Find translations that belong to the selectedFuncId
  void findFuncTrans(uint32_t selectedFuncId, std::vector<TransID> *inodes);

  void printTransRec(TransID transId, const PerfEventsMap<TransID>& transStats);

  bool isAddrInSomeTrans(TCA addr) const {
    if ((aBase      <= addr && addr < aFrontier)     ||
        (coldBase   <= addr && addr < coldFrontier)  ||
        (frozenBase <= addr && addr < frozenFrontier)) {
      return getTransContaining(addr) != INVALID_ID;
    }
    return false;
  }

  void setAnnotationsVerbosity(uint32_t level) {
    annotationsVerbosity = level;
  }

  void loadTCData(RepoWrapper* repoWrapper);

private:
  uint32_t                    headerSize;
  uint32_t                    nTranslations;
  std::vector<TransRec>       translations;
  std::vector<TransIDSet>     preds;
  std::vector<TransIDSet>     succs;

  std::vector<TransAddrRange> transAddrRanges;

  // Maps start addresses of translations to the corresponding translation id
  typedef std::unordered_map<TCA, TransID> Addr2TransMap;
  Addr2TransMap               addr2TransMap;

  char                        repoSchema[100];
  TCA                         aBase;
  TCA                         aFrontier;
  TCA                         coldBase;
  TCA                         coldFrontier;
  TCA                         frozenBase;
  TCA                         frozenFrontier;

  std::unordered_set<FuncId> funcIds;

  std::string                 dumpDir;

  uint32_t                    annotationsVerbosity;

  void loadTCHeader();
};

} }

#endif

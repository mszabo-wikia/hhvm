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
#include "hphp/runtime/vm/debug/dwarf.h"

#include <stdio.h>
#include "hphp/runtime/vm/debug/debug.h"
#include "hphp/runtime/vm/debug/gdb-jit.h"
#include "hphp/runtime/vm/debug/elfwriter.h"

namespace HPHP {
namespace Debug {

int g_dwarfCallback(LIBDWARF_CALLBACK_NAME_TYPE name, int size,
                    Dwarf_Unsigned type, Dwarf_Unsigned flags,
                    Dwarf_Unsigned link, Dwarf_Unsigned info,
                    Dwarf_Unsigned* /*sect_name_index*/, Dwarf_Ptr handle,
                    int* /*error*/) {
  ElfWriter *e = reinterpret_cast<ElfWriter *>(handle);
  return e->dwarfCallback(name, size, type, flags, link, info);
}

void DwarfBuf::byte(uint8_t c) {
  m_buf.push_back(c);
}

void DwarfBuf::clear() {
  m_buf.clear();
}

int DwarfBuf::size() {
  return m_buf.size();
}

uint8_t *DwarfBuf::getBuf() {
  return m_buf.data();
}

void DwarfBuf::print() {
  unsigned int i;
  for (i = 0; i < m_buf.size(); i++)
    printf("%x ", m_buf[i]);
  printf("\n");
}

void DwarfBuf::dwarf_cfa_def_cfa(uint8_t reg, uint8_t offset) {
  byte(DW_CFA_def_cfa);
  byte(reg);
  byte(offset);
}

void DwarfBuf::dwarf_cfa_same_value(uint8_t reg) {
  byte(DW_CFA_same_value);
  byte(reg);
}

void DwarfBuf::dwarf_cfa_offset_extended_sf(uint8_t reg, int8_t offset) {
  byte(DW_CFA_offset_extended_sf);
  byte(reg);
  byte(offset & 0x7f);
}

const char *DwarfInfo::lookupFile(const Unit *unit) {
  const char *file = nullptr;
  if (unit && unit->origFilepath()) {
    file = unit->origFilepath()->data();
  }
  if (file == nullptr || strlen(file) == 0) {
    return "anonFile";
  }
  return file;
}

void DwarfInfo::addLineEntries(TCRange range, int lineNumber, FunctionInfo* f) {
  // For stubs and missing line numbers, just add line 0
  lineNumber = std::max(0, lineNumber);
  f->m_lineTable.push_back(LineEntry(range, lineNumber));
}

void DwarfInfo::transferFuncs(DwarfChunk* from, DwarfChunk* to) {
  unsigned int size = from->m_functions.size();
  for (unsigned int i = 0; i < size; i++) {
    FunctionInfo* f = from->m_functions[i];
    f->m_chunk = to;
    to->m_functions.push_back(f);
  }
}

void DwarfInfo::compactChunks() {
  unsigned int i, j;
  for (i = 1; i < m_dwarfChunks.size(); i++) {
    if (m_dwarfChunks[i] == nullptr) {
      break;
    }
  }
  if (i >= m_dwarfChunks.size()) {
    m_dwarfChunks.push_back(nullptr);
  }
  DwarfChunk* chunk = new DwarfChunk();
  for (j = 0; j < i; j++) {
    transferFuncs(m_dwarfChunks[j], chunk);
    // unregister chunk from gdb and free chunk
    unregister_gdb_chunk(m_dwarfChunks[j]);
    delete(m_dwarfChunks[j]);
    m_dwarfChunks[j] = nullptr;
  }
  m_dwarfChunks[i] = chunk;
  // register compacted chunk with gdb
  ElfWriter e = ElfWriter(chunk);
}

static Mutex s_lock(RankLeaf);

DwarfChunk* DwarfInfo::addTracelet(TCRange range,
                                   Optional<std::string> name,
                                   const Func *func,
                                   int lineNumber,
                                   bool inPrologue) {
  DwarfChunk* chunk = nullptr;
  FunctionInfo* f = new FunctionInfo(range, false);
  const Unit* unit = func ? func->unit(): nullptr;
  if (name) {
    f->name = *name;
  } else {
    assertx(func != nullptr);
    f->name = lookupFunction(func, inPrologue, true);
    auto names = func->localNames();
    for (int i = 0; i < func->numNamedLocals(); i++) {
      if (!names[i]) continue;
      f->m_namedLocals.push_back(names[i]->toCppString());
    }
  }
  f->file = lookupFile(unit);

  TCA start = range.begin();
  const TCA end = range.end();

  Lock lock(s_lock);
  auto const it = m_functions.lower_bound(range.begin());
  auto const fi = it->second;
  if (it != m_functions.end() && fi->name == f->name &&
      fi->file == f->file &&
      start > fi->range.begin() &&
      end > fi->range.end()) {
    // XXX: verify that overlapping address come from jmp fixups
    start = fi->range.end();
    fi->range.extend(end);
    m_functions[end] = fi;
    m_functions.erase(it);
    delete f;
    f = m_functions[end];
    assertx(f->m_chunk != nullptr);
    f->m_chunk->clearSynced();
    f->clearPerfSynced();
  } else {
    m_functions[end] = f;
  }

  addLineEntries(TCRange(start, end, range.isAcold()), lineNumber, f);

  if (f->m_chunk == nullptr) {
    if (m_dwarfChunks.size() == 0 || m_dwarfChunks[0] == nullptr) {
      // new chunk of base size
      chunk = new DwarfChunk();
      m_dwarfChunks.push_back(chunk);
    } else if (m_dwarfChunks[0]->m_functions.size()
                 < Cfg::Eval::GdbSyncChunks) {
      // reuse first chunk
      chunk = m_dwarfChunks[0];
      chunk->clearSynced();
    } else {
      // compact chunks
      compactChunks();
      m_dwarfChunks[0] = chunk = new DwarfChunk();
    }
    chunk->m_functions.push_back(f);
    f->m_chunk = chunk;
  }

  if (f->m_chunk->m_functions.size() >= Cfg::Eval::GdbSyncChunks) {
    ElfWriter e = ElfWriter(f->m_chunk);
  }

  return f->m_chunk;
}

void DwarfInfo::syncChunks() {
  unsigned int i;
  Lock lock(s_lock);
  for (i = 0; i < m_dwarfChunks.size(); i++) {
    if (m_dwarfChunks[i] && !m_dwarfChunks[i]->isSynced()) {
      unregister_gdb_chunk(m_dwarfChunks[i]);
      ElfWriter e = ElfWriter(m_dwarfChunks[i]);
    }
  }
}

}
}

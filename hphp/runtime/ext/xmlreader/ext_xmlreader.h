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

#pragma once

#include "hphp/runtime/ext/extension.h"
#include "hphp/runtime/base/file.h"
#include "hphp/runtime/base/req-ptr.h"
#include <libxml/tree.h>
#include <libxml/xmlreader.h>
#include <libxml/uri.h>

using xmlreader_read_int_t = int (*)(xmlTextReaderPtr reader);
using xmlreader_read_char_t = unsigned char *(*)(xmlTextReaderPtr reader);
using xmlreader_read_const_char_t = const unsigned char *(*)(xmlTextReaderPtr reader);
using xmlreader_read_one_char_t = unsigned char *(*)(xmlTextReaderPtr reader, const unsigned char *);

namespace HPHP {
///////////////////////////////////////////////////////////////////////////////
// class XMLReader

struct XMLReader {
  XMLReader();
  ~XMLReader();
  void sweep();
  void close();
  String read_string_func(xmlreader_read_char_t internal_function);
  bool bool_func_no_arg(xmlreader_read_int_t internal_function);
  Variant string_func_string_arg(String value,
                                 xmlreader_read_one_char_t internal_function);
  bool set_relaxng_schema(String source, int type);

  req::ptr<File> m_stream; // input stream
  xmlTextReaderPtr m_ptr;
  xmlParserInputBufferPtr m_input;
  void* m_schema; // really xmlRelaxNG*.
  TYPE_SCAN_IGNORE_FIELD(m_schema);
};

///////////////////////////////////////////////////////////////////////////////
}

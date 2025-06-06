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

#include "hphp/runtime/base/plain-file.h"
#include <zlib.h>

namespace HPHP {
///////////////////////////////////////////////////////////////////////////////

/**
 * zlib based files.
 */
struct ZipFile : File {
  DECLARE_RESOURCE_ALLOCATION(ZipFile)

  ZipFile();
  virtual ~ZipFile();

  // overriding ResourceData
  const String& o_getClassNameHook() const override { return classnameof(); }

  bool open(const String& filename, const String& mode) override;
  bool close(int* unused = nullptr) final;
  int64_t readImpl(char *buffer, int64_t length) override;
  int64_t writeImpl(const char *buffer, int64_t length) override;
  bool seekable() override { return true;}
  bool seek(int64_t offset, int whence = SEEK_SET) override;
  int64_t tell() override;
  bool eof() override;
  bool rewind() override;
  bool flush() override;

  // Proxy the lock to the underlying stream
  bool lock(int operation, bool &wouldblock) override {
    if (!m_innerFile || m_innerFile->isClosed()) {
      raise_warning("Inner file descriptor is closed");
      return false;
    }
    return m_innerFile->lock(operation, wouldblock);
  }
  bool lock(int operation) override {
    bool wouldBlock = false;
    return lock(operation, wouldBlock);
  }

private:
  gzFile m_gzFile;
  req::ptr<File> m_innerFile;
  req::ptr<File> m_tempFile;
};

///////////////////////////////////////////////////////////////////////////////
}


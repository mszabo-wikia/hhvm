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
#pragma once

#include "hphp/runtime/ext/extension.h"
#include "hphp/runtime/server/transport.h"
#include "hphp/runtime/server/virtual-host.h"
#include "hphp/util/thread-local.h"
#include "hphp/util/logger.h"
#include "hphp/util/lock.h"
#include "hphp/util/cronolog.h"
#include "hphp/util/log-file-flusher.h"


namespace HPHP {
///////////////////////////////////////////////////////////////////////////////
struct LogWriter;
enum class LogChannel {CRONOLOG, THREADLOCAL, REGULAR};

struct AccessLogFileData {
  AccessLogFileData() {}
  AccessLogFileData(const std::string& baseDir,
                    const std::string& fil,
                    const std::string& lnk,
                    const std::string& fmt,
                    int mpl);
  AccessLogFileData(const std::string& fil,
                    const std::string& lnk,
                    const std::string& fmt,
                    int mpl);
  std::string file;
  std::string symLink;
  std::string format;
  int periodMultiplier;
  // Concrete LogWriter factories to be used depending on the handle
  using factory_t = std::function<
    std::unique_ptr<LogWriter>(const AccessLogFileData&, LogChannel)>;
  // Ask a registered factory for a new LogWriter instance
  std::unique_ptr<LogWriter> Writer(LogChannel chan) const;
  static void registerWriter(const std::string& handle, factory_t factory);
private:
  std::string m_logOutputType;
  static Mutex m_lock;
  static std::unordered_map<std::string, factory_t> m_factories;
};


struct AccessLog {
  struct ThreadData {
    ThreadData() : log(nullptr) {}
    FILE *log;
    int64_t startTime;
    LogFileFlusher flusher;
  };
  using GetThreadDataFunc = ThreadData* (*)();
  explicit AccessLog(GetThreadDataFunc f) :
    m_initialized(false), m_fGetThreadData(f) {}
  ~AccessLog();

  void init(const std::string &defaultFormat,
            std::vector<AccessLogFileData> &files,
            const std::string &username);
  void init(const std::string &defaultFormat,
            std::map<std::string, AccessLogFileData> &files,
            const std::string &username);
  void init(const std::string &baseDir, const std::string &format,
            const std::string &symLink, const std::string &file,
            const std::string &username);
  void init(const std::string &format, const std::string &symLink,
            const std::string &file, const std::string &username);
  void log(Transport *transport, const VirtualHost *vhost);
  bool setThreadLog(const char *file);
  void clearThreadLog();
  void onNewRequest();
  void flushAllWriters();
  void fini();
private:
  bool m_initialized;
  Mutex m_lock;
  GetThreadDataFunc m_fGetThreadData;
  std::unique_ptr<LogWriter> m_defaultWriter;
  std::vector<std::shared_ptr<LogWriter>> m_files;
};


struct LogWriter {
  explicit LogWriter(LogChannel chan)
    : m_channel(chan)
  {}
  virtual ~LogWriter() {}
  virtual void init(const std::string& username,
                    AccessLog::GetThreadDataFunc fn) = 0;
  virtual void write(Transport* transport, const VirtualHost* vhost) = 0;
  virtual void flush() {}
protected:
  const LogChannel m_channel;
  FILE* m_filelog{nullptr};
  std::unique_ptr<Cronolog> m_cronolog;
  AccessLog::GetThreadDataFunc m_threadDataFn{nullptr};
  LogFileFlusher m_flusher;
protected:
  FILE* getOutputFile() const;
  void recordWriteAndMaybeDropCaches(FILE* out, int bytes);
};

///////////////////////////////////////////////////////////////////////////////
}

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
#include "hphp/runtime/server/access-log.h"

#include <string>
#include <cctype>
#include <cstdio>
#include <cstdlib>

#include "hphp/runtime/base/timestamp.h"
#include "hphp/runtime/server/log-writer.h"

namespace HPHP {
///////////////////////////////////////////////////////////////////////////////
Mutex AccessLogFileData::m_lock;
std::unordered_map<std::string, AccessLogFileData::factory_t>
AccessLogFileData::m_factories;

AccessLogFileData::AccessLogFileData(const std::string& baseDir,
                                     const std::string& fil,
                                     const std::string& lnk,
                                     const std::string& fmt,
                                     int mpl)
  : AccessLogFileData(PATH_CONCAT(baseDir, fil), PATH_CONCAT(baseDir, lnk), fmt, mpl) {}

AccessLogFileData::AccessLogFileData(const std::string& fil,
                                     const std::string& lnk,
                                     const std::string& fmt,
                                     int mpl)
  : file(fil)
  , symLink(lnk)
  , format(fmt)
  , periodMultiplier(mpl)
{
  /*
   * a LogWriter with it's format can be selected between colons like:
   * Format = :thrift: [["%{%s}t", "out-name", "STRING"], ...]
   */
  m_logOutputType = ClassicWriter::handle;
  auto fmt_ = folly::StringPiece(fmt);
  while (!fmt_.empty() && std::isspace(fmt_.front())) fmt_.pop_front();
  if (fmt_.removePrefix(':')) {
    size_t close = fmt_.find(':');
    if (close != fmt_.npos) {
      m_logOutputType = fmt_.subpiece(0, close).str();
      fmt_.advance(close + 1);
      format = folly::trimWhitespace(fmt_).str();
    }
  }
}

std::unique_ptr<LogWriter> AccessLogFileData::Writer(LogChannel chan) const {
  Lock l(m_lock);
  auto ifactory = m_factories.find(m_logOutputType);
  if (ifactory != m_factories.end()) {
    return ifactory->second(*this, chan);
  }
  throw std::runtime_error(
      ("LogWriter not registered: " + m_logOutputType).c_str());
}

void AccessLogFileData::registerWriter(const std::string& handle,
                                       factory_t factory) {
  Lock l(m_lock);
  m_factories[handle] = factory;
}

AccessLog::~AccessLog() {
  fini();
}

void AccessLog::init(const std::string &defaultFormat,
                     std::vector<AccessLogFileData> &files,
                     const std::string &username) {
  Lock l(m_lock);
  if (m_initialized) return;
  m_initialized = true;
  m_defaultWriter =
    AccessLogFileData("", "", defaultFormat, 0).Writer(LogChannel::THREADLOCAL);
  m_defaultWriter->init(username, m_fGetThreadData);
  for (auto const& file : files) {
    auto ch = Logger::UseCronolog ? LogChannel::CRONOLOG : LogChannel::REGULAR;
    auto writer = std::shared_ptr<LogWriter>(file.Writer(ch));
    writer->init(username, m_fGetThreadData);
    m_files.push_back(writer);
  }
}

void AccessLog::init(const std::string &defaultFormat,
                     std::map<std::string, AccessLogFileData> &files,
                     const std::string &username) {
  Lock l(m_lock);
  if (m_initialized) return;
  m_initialized = true;
  m_defaultWriter =
    AccessLogFileData("", "", defaultFormat, 0).Writer(LogChannel::THREADLOCAL);
  m_defaultWriter->init(username, m_fGetThreadData);
  for (auto const& file : files) {
    auto ch = Logger::UseCronolog ? LogChannel::CRONOLOG : LogChannel::REGULAR;
    auto writer = std::shared_ptr<LogWriter>(file.second.Writer(ch));
    writer->init(username, m_fGetThreadData);
    m_files.push_back(writer);
  }
}

void AccessLog::init(const std::string &format,
                     const std::string &baseDir,
                     const std::string &symLink,
                     const std::string &file,
                     const std::string &username) {
  init(format, PATH_CONCAT(baseDir, symLink), PATH_CONCAT(baseDir, file), username);
}

void AccessLog::init(const std::string &format,
                     const std::string &symLink,
                     const std::string &file,
                     const std::string &username) {
  Lock l(m_lock);
  if (m_initialized) return;
  m_initialized = true;
  m_defaultWriter =
    AccessLogFileData("", "", format, 0).Writer(LogChannel::THREADLOCAL);
  m_defaultWriter->init(username, m_fGetThreadData);
  if (!file.empty() && !format.empty()) {
    auto ch = Logger::UseCronolog ? LogChannel::CRONOLOG : LogChannel::REGULAR;
    auto writer = std::shared_ptr<LogWriter>(
      AccessLogFileData(file, symLink, format, 0).Writer(ch));
    writer->init(username, m_fGetThreadData);
    m_files.push_back(writer);
  }
}

void AccessLog::fini() {
  if (!m_initialized) return;
  Lock l(m_lock);
  flushAllWriters();
  m_files.clear();
  m_defaultWriter.reset();
  m_initialized = false;
}

void AccessLog::log(Transport *transport, const VirtualHost *vhost) {
  assertx(transport);
  if (!m_initialized) return;
  m_defaultWriter->write(transport, vhost);
  for (auto& file : m_files) file->write(transport, vhost);
}

void AccessLog::onNewRequest() {
  if (!m_initialized) return;
  ThreadData *threadData = m_fGetThreadData();
  threadData->startTime = TimeStamp::Current();
}

void AccessLog::flushAllWriters() {
  if (!m_initialized) return;
  m_defaultWriter->flush();
  for (auto& file : m_files) file->flush();
}

bool AccessLog::setThreadLog(const char *file) {
  return (m_fGetThreadData()->log = fopen(file, "a")) != nullptr;
}
void AccessLog::clearThreadLog() {
  FILE* &threadLog = m_fGetThreadData()->log;
  if (threadLog) {
    fclose(threadLog);
  }
  threadLog = nullptr;
}

FILE* LogWriter::getOutputFile() const {
  FILE* outfile = nullptr;
  switch (m_channel) {
    case LogChannel::THREADLOCAL:
      {
        auto tData = (m_threadDataFn ? m_threadDataFn() : nullptr);
        outfile = (tData ? tData->log : nullptr);
      }
      break;
    case LogChannel::CRONOLOG:
      outfile = (m_cronolog.get() ? m_cronolog->getOutputFile() : nullptr);
      break;
    case LogChannel::REGULAR:
      outfile = m_filelog;
      break;
  }
  return outfile;
}

void LogWriter::recordWriteAndMaybeDropCaches(FILE* out, int bytes) {
  switch (m_channel) {
    case LogChannel::THREADLOCAL:
      {
        auto tData = (m_threadDataFn ? m_threadDataFn() : nullptr);
        if (tData) tData->flusher.recordWriteAndMaybeDropCaches(out, bytes);
      }
      break;
    case LogChannel::CRONOLOG:
    case LogChannel::REGULAR:
      m_flusher.recordWriteAndMaybeDropCaches(out, bytes);
      break;
  }
}

///////////////////////////////////////////////////////////////////////////////
}

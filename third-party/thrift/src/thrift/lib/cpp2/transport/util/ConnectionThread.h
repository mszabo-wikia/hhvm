/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

#include <stdint.h>

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include <gflags/gflags.h>
#include <folly/Synchronized.h>
#include <folly/io/async/ScopedEventBaseThread.h>
#include <thrift/lib/cpp2/transport/core/ClientConnectionIf.h>

DECLARE_string(transport);

namespace apache::thrift {

/**
 * A connection thread that handles connections to servers - one
 * connection per server.
 */
class ConnectionThread : public folly::ScopedEventBaseThread {
 public:
  ConnectionThread() = default;
  ~ConnectionThread() override;

  // Returns a connection that may be used to talk to a server at
  // "addr:port".
  std::shared_ptr<ClientConnectionIf> getConnection(
      const std::string& addr, uint16_t port);

 private:
  // Creates a new connection on the provided event base if necessary.
  void maybeCreateConnection(
      const std::string& serverKey, const std::string& addr, uint16_t port);

  folly::Synchronized<
      std::unordered_map<std::string, std::shared_ptr<ClientConnectionIf>>>
      connections_;
};

} // namespace apache::thrift

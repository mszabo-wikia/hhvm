/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 * All rights reserved.
 *
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include <proxygen/lib/transport/PersistentQuicTokenCache.h>

namespace proxygen {

PersistentQuicTokenCache::PersistentQuicTokenCache(
    const std::string& filename, wangle::PersistentCacheConfig config)
    : cache_(filename, std::move(config)) {
}

quic::Optional<std::string> PersistentQuicTokenCache::getToken(
    const std::string& hostname) {
  auto result = cache_.get(hostname);
  return result.has_value() ? quic::Optional<std::string>(result.value())
                            : std::nullopt;
}

void PersistentQuicTokenCache::putToken(const std::string& hostname,
                                        std::string token) {
  cache_.put(hostname, token);
}

void PersistentQuicTokenCache::removeToken(const std::string& hostname) {
  cache_.remove(hostname);
}

} // namespace proxygen

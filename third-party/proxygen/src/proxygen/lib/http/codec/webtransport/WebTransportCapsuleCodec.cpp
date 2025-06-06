/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 * All rights reserved.
 *
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include <proxygen/lib/http/codec/webtransport/WebTransportCapsuleCodec.h>

namespace proxygen {

folly::Expected<folly::Unit, CapsuleCodec::ErrorCode>
WebTransportCapsuleCodec::parseCapsule(folly::io::Cursor& cursor) {
  switch (curCapsuleType_) {
    case folly::to_underlying(CapsuleType::PADDING): {
      auto res = parsePadding(cursor, curCapsuleLength_);
      if (res) {
        if (callback_) {
          callback_->onPaddingCapsule(std::move(res.value()));
        }
      } else {
        return folly::makeUnexpected(res.error());
      }
      break;
    }
    case folly::to_underlying(CapsuleType::WT_RESET_STREAM): {
      auto res = parseWTResetStream(cursor, curCapsuleLength_);
      if (res) {
        if (callback_) {
          callback_->onWTResetStreamCapsule(std::move(res.value()));
        }
      } else {
        return folly::makeUnexpected(res.error());
      }
      break;
    }
    case folly::to_underlying(CapsuleType::WT_STOP_SENDING): {
      auto res = parseWTStopSending(cursor, curCapsuleLength_);
      if (res) {
        if (callback_) {
          callback_->onWTStopSendingCapsule(std::move(res.value()));
        }
      } else {
        return folly::makeUnexpected(res.error());
      }
      break;
    }
    case folly::to_underlying(CapsuleType::WT_STREAM):
    case folly::to_underlying(CapsuleType::WT_STREAM_WITH_FIN): {
      bool fin = curCapsuleType_ ==
                         (folly::to_underlying(CapsuleType::WT_STREAM_WITH_FIN))
                     ? true
                     : false;
      auto res = parseWTStream(cursor, curCapsuleLength_, fin);
      if (res) {
        if (callback_) {
          callback_->onWTStreamCapsule(std::move(res.value()));
        }
      } else {
        return folly::makeUnexpected(res.error());
      }
      break;
    }
    case folly::to_underlying(CapsuleType::WT_MAX_DATA): {
      auto res = parseWTMaxData(cursor, curCapsuleLength_);
      if (res) {
        if (callback_) {
          callback_->onWTMaxDataCapsule(std::move(res.value()));
        }
      } else {
        return folly::makeUnexpected(res.error());
      }
      break;
    }
    case folly::to_underlying(CapsuleType::WT_MAX_STREAM_DATA): {
      auto res = parseWTMaxStreamData(cursor, curCapsuleLength_);
      if (res) {
        if (callback_) {
          callback_->onWTMaxStreamDataCapsule(std::move(res.value()));
        }
      } else {
        return folly::makeUnexpected(res.error());
      }
      break;
    }
    case folly::to_underlying(CapsuleType::WT_MAX_STREAMS): {
      auto res = parseWTMaxStreams(cursor, curCapsuleLength_);
      if (res) {
        if (callback_) {
          callback_->onWTMaxStreamsCapsule(std::move(res.value()));
        }
      } else {
        return folly::makeUnexpected(res.error());
      }
      break;
    }
    case folly::to_underlying(CapsuleType::WT_DATA_BLOCKED): {
      auto res = parseWTDataBlocked(cursor, curCapsuleLength_);
      if (res) {
        if (callback_) {
          callback_->onWTDataBlockedCapsule(std::move(res.value()));
        }
      } else {
        return folly::makeUnexpected(res.error());
      }
      break;
    }
    case folly::to_underlying(CapsuleType::WT_STREAM_DATA_BLOCKED): {
      auto res = parseWTStreamDataBlocked(cursor, curCapsuleLength_);
      if (res) {
        if (callback_) {
          callback_->onWTStreamDataBlockedCapsule(std::move(res.value()));
        }
      } else {
        return folly::makeUnexpected(res.error());
      }
      break;
    }
    case folly::to_underlying(CapsuleType::WT_STREAMS_BLOCKED): {
      auto res = parseWTStreamsBlocked(cursor, curCapsuleLength_);
      if (res) {
        if (callback_) {
          callback_->onWTStreamsBlockedCapsule(std::move(res.value()));
        }
      } else {
        return folly::makeUnexpected(res.error());
      }
      break;
    }
    case folly::to_underlying(CapsuleType::DATAGRAM): {
      auto res = parseDatagram(cursor, curCapsuleLength_);
      if (res) {
        if (callback_) {
          callback_->onDatagramCapsule(std::move(res.value()));
        }
      } else {
        return folly::makeUnexpected(res.error());
      }
      break;
    }
    case folly::to_underlying(CapsuleType::CLOSE_WEBTRANSPORT_SESSION): {
      auto res = parseCloseWebTransportSession(cursor, curCapsuleLength_);
      if (res) {
        if (callback_) {
          callback_->onCloseWebTransportSessionCapsule(std::move(res.value()));
        }
      } else {
        return folly::makeUnexpected(res.error());
      }
      break;
    }
    case folly::to_underlying(CapsuleType::DRAIN_WEBTRANSPORT_SESSION): {
      auto res = parseDrainWebTransportSession(curCapsuleLength_);
      if (res) {
        if (callback_) {
          callback_->onDrainWebTransportSessionCapsule(std::move(res.value()));
        }
      } else {
        return folly::makeUnexpected(res.error());
      }
      break;
    }
    default:
      return folly::makeUnexpected(CapsuleCodec::ErrorCode::PARSE_ERROR);
  }
  return folly::unit;
}
} // namespace proxygen

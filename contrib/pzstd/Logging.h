/**
 * Copyright (c) 2016-present, Facebook, Inc.
 * All rights reserved.
 *
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree. An additional grant
 * of patent rights can be found in the PATENTS file in the same directory.
 */
#pragma once

#include <cstdio>
#include <mutex>

namespace pzstd {

constexpr int ERROR = 1;
constexpr int INFO = 2;
constexpr int DEBUG = 3;
constexpr int VERBOSE = 4;

class Logger {
  std::mutex mutex_;
  FILE* out_;
  const int level_;

  using Clock = std::chrono::system_clock;
  Clock::time_point lastUpdate_;
  std::chrono::milliseconds refreshRate_;

 public:
  explicit Logger(int level, FILE* out = stderr)
      : out_(out), level_(level), lastUpdate_(Clock::now()),
        refreshRate_(150) {}


  bool logsAt(int level) {
    return level <= level_;
  }

  template <typename String, typename... Args>
  void operator()(int level, String fmt, Args... args) {
    if (level > level_) {
      return;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    std::fprintf(out_, fmt, args...);
  }

  template <typename... Args>
  void update(int level, const char *fmt, Args... args) {
    if (level > level_) {
      return;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    auto now = Clock::now();
    if (now - lastUpdate_ > refreshRate_) {
      lastUpdate_ = now;
      std::fprintf(out_, "\r");
      std::fprintf(out_, fmt, args...);
    }
  }

  void clear(int level) {
    if (level > level_) {
      return;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    std::fprintf(out_, "\r%79s\r", "");
  }
};

}

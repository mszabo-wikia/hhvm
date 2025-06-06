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

#include <thrift/common/detail/string.h>

// This file contains string-related helper functions commonly used throughout
// Whisker's different layers: lexing, parsing, and evaluator.
namespace whisker::detail {

bool is_newline(char c);
bool is_whitespace(char c);
bool is_letter(char c);
bool is_digit(char c);

using apache::thrift::detail::escape;

} // namespace whisker::detail

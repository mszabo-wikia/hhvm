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

namespace py3 thrift.python.service_interceptor

service BasicService {
  string toLowerSnake(1: string input);
}

service OnewayService {
  // @lint-ignore THRIFTCHECKS
  oneway void toLowerSnake(1: string input);
}

service StreamingService {
  stream<string> toLowerSnake(1: string input);
}

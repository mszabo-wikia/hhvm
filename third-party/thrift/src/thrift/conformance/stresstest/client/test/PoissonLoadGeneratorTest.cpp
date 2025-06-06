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

#include <gtest/gtest.h>
#include <folly/executors/GlobalExecutor.h>
#include <thrift/conformance/stresstest/client/PoissonLoadGenerator.h>

using namespace apache::thrift;
using namespace apache::thrift::stress;

TEST(PoissonLoadGeneratorTest, Basic) {
  PoissonLoadGenerator generator(1000, std::chrono::milliseconds(5));
  generator.start();

  auto signals = generator.getRequestCount();
  unsigned count = 0;
  while (count < 5) {
    signals.next().viaIfAsync(folly::getGlobalCPUExecutor()).await_ready();
    count++;
  }

  EXPECT_EQ(count, 5);
}

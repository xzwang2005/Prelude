// Copyright (c) 2018 Xuezhong Wang. All rights reserved.
// Use of this source code is governed by a MIT license that can be
// found in the LICENSE file.

#include "testing/gtest/include/gtest/gtest.h"

// switches such as "single-process-tests" can be found:
// //base/test/launcher/unit_test_launcher.cc

namespace {

int plus(int x, int y) { return x + y; }

} // namespace

namespace prelude {
namespace unit_test_samples {

TEST(DummyTest, OnePlusOneEqualTwo) {
  printf("this is a dummy test.\n");
  EXPECT_EQ(2, plus(1, 1));
}

} // namespace unit_test_samples
} // namespace prelude

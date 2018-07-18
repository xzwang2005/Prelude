// Copyright (c) 2018 The Prelude Authors.
// Use of this source code is governed by MIT license that can be
// found in MIT-LICENSE file.

#include "base/bind.h"
#include "base/run_loop.h"
#include "services/service_manager/public/cpp/service_test.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "prelude/excerpt04_service/interfaces/logger.mojom.h"
#include "prelude/excerpt04_service/interfaces/constants.mojom.h"

class MultiClassServiceTest : public service_manager::test::ServiceTest {
public:
  // Our tests run as service instances themselves. In this case each instance
  // identifies as the service named "single_class_service_unittests".
  MultiClassServiceTest() : service_manager::test::ServiceTest("multi_class_service_unittests") {
  }

  ~MultiClassServiceTest() override {}
};

// the return type/value from the server is always the last parameter here,
// so that we can insert a callback as bound parameter
void OnGetTail(const base::Closure& callback, const std::string& message) {
  //LOG(ERROR) << "Tail was: " << message;
  // make this test fail to verify that the service call succeeded
  EXPECT_EQ(message, "test logger not equal (multi-class)");
  callback.Run();
}

TEST_F(MultiClassServiceTest, Basic) {
  connector()->StartService("multi_class_service");  // nice to have, but not necessary
  prelude::mojom::LoggerPtr logger;
  connector()->BindInterface(prelude::mojom::kMultiClassServiceName, &logger);
  base::RunLoop loop;

  logger->Log("test logger");

  // get message back
  logger->GetTail(base::BindOnce(&OnGetTail, loop.QuitClosure()));
  loop.Run();
}
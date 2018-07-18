// Copyright (c) 2018 The Prelude Authors.
// Use of this source code is governed by MIT license that can be
// found in MIT-LICENSE file.

#include "single_class_service_impl.h"
#include "services/service_manager/public/c/main.h"
#include "services/service_manager/public/cpp/service_runner.h"

MojoResult ServiceMain(MojoHandle service_request_handle) {
  return service_manager::ServiceRunner(new prelude::excerpt04::SingleClassServiceImpl).Run(
    service_request_handle);
}
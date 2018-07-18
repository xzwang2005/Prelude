// Copyright (c) 2018 The Prelude Authors.
// Use of this source code is governed by MIT license that can be
// found in MIT-LICENSE file.

#include "service_impl.h"
#include "base/logging.h"
#include "logger_impl.h"

namespace prelude {
namespace excerpt04 {

ServiceImpl::ServiceImpl() {
  // create a interface impl instance
  logger_ = std::make_unique<LoggerImpl>();
  registry_.AddInterface(
      base::Bind(&LoggerImpl::Create, base::Unretained(logger_.get())));
}

ServiceImpl::~ServiceImpl() = default;

void ServiceImpl::OnBindInterface(
    const service_manager::BindSourceInfo &remote_info,
    const std::string &interface_name, mojo::ScopedMessagePipeHandle handle) {
  registry_.BindInterface(interface_name, std::move(handle));
}
} // namespace excerpt04
} // namespace prelude

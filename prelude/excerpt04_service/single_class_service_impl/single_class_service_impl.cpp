// Copyright (c) 2018 The Prelude Authors.
// Use of this source code is governed by MIT license that can be
// found in MIT-LICENSE file.

#include "single_class_service_impl.h"
#include "base/logging.h"
#include "base/run_loop.h"

namespace prelude {
namespace excerpt04 {

SingleClassServiceImpl::SingleClassServiceImpl() = default;

SingleClassServiceImpl::~SingleClassServiceImpl() = default;

void SingleClassServiceImpl::OnStart() {
  bindings_.set_connection_error_handler(base::Bind(
      &SingleClassServiceImpl::OnConnectionError, base::Unretained(this)));
  registry_.AddInterface<prelude::mojom::Logger>(
      base::Bind(&SingleClassServiceImpl::BindLoggerServiceRequest,
                 base::Unretained(this)));
}

void SingleClassServiceImpl::OnBindInterface(
    const service_manager::BindSourceInfo &remote_info,
    const std::string &interface_name, mojo::ScopedMessagePipeHandle handle) {
  registry_.BindInterface(interface_name, std::move(handle), remote_info);
}

void SingleClassServiceImpl::Log(const std::string &message) {
  LOG(ERROR) << "[Logger] " << message;
  lines_.push_back(message);
  log_count++;
}

void SingleClassServiceImpl::GetTail(GetTailCallback callback) {
  std::move(callback).Run(lines_.back());
}


void SingleClassServiceImpl::GetLogCount(GetLogCountCallback callback)
{
  std::move(callback).Run(log_count);
}

void SingleClassServiceImpl::OnConnectionError() {
  if (bindings_.empty()) {
    base::RunLoop::QuitCurrentWhenIdleDeprecated();
  }
}

void SingleClassServiceImpl::BindLoggerServiceRequest(
    prelude::mojom::LoggerRequest request,
    const service_manager::BindSourceInfo &source_info) {
  bindings_.AddBinding(this, std::move(request));
}

} // namespace excerpt04
} // namespace prelude

// Copyright (c) 2018 The Prelude Authors.
// Use of this source code is governed by MIT license that can be
// found in MIT-LICENSE file.

#include "logger_impl.h"
#include "base/logging.h"
#include "base/run_loop.h"

namespace prelude {
namespace excerpt04 {

LoggerImpl::LoggerImpl() = default;
LoggerImpl::~LoggerImpl() = default;

void LoggerImpl::Log(const std::string &message) {
  LOG(ERROR) << "[Logger] " << message;
  lines_.push_back(message);
  log_count++;
}

void LoggerImpl::GetTail(GetTailCallback callback) {
  std::move(callback).Run(lines_.back());
}


void LoggerImpl::GetLogCount(GetLogCountCallback callback)
{
  std::move(callback).Run(log_count);
}

void LoggerImpl::OnConnectionError() {
  if (bindings_.empty()) {
    base::RunLoop::QuitCurrentWhenIdleDeprecated();
  }
}

void LoggerImpl::Create(prelude::mojom::LoggerRequest request) {
  bindings_.set_connection_error_handler(
      base::Bind(&LoggerImpl::OnConnectionError, base::Unretained(this)));
  bindings_.AddBinding(this, std::move(request));
}

} // namespace excerpt04
} // namespace prelude

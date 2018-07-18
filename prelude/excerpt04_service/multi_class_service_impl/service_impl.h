// Copyright (c) 2018 The Prelude Authors.
// Use of this source code is governed by MIT license that can be
// found in MIT-LICENSE file.

#include "base/macros.h"
#include "services/service_manager/public/cpp/binder_registry.h"
#include "services/service_manager/public/cpp/service.h"

namespace prelude {
namespace excerpt04 {
class LoggerImpl;

class ServiceImpl : public service_manager::Service {
public:
  ServiceImpl();
  ~ServiceImpl() override;

private:
  // service_manager::Service:
  void OnBindInterface(const service_manager::BindSourceInfo &remote_info,
                       const std::string &interface_name,
                       mojo::ScopedMessagePipeHandle handle) override;

  service_manager::BinderRegistry registry_;
  std::unique_ptr<LoggerImpl> logger_;

  DISALLOW_COPY_AND_ASSIGN(ServiceImpl);
};
} // namespace excerpt04
} // namespace prelude

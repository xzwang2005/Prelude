// Copyright (c) 2018 The Prelude Authors.
// Use of this source code is governed by MIT license that can be
// found in MIT-LICENSE file.

#ifndef SINGLE_CLASS_SERVICE_IMPL_H_
#define SINGLE_CLASS_SERVICE_IMPL_H_

#include "base/macros.h"
#include "mojo/public/cpp/bindings/binding_set.h"
#include "prelude/excerpt04_service/interfaces/logger.mojom.h"
#include "services/service_manager/public/cpp/binder_registry.h"
#include "services/service_manager/public/cpp/connector.h"
#include "services/service_manager/public/cpp/service.h"

#include <vector>

namespace prelude {
namespace excerpt04 {

class SingleClassServiceImpl : public service_manager::Service,
                               public prelude::mojom::Logger {
public:
  SingleClassServiceImpl();
  ~SingleClassServiceImpl() override;

private:
  // service_manager::Service:
  void OnStart() override;
  void OnBindInterface(const service_manager::BindSourceInfo &remote_info,
                       const std::string &interface_name,
                       mojo::ScopedMessagePipeHandle handle) override;
  // Logger
  void Log(const std::string &message) override;
  void GetTail(GetTailCallback callback) override;
  void GetLogCount(GetLogCountCallback callback) override;

  void OnConnectionError();

  void
  BindLoggerServiceRequest(prelude::mojom::LoggerRequest request,
                           const service_manager::BindSourceInfo &source_info);

  // the registry here is of a different type than the one used in multi_class
  // case. It has different signatures for AddInterface and BindInterface
  service_manager::BinderRegistryWithArgs<
      const service_manager::BindSourceInfo &>
      registry_;
  mojo::BindingSet<prelude::mojom::Logger> bindings_;
  std::vector<std::string> lines_;
  int log_count{ 0 };

  DISALLOW_COPY_AND_ASSIGN(SingleClassServiceImpl);
};

} // namespace excerpt04
} // namespace prelude
#endif // SINGLE_CLASS_SERVICE_IMPL_H_

// Copyright (c) 2019 The Prelude Authors.
// Use of this source code is governed by MIT license that can be
// found in MIT-LICENSE file.

#ifndef _SERVICE_101_VANILLA_COOKIE_SERVICE_H__
#define _SERVICE_101_VANILLA_COOKIE_SERVICE_H__

#include "base/macros.h"
#include "services/service_manager/public/cpp/binder_registry.h"
#include "services/service_manager/public/cpp/service.h"

#include "prelude/excerpt02_mojo/public/mojom/fortune_cookie.mojom.h"

namespace prelude {
class VanillaCookieService : public service_manager::Service {
 public:
  VanillaCookieService();
  ~VanillaCookieService() override;

 private:
  // service_manager::Service
  void OnStart() override;
  void OnBindInterface(const service_manager::BindSourceInfo& source_info,
                       const std::string& interface_name,
                       mojo::ScopedMessagePipeHandle interface_pipe) override;

  void BindCookieService(mojom::FortuneCookieRequest request);

  service_manager::BinderRegistry registry_;

  DISALLOW_COPY_AND_ASSIGN(VanillaCookieService);
};
}  // namespace prelude
#endif  // _SERVICE_101_VANILLA_COOKIE_SERVICE_H__

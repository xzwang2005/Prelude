// Copyright (c) 2019 The Prelude Authors.
// Use of this source code is governed by MIT license that can be
// found in MIT-LICENSE file.

#include "prelude/excerpt02_mojo/service_101/vanilla_cookie_service.h"
#include "prelude/excerpt02_mojo/service_101/vanilla_fortune_cookie.h"

namespace prelude {

VanillaCookieService::VanillaCookieService() = default;

VanillaCookieService::~VanillaCookieService() = default;

void VanillaCookieService::OnStart() {
  registry_.AddInterface<mojom::FortuneCookie>(base::Bind(
      &VanillaCookieService::BindCookieService, base::Unretained(this)));
}

void VanillaCookieService::OnBindInterface(
    const service_manager::BindSourceInfo& source_info,
    const std::string& interface_name,
    mojo::ScopedMessagePipeHandle interface_pipe) {
  registry_.BindInterface(interface_name, std::move(interface_pipe));
}

void VanillaCookieService::BindCookieService(
    mojom::FortuneCookieRequest request) {
  VanillaFortuneCookie::Create(std::move(request));
}

}  // namespace prelude

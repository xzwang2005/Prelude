// Copyright (c) 2019 The Prelude Authors.
// Use of this source code is governed by MIT license that can be
// found in MIT-LICENSE file.

#include "prelude/excerpt02_mojo/service_101/vanilla_fortune_cookie.h"
#include "mojo/public/cpp/bindings/strong_binding.h"

namespace prelude {

VanillaFortuneCookie::VanillaFortuneCookie() = default;

VanillaFortuneCookie::~VanillaFortuneCookie() = default;

void VanillaFortuneCookie::Create(mojom::FortuneCookieRequest request) {
  mojo::MakeStrongBinding(std::make_unique<VanillaFortuneCookie>(),
                          std::move(request));
}

void VanillaFortuneCookie::Crack(CrackCallback callback) {
  std::move(callback).Run("Your road to glory will be rocky, but fulfilling.");
}

}  // namespace prelude

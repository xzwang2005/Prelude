// Copyright (c) 2019 The Prelude Authors.
// Use of this source code is governed by MIT license that can be
// found in MIT-LICENSE file.
#include "prelude/excerpt02_mojo/mojo_101/fortune_cookie_impl.h"
#include <iostream>

namespace prelude {

FortuneCookieImplAlpha::FortuneCookieImplAlpha() : binding_(this) {}

FortuneCookieImplAlpha::~FortuneCookieImplAlpha() = default;

void FortuneCookieImplAlpha::BindRequest(mojom::FortuneCookieRequest request) {
  std::cout << "Fortune cookie binding.\n";
  binding_.Bind(std::move(request));
}

void FortuneCookieImplAlpha::EatMe() {
  std::cout << "Bon appetit.\n";
}

void FortuneCookieImplAlpha::Crack(CrackCallback callback) {
  std::cout << "crack now.\n";
  std::move(callback).Run("A dream you have will come true.");
}

}  // namespace prelude

// Copyright (c) 2019 The Prelude Authors.
// Use of this source code is governed by MIT license that can be
// found in MIT-LICENSE file.

#ifndef _PRELUDE_MOJO_101_FORTUNE_COOKIE_IMPL_H__
#define _PRELUDE_MOJO_101_FORTUNE_COOKIE_IMPL_H__

#include "base/macros.h"
#include "mojo/public/cpp/bindings/binding.h"
#include "prelude/excerpt02_mojo/public/mojom/fortune_cookie.mojom.h"

namespace prelude {
class FortuneCookieImplAlpha : public mojom::FortuneCookie {
 public:
  FortuneCookieImplAlpha();
  ~FortuneCookieImplAlpha() override;

  void BindRequest(mojom::FortuneCookieRequest request);

  void EatMe();

 private:
  // mojom::FortuneCookie impl
  void Crack(CrackCallback callback) override;

  mojo::Binding<mojom::FortuneCookie> binding_;

  DISALLOW_COPY_AND_ASSIGN(FortuneCookieImplAlpha);
};
}  // namespace prelude
#endif // _PRELUDE_MOJO_101_FORTUNE_COOKIE_IMPL_H__

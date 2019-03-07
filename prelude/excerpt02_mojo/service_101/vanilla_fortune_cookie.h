// Copyright (c) 2019 The Prelude Authors.
// Use of this source code is governed by MIT license that can be
// found in MIT-LICENSE file.

#ifndef _SERVICE_101_VANILLA_FORTUNE_COOKIE_H__
#define _SERVICE_101_VANILLA_FORTUNE_COOKIE_H__

#include "base/macros.h"
#include "prelude/excerpt02_mojo/public/mojom/fortune_cookie.mojom.h"

namespace prelude {
class VanillaFortuneCookie : public mojom::FortuneCookie {
 public:
  VanillaFortuneCookie();
  ~VanillaFortuneCookie() override;

  static void Create(mojom::FortuneCookieRequest request);

 private:
  void Crack(CrackCallback callback) override;

  DISALLOW_COPY_AND_ASSIGN(VanillaFortuneCookie);
};
}  // namespace prelude
#endif // _SERVICE_101_VANILLA_FORTUNE_COOKIE_H__

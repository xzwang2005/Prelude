// Copyright (c) 2019 The Prelude Authors.
// Use of this source code is governed by MIT license that can be
// found in MIT-LICENSE file.

#ifndef _PRELUDE_MOJO_101_RADIATION_LISTENER_IMPL_H_
#define _PRELUDE_MOJO_101_RADIATION_LISTENER_IMPL_H_

#include "base/macros.h"
#include "mojo/public/cpp/bindings/binding.h"
#include "prelude/excerpt02_mojo/public/mojom/radiation_monitor.mojom.h"

namespace prelude {
class RadiationListenerImpl : public mojom::RadiationListener {
 public:
  RadiationListenerImpl();
  ~RadiationListenerImpl() override;

  void BindRequest(mojom::RadiationListenerRequest request);

 private:
  // mojom::RadiationListener
  void OnRadiationLeak(const std::string& msg) override;

  mojo::Binding<mojom::RadiationListener> binding_;
  DISALLOW_COPY_AND_ASSIGN(RadiationListenerImpl);
};
}  // namespace prelude
#endif // _PRELUDE_MOJO_101_RADIATION_LISTENER_IMPL_H_

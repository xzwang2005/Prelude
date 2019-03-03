// Copyright (c) 2019 The Prelude Authors.
// Use of this source code is governed by MIT license that can be
// found in MIT-LICENSE file.

#include "prelude/excerpt02_mojo/mojo_101/radiation_listener_impl.h"
#include <iostream>

namespace prelude {

RadiationListenerImpl::RadiationListenerImpl() : binding_(this) {}

RadiationListenerImpl::~RadiationListenerImpl() = default;

void RadiationListenerImpl::BindRequest(
    mojom::RadiationListenerRequest request) {
  std::cout << "listener bound\n";
  binding_.Bind(std::move(request));
}

void RadiationListenerImpl::OnRadiationLeak(const std::string& msg) {
  std::cout << "listener received message:" << msg << std::endl;
  if (msg == "drill") {
    std::cout << "Don't panic.\n";
  } else if (msg == "meltdown") {
    std::cout << "Run for your life!\n";
  } else {
    std::cout << "err... not expecting this.\n";
  }
}

}  // namespace prelude

// Copyright (c) 2019 The Prelude Authors.
// Use of this source code is governed by MIT license that can be
// found in MIT-LICENSE file.
#include "prelude/excerpt02_mojo/mojo_101/radiation_monitor_impl.h"
#include <iostream>

namespace prelude {

RadiationMonitorImpl::RadiationMonitorImpl() : binding_(this) {}

RadiationMonitorImpl::~RadiationMonitorImpl() = default;

void RadiationMonitorImpl::BindRequest(mojom::RadiationMonitorRequest request) {
  std::cout << "monitor bound.\n";
  binding_.Bind(std::move(request));
}

void RadiationMonitorImpl::Drill() {
  std::cout << "monitor drill.\n";
  for (const auto& listener : listeners_) {
    listener.second->OnRadiationLeak("drill");
  }
}

void RadiationMonitorImpl::MeltDown() {
  std::cout << "monitor detect meltdown\n";
  for (const auto& listener : listeners_) {
    listener.second->OnRadiationLeak("meltdown");
  }
}

void RadiationMonitorImpl::RegisterListener(
    mojom::RadiationListenerPtr listener) {
  std::cout << "register listener.\n";
  listeners_[next_listener_id_++] = std::move(listener);
}

}  // namespace prelude

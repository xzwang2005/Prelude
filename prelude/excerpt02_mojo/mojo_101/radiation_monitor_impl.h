// Copyright (c) 2019 The Prelude Authors.
// Use of this source code is governed by MIT license that can be
// found in MIT-LICENSE file.

#ifndef _PRELUDE_MOJO_101_RADIATION_MONITOR_IMPL_H_
#define _PRELUDE_MOJO_101_RADIATION_MONITOR_IMPL_H_

#include <string>
#include "base/containers/flat_map.h"
#include "base/macros.h"
#include "mojo/public/cpp/bindings/binding.h"
#include "prelude/excerpt02_mojo/public/mojom/radiation_monitor.mojom.h"

namespace prelude {
class RadiationMonitorImpl : public mojom::RadiationMonitor {
 public:
  RadiationMonitorImpl();
  ~RadiationMonitorImpl() override;

  void BindRequest(mojom::RadiationMonitorRequest request);

  void Drill();

  void MeltDown();

 private:
  void RegisterListener(mojom::RadiationListenerPtr listener) override;

  mojo::Binding<mojom::RadiationMonitor> binding_;
  base::flat_map<int, mojom::RadiationListenerPtr> listeners_;
  int next_listener_id_{0};

  DISALLOW_COPY_AND_ASSIGN(RadiationMonitorImpl);
};
}  // namespace prelude
#endif  // _PRELUDE_MOJO_101_RADIATION_MONITOR_IMPL_H_

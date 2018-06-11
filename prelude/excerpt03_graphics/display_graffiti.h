// Copyright (c) 2018 The Prelude Authors.
// Use of this source code is governed by MIT license that can be
// found in MIT-LICENSE file.

#ifndef PRELUDE_DISPLAY_GRAFFITI_H_
#define PRELUDE_DISPLAY_GRAFFITI_H_

#include "base/single_thread_task_runner.h"
#include "cc/test/test_image_factory.h"
#include "cc/test/test_shared_bitmap_manager.h"
#include "components/viz/common/display/renderer_settings.h"
#include "components/viz/common/quads/render_pass.h"
#include "components/viz/common/surfaces/frame_sink_id.h"
#include "components/viz/common/surfaces/local_surface_id.h"
#include "components/viz/common/surfaces/parent_local_surface_id_allocator.h"
#include "components/viz/service/display/display.h"
#include "components/viz/service/display/display_client.h"
#include "components/viz/service/display/display_scheduler.h"
#include "components/viz/service/frame_sinks/compositor_frame_sink_support.h"
#include "components/viz/service/frame_sinks/frame_sink_manager_impl.h"
#include "components/viz/test/test_gpu_memory_buffer_manager.h"
#include "direct_output_surface.h"
#include "ui/base/cursor/cursor.h"
#include "ui/gfx/geometry/rect.h"
#include "ui/gfx/native_widget_types.h"
#include "ui/platform_window/platform_window_delegate.h"

namespace ui {
class PlatformWindow;
}

namespace prelude {

class StubDisplayClient : public viz::DisplayClient {
public:
  StubDisplayClient() = default;
  ~StubDisplayClient() override = default;
  void DisplayOutputSurfaceLost() override {}
  void
  DisplayWillDrawAndSwap(bool will_draw_and_swap,
                         const viz::RenderPassList &render_passes) override {}
  void DisplayDidDrawAndSwap() override {}
  void DisplayDidReceiveCALayerParams(
      const gfx::CALayerParams &ca_layer_params) override{};
};

class DisplayGraffiti : public ui::PlatformWindowDelegate {
public:
  DisplayGraffiti();
  ~DisplayGraffiti() override;

  void Initialize(gfx::Size &winSize);
  void ShutDown();
  void SubmitAndDrawCompositorFrame(viz::RenderPassList *pass_list);
  void ShowWindow();

  // ui::PlatformWindowDelegate
  void OnBoundsChanged(const gfx::Rect &new_bounds) override;
  void OnDamageRect(const gfx::Rect &damaged_region) override;
  void DispatchEvent(ui::Event *event) override;
  void OnCloseRequest() override;
  void OnClosed() override;
  void OnWindowStateChanged(ui::PlatformWindowState new_state) override;
  void OnLostCapture() override;
  void OnAcceleratedWidgetAvailable(gfx::AcceleratedWidget widget,
                                    float device_pixel_ratio) override;
  void OnAcceleratedWidgetDestroyed() override;
  void OnActivationChanged(bool active) override;

private:
  viz::FrameSinkManagerImpl manager_;
  std::unique_ptr<viz::CompositorFrameSinkSupport> support_;
  viz::RendererSettings renderer_settings_;
  viz::TestGpuMemoryBufferManager gpu_memory_buffer_manager_;
  cc::TestImageFactory image_factory_;
  viz::ParentLocalSurfaceIdAllocator id_allocator_;
  scoped_refptr<base::SingleThreadTaskRunner> task_runner_;
  cc::TestSharedBitmapManager shared_bitmap_manager_;
  std::unique_ptr<viz::BeginFrameSource> begin_frame_source_;
  std::unique_ptr<viz::Display> display_;
  viz::LocalSurfaceId local_surface_id_;

  // ref: WindowTreeHostPlatform
  gfx::AcceleratedWidget widget_;
  std::unique_ptr<ui::PlatformWindow> platform_window_;
  gfx::NativeCursor current_cursor_;
  gfx::Rect bounds_;
  StubDisplayClient client;

  scoped_refptr<ui::InProcessContextProvider> context_provider_;

  double refresh_rate_ = 30.0;
};

} // namespace prelude
#endif // PRELUDE_DISPLAY_GRAFFITI_H_

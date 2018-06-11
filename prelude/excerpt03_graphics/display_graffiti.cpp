// Copyright (c) 2018 The Prelude Authors.
// Use of this source code is governed by MIT license that can be
// found in MIT-LICENSE file.

#include "display_graffiti.h"

#include "base/threading/thread_task_runner_handle.h"
#include "base/time/time.h"
#include "build/build_config.h"
#include "command_buffer/common/gles2_cmd_utils.h"
#include "components/viz/common/frame_sinks/delay_based_time_source.h"
#include "components/viz/common/quads/compositor_frame.h"
#include "components/viz/service/display/display_client.h"
#include "components/viz/service/display/display_scheduler.h"
#include "components/viz/test/compositor_frame_helpers.h"
#include "ui/compositor/test/in_process_context_provider.h"
#include "ui/gfx/color_space.h"
#include "ui/platform_window/win/win_window.h"

namespace prelude {

static constexpr viz::FrameSinkId kArbitraryFrameSinkId(3, 3);

DisplayGraffiti::DisplayGraffiti()
    : support_(viz::CompositorFrameSinkSupport::Create(
          nullptr, &manager_, kArbitraryFrameSinkId, true /* is_root */,
          true /* needs_sync_points */)),
      task_runner_(base::ThreadTaskRunnerHandle::Get()) {}

DisplayGraffiti::~DisplayGraffiti() {
}

// ref: InProcessContextFactory::CreateLayerTreeFrameSink
void DisplayGraffiti::Initialize(gfx::Size &winSize) {
  // 0. create a window. this window will call OnAcceleratedWidgetAvailable()
  // when it's created, passing in its HWND
  gfx::Rect bounds(0, 0, winSize.width(), winSize.height());
#if defined(OS_WIN)
  platform_window_.reset(new ui::WinWindow(this, bounds));
  // platform_window_->SetTitle("display demo");
#else
  NOTIMPLEMENTED();
#endif
  DCHECK(widget_);

  // 1. create ContextProvider
  gpu::gles2::ContextCreationAttribHelper attribs;
  attribs.alpha_size = 8;
  attribs.blue_size = 8;
  attribs.green_size = 8;
  attribs.red_size = 8;
  attribs.depth_size = 0;
  attribs.stencil_size = 0;
  attribs.samples = 0;
  attribs.sample_buffers = 0;
  attribs.fail_if_major_perf_caveat = false;
  attribs.bind_generates_resource = false;

  context_provider_ = ui::InProcessContextProvider::Create(
      attribs, nullptr, &gpu_memory_buffer_manager_, &image_factory_, widget_,
      "UICompositor", false /* support_lock */);

  auto result = context_provider_->BindToCurrentThread();
  if (result != gpu::ContextResult::kSuccess) {
    context_provider_ = nullptr;
    return;
  }

  // 2. create OutputSurface
  std::unique_ptr<viz::OutputSurface> display_output_surface =
      std::make_unique<DirectOutputSurface>(context_provider_);

  // 3. create BeginFrameSource
  auto time_source =
      std::make_unique<viz::DelayBasedTimeSource>(task_runner_.get());
  time_source->SetTimebaseAndInterval(
      base::TimeTicks(),
      base::TimeDelta::FromMicroseconds(base::Time::kMicrosecondsPerSecond /
                                        refresh_rate_));
  begin_frame_source_ = std::make_unique<viz::DelayBasedBeginFrameSource>(
      std::move(time_source), viz::BeginFrameSource::kNotRestartableId);

  // 4. create DisplayScheduler
  auto scheduler = std::make_unique<viz::DisplayScheduler>(
      begin_frame_source_.get(), task_runner_.get(),
      display_output_surface->capabilities().max_frames_pending);

  // 5. create Display
#if defined(OS_WIN)
  renderer_settings_.finish_rendering_on_resize = true;
#endif
  display_ = std::make_unique<viz::Display>(
      &shared_bitmap_manager_, &gpu_memory_buffer_manager_, renderer_settings_,
      kArbitraryFrameSinkId, std::move(display_output_surface),
      std::move(scheduler), task_runner_);

  // 6. register
  manager_.RegisterBeginFrameSource(begin_frame_source_.get(),
                                    kArbitraryFrameSinkId);

  // 7. set color space (is this necessary?)
  gfx::ColorSpace color_space_1 = gfx::ColorSpace::CreateXYZD50();
  gfx::ColorSpace color_space_2 = gfx::ColorSpace::CreateSCRGBLinear();

  display_->Initialize(&client, manager_.surface_manager());
  display_->SetColorSpace(color_space_1, color_space_1);

  // 8. set surface id, set display size
  local_surface_id_ = id_allocator_.GenerateId();
  display_->SetLocalSurfaceId(local_surface_id_, 1.f);
  display_->Resize(winSize);
  display_->SetVisible(true);
}

void DisplayGraffiti::ShutDown() {
  if (begin_frame_source_)
    manager_.UnregisterBeginFrameSource(begin_frame_source_.get());
}

void DisplayGraffiti::SubmitAndDrawCompositorFrame(
    viz::RenderPassList *pass_list) {
  viz::CompositorFrame frame = viz::CompositorFrameBuilder()
                                   .SetRenderPassList(std::move(*pass_list))
                                   .Build();
  pass_list->clear();

  support_->SubmitCompositorFrame(local_surface_id_, std::move(frame));

  if (display_ != nullptr)
    display_->DrawAndSwap();
}

void DisplayGraffiti::ShowWindow() { platform_window_->Show(); }

void DisplayGraffiti::OnBoundsChanged(const gfx::Rect &new_bounds) {}

void DisplayGraffiti::OnDamageRect(const gfx::Rect &damaged_region) {}

void DisplayGraffiti::DispatchEvent(ui::Event *event) {}

void DisplayGraffiti::OnCloseRequest() {}

void DisplayGraffiti::OnClosed() {}

void DisplayGraffiti::OnWindowStateChanged(ui::PlatformWindowState new_state) {}

void DisplayGraffiti::OnLostCapture() {}

void DisplayGraffiti::OnAcceleratedWidgetAvailable(
    gfx::AcceleratedWidget widget, float device_pixel_ratio) {
  widget_ = widget;
}

void DisplayGraffiti::OnAcceleratedWidgetDestroyed() {
  widget_ = gfx::kNullAcceleratedWidget;
}

void DisplayGraffiti::OnActivationChanged(bool active) {}

} // namespace prelude

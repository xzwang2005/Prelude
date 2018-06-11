// Copyright (c) 2018 The Prelude Authors.
// Use of this source code is governed by MIT license that can be
// found in MIT-LICENSE file.

#ifndef _PRELUDE_DIRECT_OUTPUT_SURFACE_H_
#define _PRELUDE_DIRECT_OUTPUT_SURFACE_H_

#include "base/bind.h"
#include "components/viz/service/display/output_surface.h"
#include "components/viz/service/display/output_surface_client.h"
#include "components/viz/service/display/output_surface_frame.h"
#include "gpu/command_buffer/client/context_support.h"
#include "gpu/command_buffer/client/gles2_interface.h"
#include "gpu/command_buffer/common/gles2_cmd_utils.h"
#include "ui/compositor/test/in_process_context_provider.h"
#include "ui/gfx/presentation_feedback.h"
#include "ui/gl/gl_utils.h"

// An OutputSurface implementation that directly draws and swaps to an actual
// GL surface.

namespace prelude {

class DirectOutputSurface : public viz::OutputSurface {
public:
  explicit DirectOutputSurface(
      scoped_refptr<ui::InProcessContextProvider> context_provider);

  ~DirectOutputSurface() override;

  // viz::OutputSurface implementation.
  void BindToClient(viz::OutputSurfaceClient *client) override;
  void EnsureBackbuffer() override {}
  void DiscardBackbuffer() override {}
  void BindFramebuffer() override;
  void SetDrawRectangle(const gfx::Rect &rect) override {}
  void Reshape(const gfx::Size &size, float device_scale_factor,
               const gfx::ColorSpace &color_space, bool has_alpha,
               bool use_stencil) override;
  void SwapBuffers(viz::OutputSurfaceFrame frame) override;

  uint32_t GetFramebufferCopyTextureFormat() override;

  viz::OverlayCandidateValidator *GetOverlayCandidateValidator() const override;

  bool IsDisplayedAsOverlayPlane() const override;
  unsigned GetOverlayTextureId() const override;
  gfx::BufferFormat GetOverlayBufferFormat() const override;

  bool SurfaceIsSuspendForRecycle() const override;
  bool HasExternalStencilTest() const override;
  void ApplyExternalStencil() override {}
  //#if BUILDFLAG(ENABLE_VULKAN)
  //  gpu::VulkanSurface* GetVulkanSurface() override { return nullptr; }
  //#endif

private:
  void OnSwapBuffersComplete(uint64_t swap_id);

  viz::OutputSurfaceClient *client_ = nullptr;
  uint64_t swap_id_ = 0;
  base::WeakPtrFactory<DirectOutputSurface> weak_ptr_factory_;

  DISALLOW_COPY_AND_ASSIGN(DirectOutputSurface);
};

} // namespace prelude
#endif // _PRELUDE_DIRECT_OUTPUT_SURFACE_H_

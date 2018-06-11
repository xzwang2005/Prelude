// Copyright (c) 2018 The Prelude Authors.
// Use of this source code is governed by MIT license that can be
// found in MIT-LICENSE file.

#include "direct_output_surface.h"

namespace prelude {

DirectOutputSurface::DirectOutputSurface(
    scoped_refptr<ui::InProcessContextProvider> context_provider)
    : viz::OutputSurface(context_provider), weak_ptr_factory_(this) {
  capabilities_.flipped_output_surface = true;
}

DirectOutputSurface::~DirectOutputSurface() {}

void DirectOutputSurface::BindToClient(viz::OutputSurfaceClient *client) {
  client_ = client;
}

void DirectOutputSurface::BindFramebuffer() {
  context_provider()->ContextGL()->BindFramebuffer(GL_FRAMEBUFFER, 0);
}

void DirectOutputSurface::Reshape(const gfx::Size &size,
                                  float device_scale_factor,
                                  const gfx::ColorSpace &color_space,
                                  bool has_alpha, bool use_stencil) {
  context_provider()->ContextGL()->ResizeCHROMIUM(
      size.width(), size.height(), device_scale_factor,
      gl::GetGLColorSpace(color_space), has_alpha);
}

void DirectOutputSurface::SwapBuffers(viz::OutputSurfaceFrame frame) {
  DCHECK(context_provider_.get());
  if (frame.sub_buffer_rect) {
    context_provider_->ContextSupport()->PartialSwapBuffers(
        *frame.sub_buffer_rect);
  } else {
    context_provider_->ContextSupport()->Swap();
  }
  gpu::gles2::GLES2Interface *gl = context_provider_->ContextGL();
  const uint64_t fence_sync = gl->InsertFenceSyncCHROMIUM();
  gl->ShallowFlushCHROMIUM();

  gpu::SyncToken sync_token;
  gl->GenUnverifiedSyncTokenCHROMIUM(fence_sync, sync_token.GetData());

  context_provider_->ContextSupport()->SignalSyncToken(
      sync_token, base::Bind(&DirectOutputSurface::OnSwapBuffersComplete,
                             weak_ptr_factory_.GetWeakPtr(), ++swap_id_));
}

uint32_t DirectOutputSurface::GetFramebufferCopyTextureFormat() {
  auto *gl = static_cast<ui::InProcessContextProvider *>(context_provider());
  return gl->GetCopyTextureInternalFormat();
}

viz::OverlayCandidateValidator *
DirectOutputSurface::GetOverlayCandidateValidator() const {
  return nullptr;
}

bool DirectOutputSurface::IsDisplayedAsOverlayPlane() const { return false; }
unsigned DirectOutputSurface::GetOverlayTextureId() const { return 0; }
gfx::BufferFormat DirectOutputSurface::GetOverlayBufferFormat() const {
  return gfx::BufferFormat::RGBX_8888;
}

bool DirectOutputSurface::SurfaceIsSuspendForRecycle() const { return false; }
bool DirectOutputSurface::HasExternalStencilTest() const { return false; }

void DirectOutputSurface::OnSwapBuffersComplete(uint64_t swap_id) {
  client_->DidReceiveSwapBuffersAck(swap_id);
  client_->DidReceivePresentationFeedback(swap_id, gfx::PresentationFeedback());
}

} // namespace prelude

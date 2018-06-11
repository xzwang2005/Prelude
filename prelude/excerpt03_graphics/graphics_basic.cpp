#include "SkBlendMode.h"
#include "base/at_exit.h"
#include "base/bind.h"
#include "base/command_line.h"
#include "base/files/file_util.h"
#include "base/message_loop/message_loop.h"
#include "base/run_loop.h"
#include "base/task_scheduler/task_scheduler.h"
#include "base/time/time.h"
#include "components/viz/common/quads/render_pass.h"
#include "components/viz/common/quads/render_pass_draw_quad.h"
#include "components/viz/common/quads/shared_quad_state.h"
#include "components/viz/common/quads/solid_color_draw_quad.h"
#include "display_graffiti.h"
#include "ui/gl/init/gl_factory.h" // for gl::init

using viz::RenderPass;
using viz::RenderPassList;
using viz::SharedQuadState;
using viz::SolidColorDrawQuad;

namespace {

void WaitForMessageWindowShutdown(const base::Closure &callback) {
  printf("exit.\n");
  callback.Run();
}
} // namespace

int main(int argc, char **argv) {
  base::CommandLine::Init(argc, argv);
  base::AtExitManager exit_manager;
  gl::init::InitializeGLOneOff();

  base::MessageLoop loop;
  base::TaskScheduler::CreateAndStartWithDefaultParams("graphics_basic");

  std::unique_ptr<prelude::DisplayGraffiti> graffiti =
      std::make_unique<prelude::DisplayGraffiti>();
  DCHECK(graffiti);
  gfx::Size winSize(800, 600);
  graffiti->Initialize(winSize);

  // create a render pass
  viz::RenderPassList pass_list;
  auto pass = viz::RenderPass::Create();
  gfx::Rect rect(20, 20, 100, 100);
  const gfx::Transform transform_to_root;
  gfx::Rect output_rect(0, 0, winSize.width(), winSize.height());
  pass->SetNew(1, output_rect, rect, transform_to_root);

  // set quad state
  const gfx::Rect layer_rect = output_rect;
  const gfx::Rect visible_layer_rect = output_rect;
  const gfx::Rect clip_rect = output_rect;
  const bool is_clipped = false;
  const bool are_contents_opaque = false;
  const float opacity = 1.0f;
  const SkBlendMode blend_mode = SkBlendMode::kSrcOver;
  int sorting_context_id = 0;

  // for root render pass, its output_rect must match the size of the surface
  SharedQuadState *shared_state = pass->CreateAndAppendSharedQuadState();
  shared_state->SetAll(gfx::Transform(), layer_rect, visible_layer_rect,
                       clip_rect, is_clipped, are_contents_opaque, opacity,
                       blend_mode, sorting_context_id);

  auto *color_quad = pass->CreateAndAppendDrawQuad<viz::SolidColorDrawQuad>();
  color_quad->SetNew(shared_state, rect, rect, SK_ColorGREEN, false);

  pass_list.push_back(std::move(pass));

  graffiti->SubmitAndDrawCompositorFrame(&pass_list);

  printf("show the window\n");
  graffiti->ShowWindow();

  base::RunLoop run_loop;

  // allow program to quit after 3 seconds.
  base::ThreadTaskRunnerHandle::Get()->PostDelayedTask(
      FROM_HERE,
      base::BindOnce(&WaitForMessageWindowShutdown, run_loop.QuitClosure()),
      base::TimeDelta::FromSeconds(3));
  run_loop.Run();

  graffiti->ShutDown();

  return 0;
}

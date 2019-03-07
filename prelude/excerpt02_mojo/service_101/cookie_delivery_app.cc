// Copyright (c) 2019 The Prelude Authors.
// Use of this source code is governed by MIT license that can be
// found in MIT-LICENSE file.

#include "base/at_exit.h"
#include "base/base_switches.h"
#include "base/bind.h"
#include "base/command_line.h"
#include "base/macros.h"
#include "base/message_loop/message_loop.h"
#include "base/run_loop.h"
#include "base/task/post_task.h"
#include "base/task/task_scheduler/task_scheduler.h"
#include "mojo/core/embedder/embedder.h"
#include "mojo/core/embedder/scoped_ipc_support.h"
#include "services/catalog/catalog.h"
#include "services/service_manager/background/background_service_manager.h"
#include "services/service_manager/public/cpp/connector.h"
#include "services/service_manager/public/cpp/service.h"
#include "services/service_manager/public/cpp/service_context.h"

#include "prelude/excerpt02_mojo/public/mojom/fortune_cookie.mojom.h"

#include <iostream>

namespace {
void ReceiveOneStringAndClose(base::Closure callback,
                              const std::string& in_string) {
  std::cout << "cookie says: " << in_string << std::endl;
  callback.Run();
}
}  // namespace

int main(int argc, char** argv) {
  CHECK(base::CommandLine::Init(argc, argv));
  base::AtExitManager exit_manager;

  const base::CommandLine* command_line =
      base::CommandLine::ForCurrentProcess();
  if (command_line->HasSwitch(switches::kWaitForDebugger)) {
    base::debug::WaitForDebugger(60, true);
  }

  base::MessageLoop main_loop;
  // must have for inter-process communication
  base::TaskScheduler::CreateAndStartWithDefaultParams("cookie_delivery_app");
  mojo::core::Init();
  // must have for inter-process communication
  base::Thread ipc_thread_("IPC thread");
  ipc_thread_.StartWithOptions(
      base::Thread::Options(base::MessageLoop::TYPE_IO, 0));
  mojo::core::ScopedIPCSupport ipc_support_(
      ipc_thread_.task_runner(),
      mojo::core::ScopedIPCSupport::ShutdownPolicy::CLEAN);

  // load catalog data
  base::FilePath catalog_path(
      FILE_PATH_LITERAL("cookie_delivery_app_catalog.json"));
  catalog::Catalog::LoadDefaultCatalogManifest(catalog_path);

  auto background_service_manager =
      std::make_unique<service_manager::BackgroundServiceManager>(nullptr,
                                                                  nullptr);

  service_manager::mojom::ServicePtr service;
  auto context = std::make_unique<service_manager::ServiceContext>(
      std::make_unique<service_manager::Service>(),
      mojo::MakeRequest(&service));

  background_service_manager->RegisterService(
      service_manager::Identity(prelude::mojom::kCookieDeliveryApp,
                                service_manager::mojom::kRootUserID),
      std::move(service), nullptr);

  prelude::mojom::FortuneCookiePtr cookie;
  context->connector()->BindInterface("cookie_delivery_service", &cookie);

  base::RunLoop loop;
  cookie->Crack(base::BindOnce(&ReceiveOneStringAndClose, loop.QuitClosure()));

  loop.Run();
  return 0;
}

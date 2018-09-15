// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/at_exit.h"
#include "base/base_switches.h"
#include "base/command_line.h"
#include "base/macros.h"
#include "base/memory/ptr_util.h"
#include "base/run_loop.h"
#include "base/strings/stringprintf.h"
#include "base/strings/utf_string_conversions.h"
#include "mojo/edk/embedder/connection_params.h"
#include "mojo/edk/embedder/embedder.h"
#include "mojo/edk/embedder/outgoing_broker_client_invitation.h"
#include "mojo/edk/embedder/platform_channel_pair.h"
#include "mojo/edk/embedder/scoped_ipc_support.h"
#include "mojo/edk/embedder/transport_protocol.h"
#include "mojo/public/cpp/bindings/binding_set.h"
#include "prelude/excerpt04_service/single_class_service_impl/single_class_service_impl.h"
#include "services/catalog/catalog.h"
#include "services/service_manager/background/background_service_manager.h"
#include "services/service_manager/public/c/main.h"
#include "services/service_manager/public/cpp/binder_registry.h"
#include "services/service_manager/public/cpp/service.h"
#include "services/service_manager/public/cpp/service_context.h"
#include "services/service_manager/public/cpp/service_runner.h"
#include "services/service_manager/public/interfaces/service_factory.mojom.h"
#include "services/service_manager/public/interfaces/connector.mojom.h"
#include "services/service_manager/public/interfaces/constants.mojom.h"
#include <iostream>
#include <string>
#include <memory>

namespace {

void OnGetTail(const base::Closure &callback, const std::string &message) {
  // EXPECT_EQ(message, "test logger not equal");
  std::cout << "received message: " << message << std::endl;
  callback.Run();
}

} // namespace

namespace prelude {
namespace excerpt04 {
class Embedder : public service_manager::Service,
                 service_manager::mojom::ServiceFactory {
public:
  Embedder() {
    registry_.AddInterface<service_manager::mojom::ServiceFactory>(
        base::Bind(&Embedder::Create, base::Unretained(this)));
  }
  ~Embedder() override {}

private:
  // service_manager::Service:
  void OnBindInterface(const service_manager::BindSourceInfo &source_info,
                       const std::string &interface_name,
                       mojo::ScopedMessagePipeHandle interface_pipe) override {
    registry_.BindInterface(interface_name, std::move(interface_pipe));
  }

  bool OnServiceManagerConnectionLost() override {
    base::RunLoop::QuitCurrentWhenIdleDeprecated();
    return true;
  }

  void Create(service_manager::mojom::ServiceFactoryRequest request) {
    service_factory_bindings_.AddBinding(this, std::move(request));
  }

  // service_manager::mojom::ServiceFactory:
  void CreateService(service_manager::mojom::ServiceRequest request,
                     const std::string &name) override {
    if (name == "in_process_logger_service") {
      context_.reset(new service_manager::ServiceContext(
          std::make_unique<SingleClassServiceImpl>(), std::move(request)));
    } else {
      LOG(ERROR) << "Failed to create unknown service " << name;
    }
  }

  std::unique_ptr<service_manager::ServiceContext> context_;
  service_manager::BinderRegistry registry_;
  mojo::BindingSet<service_manager::mojom::ServiceFactory>
      service_factory_bindings_;

  DISALLOW_COPY_AND_ASSIGN(Embedder);
};

} // namespace excerpt04
} // namespace prelude

int main(int argc, char **argv) {
  base::AtExitManager exit_manager_;
  CHECK(base::CommandLine::Init(argc, argv));
  base::MessageLoop main_loop;

  if (base::CommandLine::ForCurrentProcess()->HasSwitch(
          switches::kWaitForDebugger)) {
#if defined(OS_WIN)
    std::string app{"launcher"};
    base::string16 appw = base::UTF8ToUTF16(app);
    base::string16 message = base::UTF8ToUTF16(
        base::StringPrintf("%s - %ld", app.c_str(), GetCurrentProcessId()));
    MessageBox(NULL, message.c_str(), appw.c_str(), MB_OK | MB_SETFOREGROUND);
#else
    LOG(ERROR) << app << " waiting for GDB. pid: " << getpid();
    base::debug::WaitForDebugger(60, true);
#endif
  }

  // step 0. prepare catalog data
  base::FilePath catalog_path_(
      FILE_PATH_LITERAL("in_process_app_catalog.json"));
  catalog::Catalog::LoadDefaultCatalogManifest(catalog_path_);

  // step 1. setup mojo and ipc
  mojo::edk::Init();
#if defined(OS_MACOSX) && !defined(OS_IOS)
  mojo::edk::SetMachPortProvider(
      service_manager::MachBroker::GetInstance()->port_provider());
#endif
  base::Thread ipc_thread_("IPC thread");
  ipc_thread_.StartWithOptions(
      base::Thread::Options(base::MessageLoop::TYPE_IO, 0));
  mojo::edk::ScopedIPCSupport ipc_support_(
      ipc_thread_.task_runner(),
      mojo::edk::ScopedIPCSupport::ShutdownPolicy::CLEAN);

  // step 2. setup client side
  // first create the background service manager
  auto background_service_manager_ =
      std::make_unique<service_manager::BackgroundServiceManager>(nullptr,
                                                                  nullptr);

  // create client side service
  service_manager::mojom::ServicePtr service; // ???
  auto context_ = std::make_unique<service_manager::ServiceContext>(
      std::make_unique<prelude::excerpt04::Embedder>(),
      mojo::MakeRequest(&service));

  // register client side service to service manager
  background_service_manager_->RegisterService(
      service_manager::Identity("in_process_app",
                                service_manager::mojom::kRootUserID),
      std::move(service), nullptr);

  //service_manager::mojom::ServiceFactoryPtr factory;
  //context_->connector()->BindInterface("in_process_app", &factory);

  // step 4. call service
  prelude::mojom::LoggerPtr logger;
  context_->connector()->BindInterface("in_process_logger_service", &logger);
  base::RunLoop loop;
  logger->Log("test logger");

  // get message back
  logger->GetTail(base::BindOnce(&OnGetTail, loop.QuitClosure()));
  loop.Run();

  return 0;
}

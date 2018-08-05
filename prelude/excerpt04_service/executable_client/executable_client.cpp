// Copyright (c) 2018 The Prelude Authors.
// Use of this source code is governed by MIT license that can be
// found in MIT-LICENSE file.

#include "base/at_exit.h"
#include "base/bind.h"
#include "base/command_line.h"
#include "base/macros.h"
#include "base/message_loop/message_loop.h"
#include "base/run_loop.h"
#include "mojo/edk/embedder/embedder.h"
#include "mojo/edk/embedder/scoped_ipc_support.h"
#include "mojo/public/cpp/bindings/binding_set.h"

#include "services/catalog/catalog.h"
#include "services/service_manager/background/background_service_manager.h"
#include "services/service_manager/public/cpp/connector.h"
#include "services/service_manager/public/cpp/service.h"
#include "services/service_manager/public/cpp/service_context.h"

#include "prelude/excerpt04_service/executable_client/executable_client__catalog_source.h"
#include "prelude/excerpt04_service/interfaces/constants.mojom.h"
#include "prelude/excerpt04_service/interfaces/logger.mojom.h"

#include <iostream>
#include <memory>

namespace prelude {
namespace excerpt04 {

class ClientService : public service_manager::Service {
public:
  explicit ClientService() = default;
  ~ClientService() override = default;

private:
  void OnStart() override{};
  void OnBindInterface(const service_manager::BindSourceInfo &source_info,
                       const std::string &interface_name,
                       mojo::ScopedMessagePipeHandle interface_pipe) override{};

  DISALLOW_COPY_AND_ASSIGN(ClientService);
};

class SampleServiceConsumer {
public:
  explicit SampleServiceConsumer() {}
  ~SampleServiceConsumer() {
    // tear down
    background_service_manager_.reset();
    context_.reset();
  }

  void SetUp() {
    background_service_manager_ =
        base::MakeUnique<service_manager::BackgroundServiceManager>(nullptr,
                                                                    nullptr);
    // Create the service manager connection. We don't proceed until we get our
    // Service's OnStart() method is called.
    base::MessageLoop::ScopedNestableTaskAllower allow(
        base::MessageLoop::current());

    service_manager::mojom::ServicePtr service;
    context_ = base::MakeUnique<service_manager::ServiceContext>(
        CreateService(), mojo::MakeRequest(&service));
    background_service_manager_->RegisterService(
        service_manager::Identity("executable_client", //!!! register itself !!!
                                  service_manager::mojom::kRootUserID),
        std::move(service), nullptr);
    connector_ = context_->connector();
  }

  service_manager::Connector *connector() { return connector_; }

private:
  std::unique_ptr<service_manager::Service> CreateService() {
    return base::MakeUnique<ClientService>();
  }

  // base::test::ScopedTaskEnvironment scoped_task_environment_;
  std::unique_ptr<service_manager::ServiceContext> context_;
  std::unique_ptr<service_manager::BackgroundServiceManager>
      background_service_manager_;
  service_manager::Connector *connector_ = nullptr;

  DISALLOW_COPY_AND_ASSIGN(SampleServiceConsumer);
};

} // namespace excerpt04
} // namespace prelude

void OnGetTail(const base::Closure &callback, const std::string &message) {
  // EXPECT_EQ(message, "test logger not equal");
  std::cout << "received message: " << message << std::endl;
  callback.Run();
}

int main(int argc, char **argv) {
  base::AtExitManager exit_manager_;
  CHECK(base::CommandLine::Init(argc, argv));

  base::MessageLoop main_loop;

  // copied from service_manager::InitializeAndLaunchUnitTests()
  catalog::Catalog::SetDefaultCatalogManifest(prelude::CreateCatalog());
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

  // create the service
  std::unique_ptr<prelude::excerpt04::SampleServiceConsumer> service_ =
      std::make_unique<prelude::excerpt04::SampleServiceConsumer>();

  service_->SetUp();
  // not necessary to call service_->connector()->StartService("multi_class_service")
  prelude::mojom::LoggerPtr logger;
  service_->connector()->BindInterface(prelude::mojom::kMultiClassServiceName,
                                       &logger);
  base::RunLoop loop;

  logger->Log("test logger");

  // get message back
  logger->GetTail(base::BindOnce(&OnGetTail, loop.QuitClosure()));
  loop.Run();

  // delete service_;
  return 0;
}

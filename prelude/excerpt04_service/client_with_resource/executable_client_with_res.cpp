// Copyright (c) 2018 The Prelude Authors.
// Use of this source code is governed by MIT license that can be
// found in MIT-LICENSE file.

#include "base/at_exit.h"
#include "base/bind.h"
#include "base/command_line.h"
#include "base/files/file_path.h"
#include "base/guid.h"
#include "base/json/json_reader.h"
#include "base/macros.h"
#include "base/message_loop/message_loop.h"
#include "base/path_service.h"
#include "base/run_loop.h"
#include "base/task_scheduler/task_scheduler.h"
#include "base/test/scoped_task_environment.h"
#include "base/threading/thread.h"
#include "base/win/resource_util.h"
#include "mojo/edk/embedder/embedder.h"
#include "mojo/edk/embedder/scoped_ipc_support.h"
#include "mojo/public/cpp/bindings/binding_set.h"

#include "services/catalog/catalog.h"
#include "services/service_manager/background/background_service_manager.h"
#include "services/service_manager/public/cpp/connector.h"
#include "services/service_manager/public/cpp/service.h"
#include "services/service_manager/public/cpp/service_context.h"

#include "prelude/excerpt04_service/interfaces/constants.mojom.h"
#include "prelude/excerpt04_service/interfaces/logger.mojom.h"

#include "prelude/excerpt04_service/client_with_resource/logger_resource.h"
#include "ui/base/resource/resource_bundle.h"

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
        service_manager::Identity(
            "executable_client_with_res", //!!! register itself !!!
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

void OnGetTailNoClosure(const std::string &message) {
  std::cout << "received message without close run loop:" << message << std::endl;
}

void OnGetTailGetResult(std::string *out_string, const std::string &message) {
  std::cout << "copy string back." << message << std::endl;
  *out_string = message;
}

void OnGetTail(const base::Closure &callback, const std::string &message) {
  std::cout << "received message: " << message << std::endl;
  callback.Run();
}

void OnGetLogCount(int8_t* out_result, const int8_t in_result) {
  std::cout << "OnGetLogCount: " << static_cast<int>(in_result) << std::endl;
  *out_result = in_result;
}

int main(int argc, char **argv) {
  base::AtExitManager exit_manager_;
  CHECK(base::CommandLine::Init(argc, argv));

  base::MessageLoopForIO main_loop;
  base::TaskScheduler::CreateAndStartWithDefaultParams(
      "executable_client_with_resource");

  // initialize ResourceBundle
  base::FilePath path;
  PathService::Get(base::DIR_MODULE, &path);
  base::FilePath pak_file_strings =
      path.Append(FILE_PATH_LITERAL("logger_resource.pak"));

  std::string path_str = pak_file_strings.MaybeAsASCII();

  ui::ResourceBundle::InitSharedInstanceWithPakPath(pak_file_strings);
  ui::ResourceBundle &rb = ui::ResourceBundle::GetSharedInstance();
  base::StringPiece manifest = rb.GetRawDataResourceForScale(
      IDR_MOJO_LOGGER_SERVICE_MANIFEST, ui::ScaleFactor::SCALE_FACTOR_NONE);

  std::unique_ptr<base::Value> manifest_value =
      base::JSONReader::Read(manifest);
  DCHECK(manifest_value);

  // copied from service_manager::InitializeAndLaunchUnitTests()
  catalog::Catalog::SetDefaultCatalogManifest(std::move(manifest_value));

  // initialize mojo
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

  // create two service
  std::unique_ptr<prelude::excerpt04::SampleServiceConsumer> service_a_ =
      std::make_unique<prelude::excerpt04::SampleServiceConsumer>();

  std::unique_ptr<prelude::excerpt04::SampleServiceConsumer> service_b_ =
    std::make_unique<prelude::excerpt04::SampleServiceConsumer>();

  service_a_->SetUp();
  service_b_->SetUp();
  // not necessary to call
  // service_->connector()->StartService("multi_class_service")
  prelude::mojom::LoggerPtr logger;
  // bind with an identity, the identity constructor takes 1 ~ 3 strings
  // this requires service_manager:user_Id and instance_name
  // in the multi_class_service manifest

  // prepare two identities
  const std::string comm_user_id{ "981f235c-0bbc-4da6-a689-dc3193e02c39" };
  service_manager::Identity bogart(prelude::mojom::kMultiClassServiceName,
    comm_user_id, "bogart");
  service_manager::Identity maya(prelude::mojom::kMultiClassServiceName,
    comm_user_id, "maya");
  {
    service_a_->connector()->BindInterface(bogart, &logger);
    // a typical use case, a round trip
    base::RunLoop loop;
    // get message back
    logger->Log("message 1");
    logger->GetTail(base::BindOnce(&OnGetTailNoClosure));

    // echo_string is re-written only after loop.Run() is called. i.e.,
    // retrieve result is async
    std::string echo_string{ "echo" };
    logger->Log("message 2");
    logger->GetTail(base::BindOnce(&OnGetTailGetResult, &echo_string));
    std::cout << "display string in main: " << echo_string << std::endl;

    logger->Log("message 3");
    logger->GetTail(base::BindOnce(&OnGetTailNoClosure));

    logger->Log("last message");
    logger->GetTail(base::BindOnce(&OnGetTail, loop.QuitClosure()));
    // Run() starts the service instance.
    loop.Run();
    std::cout << "display string in main after Run: " << echo_string << std::endl;
  }


  // binding with same identity to the same service instance (service_a_),
  // get the same log instance
  prelude::mojom::LoggerPtr another_logger;
  {
    service_a_->connector()->BindInterface(bogart, &another_logger);
    base::RunLoop loop;
    int8_t count;
    another_logger->GetLogCount(base::BindOnce(&OnGetLogCount, &count));
    another_logger->Log("message with another_logger");
    another_logger->GetTail(base::BindOnce(&OnGetTail, loop.QuitClosure()));
    another_logger->GetLogCount(base::BindOnce(&OnGetLogCount, &count));
    loop.Run();
    std::cout << "another_logger final log count: " << static_cast<int>(count) << std::endl;
  }

  // binding a different identity, get a different log instance
  prelude::mojom::LoggerPtr third_logger;
  {
    service_a_->connector()->BindInterface(maya, &third_logger);
    base::RunLoop loop;
    int8_t count;
    third_logger->GetLogCount(base::BindOnce(&OnGetLogCount, &count));
    third_logger->Log("message with the third logger");
    third_logger->GetTail(base::BindOnce(&OnGetTail, loop.QuitClosure()));
    third_logger->GetLogCount(base::BindOnce(&OnGetLogCount, &count));
    loop.Run();
    std::cout << "another_logger final log count: " << static_cast<int>(count) << std::endl;
  }
  // delete service_;
  return 0;
}

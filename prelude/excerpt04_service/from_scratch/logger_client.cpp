// Copyright (c) 2018 The Prelude Authors.
// Use of this source code is governed by MIT license that can be
// found in MIT-LICENSE file.

#include "base/at_exit.h"
#include "base/base_switches.h"
#include "base/bind.h"
#include "base/command_line.h"
#include "base/macros.h"
#include "base/message_loop/message_loop.h"
#include "base/path_service.h"
#include "base/process/launch.h"
#include "base/run_loop.h"
#include "mojo/edk/embedder/connection_params.h"
#include "mojo/edk/embedder/embedder.h"
#include "mojo/edk/embedder/outgoing_broker_client_invitation.h"
#include "mojo/edk/embedder/platform_channel_pair.h"
#include "mojo/edk/embedder/scoped_ipc_support.h"
#include "mojo/edk/embedder/transport_protocol.h"
#include "mojo/public/cpp/bindings/binding_set.h"
#include "prelude/excerpt04_service/interfaces/constants.mojom.h"
#include "prelude/excerpt04_service/interfaces/logger.mojom.h"
#include "services/catalog/catalog.h"
#include "services/service_manager/background/background_service_manager.h"
#include "services/service_manager/public/cpp/connector.h"
#include "services/service_manager/public/cpp/identity.h"
#include "services/service_manager/public/cpp/service.h"
#include "services/service_manager/public/cpp/service_context.h"
#include "services/service_manager/public/interfaces/connector.mojom.h"
#include "services/service_manager/public/interfaces/constants.mojom.h"
#include "services/service_manager/runner/common/client_util.h"
#include <iostream>
#include <memory>
#include "base/strings/utf_string_conversions.h"
#include "base/strings/stringprintf.h"

namespace {
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

void OnGetTail(const base::Closure &callback, const std::string &message) {
  // EXPECT_EQ(message, "test logger not equal");
  std::cout << "received message: " << message << std::endl;
  callback.Run();
}

} // namespace

int main(int argc, char **argv) {
  base::AtExitManager exit_manager_;
  CHECK(base::CommandLine::Init(argc, argv));
  base::MessageLoop main_loop;

  if (base::CommandLine::ForCurrentProcess()->HasSwitch(
          switches::kWaitForDebugger)) {
#if defined(OS_WIN)
    std::string app{ "launcher" };
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
  base::FilePath catalog_path_(FILE_PATH_LITERAL("logger_client_catalog.json"));
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
  service_manager::mojom::ServicePtr service;
  auto context_ = std::make_unique<service_manager::ServiceContext>(
      std::make_unique<ClientService>(), mojo::MakeRequest(&service));

  // register client side service to service manager
  background_service_manager_->RegisterService(
      service_manager::Identity("logger_client",
                                service_manager::mojom::kRootUserID),
      std::move(service), nullptr);

  // step 3. launch the service process. ref: ServiceManagerTest::StartTarget()
  base::FilePath target_path;
  CHECK(base::PathService::Get(base::DIR_EXE, &target_path));
#if defined(OS_WIN)
  target_path =
      target_path.Append(FILE_PATH_LITERAL("logger_executable.exe"));
#else
  target_path =
      target_path.Append(FILE_PATH_LITERAL("logger_executable"));
#endif

  base::CommandLine child_command_line(target_path);
  //// Forward the wait-for-debugger flag but nothing else - we don't want to
  //// stamp on the platform-channel flag.
  //if (base::CommandLine::ForCurrentProcess()->HasSwitch(
  //        switches::kWaitForDebugger)) {
  //  child_command_line.AppendSwitch(switches::kWaitForDebugger);
  //}

  // Create the channel to be shared with the target process. Pass one end
  // on the command line.
  mojo::edk::PlatformChannelPair platform_channel_pair;
  base::LaunchOptions options;
#if defined(OS_WIN)
  platform_channel_pair.PrepareToPassClientHandleToChildProcess(
      &child_command_line, &options.handles_to_inherit);
#elif defined(OS_FUCHSIA)
  platform_channel_pair.PrepareToPassClientHandleToChildProcess(
      &child_command_line, &options.handles_to_transfer);
#else
  platform_channel_pair.PrepareToPassClientHandleToChildProcess(
      &child_command_line, &options.fds_to_remap);
#endif

  mojo::edk::OutgoingBrokerClientInvitation invitation;
  service_manager::mojom::ServicePtr client =
      service_manager::PassServiceRequestOnCommandLine(&invitation,
                                                       &child_command_line);
  service_manager::mojom::PIDReceiverPtr receiver;

  service_manager::Identity target("logger_executable",
                                   service_manager::mojom::kInheritUserID);
  context_->connector()->StartService(target, std::move(client), MakeRequest(&receiver));

  base::Process target_ = base::LaunchProcess(child_command_line, options);
  DCHECK(target_.IsValid());
  platform_channel_pair.ChildProcessLaunched();
  receiver->SetPID(target_.Pid());
  invitation.Send(
      target_.Handle(),
      mojo::edk::ConnectionParams(mojo::edk::TransportProtocol::kLegacy,
                                  platform_channel_pair.PassServerHandle()));

  // step 4. call service
  prelude::mojom::LoggerPtr logger;
  context_->connector()->BindInterface("logger_executable",
                                       &logger);
  base::RunLoop loop;
  logger->Log("test logger");

  // get message back
  logger->GetTail(base::BindOnce(&OnGetTail, loop.QuitClosure()));
  loop.Run();

  return 0;
}

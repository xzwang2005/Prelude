// Copyright (c) 2019 The Prelude Authors.
// Use of this source code is governed by MIT license that can be
// found in MIT-LICENSE file.

#include "base/at_exit.h"
#include "base/base_switches.h"
#include "base/bind.h"
#include "base/command_line.h"
#include "base/files/file_util.h"
#include "base/json/json_reader.h"
#include "base/macros.h"
#include "base/message_loop/message_loop.h"
#include "base/path_service.h"
#include "base/run_loop.h"
#include "mojo/core/embedder/embedder.h"
#include "services/catalog/catalog.h"
#include "services/service_manager/background/background_service_manager.h"
#include "services/service_manager/public/cpp/connector.h"
#include "services/service_manager/public/cpp/service.h"
#include "services/service_manager/public/cpp/service_context.h"

#include "prelude/excerpt02_mojo/service_101/vanilla_cookie_service.h"

#include <iostream>

namespace {
const char kServiceName[] = "cookie_dine_in_service";
void ReceiveOneStringAndClose(base::Closure callback,
                              const std::string& in_string) {
  std::cout << "cookie says: " << in_string << std::endl;
  callback.Run();
}
}  // namespace

int main(int argc, char** argv) {
  CHECK(base::CommandLine::Init(argc, argv));
  base::AtExitManager exit_manager_;

  const base::CommandLine* command_line =
      base::CommandLine::ForCurrentProcess();
  if (command_line->HasSwitch(switches::kWaitForDebugger)) {
    base::debug::WaitForDebugger(60, true);
  }

  base::MessageLoop main_loop;

  base::FilePath exe_path;
  base::PathService::Get(base::FILE_EXE, &exe_path);
  base::FilePath jsonPath(FILE_PATH_LITERAL("dine_in_catalog.json"));
  auto catalog_path = exe_path.DirName().Append(jsonPath);

  std::string catalog_string;
  bool read_success = base::ReadFileToString(catalog_path, &catalog_string);

  if (!read_success) {
    std::cout << "Failed to read catalog file.\n";
    return 1;
  }

  //// read json
  int error_code;
  std::string error;
  std::unique_ptr<base::Value> json_value =
      base::JSONReader::ReadAndReturnError(catalog_string, base::JSON_PARSE_RFC,
                                           &error_code, &error);

  if (!json_value) {
    std::cout << "Failed to parse json file.\n";
    return 1;
  }

  mojo::core::Init();

  auto background_service_manager(
      std::make_unique<service_manager::BackgroundServiceManager>(
          nullptr, std::move(json_value)));

  service_manager::mojom::ServicePtr service;
  auto service_ctx(std::make_unique<service_manager::ServiceContext>(
      std::make_unique<prelude::VanillaCookieService>(),
      mojo::MakeRequest(&service)));

  background_service_manager->RegisterService(
      service_manager::Identity(kServiceName,
                                service_manager::mojom::kRootUserID),
      std::move(service), nullptr);

  prelude::mojom::FortuneCookiePtr cookie;
  service_ctx->connector()->BindInterface(kServiceName, &cookie);

  base::RunLoop loop;
  cookie->Crack(base::BindOnce(&ReceiveOneStringAndClose, loop.QuitClosure()));

  loop.Run();
  return 0;
}

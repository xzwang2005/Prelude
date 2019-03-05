// Copyright (c) 2019 The Prelude Authors.
// Use of this source code is governed by MIT license that can be
// found in MIT-LICENSE file.

#include "prelude/excerpt02_mojo/mojo_101/fortune_cookie_impl.h"

#include <iostream>
#include <string>

#include "base/at_exit.h"
#include "base/bind.h"
#include "base/command_line.h"
#include "base/message_loop/message_loop.h"
#include "base/run_loop.h"
#include "base/sequenced_task_runner.h"
#include "mojo/core/embedder/embedder.h"

namespace {

std::string copy_msg;
base::Closure cb;

void PrintAndQuit() {
  std::cout << "check the copied message asynchronously: " << copy_msg << std::endl;
  std::move(cb).Run();
}

void CopyMessage(std::string* out_msg, const std::string& in_msg) {
  std::cout << "Copy message: " << in_msg << std::endl;
  *out_msg = in_msg;
  base::SequencedTaskRunnerHandle::Get()->PostTask(
      FROM_HERE, base::BindOnce(&PrintAndQuit));
}

}  // namespace

int main(int argc, char** argv) {
  CHECK(base::CommandLine::Init(argc, argv));
  base::AtExitManager exit_manager_;

  // mojo requires a sequenced context, i.e., a message loop in the thread.
  base::MessageLoop main_loop;
  mojo::core::Init();

  auto real_cookie = std::make_unique<prelude::FortuneCookieImplAlpha>();

  prelude::mojom::FortuneCookiePtr cookie_ptr;
  real_cookie->BindRequest(mojo::MakeRequest(&cookie_ptr));

  base::RunLoop loop;
  cb = loop.QuitClosure();

  cookie_ptr->Crack(base::BindOnce(&CopyMessage, &copy_msg));
  // since Crack() is called asynchronously, |copy_msg| is empty here
  std::cout << "check the copied message synchronously: " << (copy_msg.empty() ? "Empty" : copy_msg) << std::endl;

  real_cookie->EatMe();

  loop.Run();

  return 0;
}

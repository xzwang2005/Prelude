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
#include "mojo/core/embedder/embedder.h"

namespace {
void CopyMessage(std::string* out_msg, const std::string& in_msg) {
  std::cout << "Copy message: " << in_msg << std::endl;
  *out_msg = in_msg;
}

void PrintAndQuit(base::Closure callback, const std::string& msg) {
  std::cout << "cookie wisdom: " << msg << std::endl;
  std::move(callback).Run();
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

  std::string note;
  cookie_ptr->Crack(base::BindOnce(&CopyMessage, &note));
  // since Crack() is called asynchronously, |note| is empty
  std::cout << "check the copied message: " << note << std::endl;

  cookie_ptr->Crack(base::BindOnce(&PrintAndQuit, loop.QuitClosure()));

  real_cookie->EatMe();

  loop.Run();

  return 0;
}

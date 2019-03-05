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

int main(int argc, char** argv) {
  CHECK(base::CommandLine::Init(argc, argv));
  base::AtExitManager exit_manager_;

  // mojo requires a sequenced context, i.e., a message loop in the thread.
  base::MessageLoop main_loop;
  mojo::core::Init();

  auto real_cookie = std::make_unique<prelude::FortuneCookieImplAlpha>();

  prelude::mojom::FortuneCookiePtr cookie_ptr;
  real_cookie->BindRequest(mojo::MakeRequest(&cookie_ptr));

  // although the method is defined as private, it can still be called through
  // mojo interface pointer. But this call is asynchronous, which won't run
  // until RunLoop().Run() is called
  cookie_ptr->Crack(base::DoNothing());

  // this call is executed BEFORE Crack() above
  real_cookie->EatMe();

  //base::RunLoop().RunUntilIdle();

  return 0;
}

// Copyright (c) 2018 The Prelude Authors.
// Use of this source code is governed by MIT license that can be
// found in MIT-LICENSE file.

#include "base/at_exit.h"
#include "base/command_line.h"
#include "base/macros.h"
#include "base/location.h"
#include "base/message_loop/message_loop.h"
#include "base/task_scheduler/post_task.h"
#include "base/task_scheduler/task_scheduler.h"
#include "base/run_loop.h"
#include "build/build_config.h"

namespace {

void SayHello(std::string name) {
    printf("hello %s\n", name.c_str());
}
}

int main(int argc, char** argv) {
    base::CommandLine::Init(argc, argv);
    base::AtExitManager exit_manager;

    base::MessageLoop main_loop;
    base::TaskScheduler::CreateAndStartWithDefaultParams("task_demo");
    // now can use the APIs in post_task.h

    // post a simple one-off task
    base::PostTask(FROM_HERE, base::BindOnce(&SayHello, "bogart"));

    base::RunLoop().RunUntilIdle();
}
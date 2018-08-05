// Copyright (c) 2018 The Prelude Authors.
// Use of this source code is governed by MIT license that can be
// found in MIT-LICENSE file.

#include "base/at_exit.h"
#include "base/command_line.h"
#include "base/location.h"
#include "base/macros.h"
#include "base/message_loop/message_loop.h"
#include "base/run_loop.h"
#include "base/task_scheduler/post_task.h"
#include "base/task_scheduler/task_scheduler.h"
#include "base/threading/platform_thread.h"
#include "build/build_config.h"

#include <string>

namespace {
void SayHello(std::string name) {
  printf("%s says hello in thread %lu.\n", name.c_str(),
         static_cast<unsigned long>(base::PlatformThread::CurrentId()));
}

// simulate a function that takes a long time
void SayLongHello(std::string name) {
  base::PlatformThread::Sleep(base::TimeDelta::FromSeconds(2));
  printf("%s says heeeeelloooooo in thread %lu.\n", name.c_str(),
         static_cast<unsigned long>(base::PlatformThread::CurrentId()));
}

void HelloToYou2(const base::Closure &callback) {
  printf("Hello to you 2~ from thread %lu.\n",
         static_cast<unsigned long>(base::PlatformThread::CurrentId()));
  std::move(callback).Run();
}

} // namespace

int main(int argc, char **argv) {
  base::CommandLine::Init(argc, argv);
  base::AtExitManager exit_manager;

  // uncomment this line to attach to the process
  // base::debug::WaitForDebugger(60, true);

  auto main_thread_id = base::PlatformThread::CurrentId();
  printf("main thread id: %lu\n", main_thread_id);

  base::MessageLoop main_loop;
  base::RunLoop loop;
  // post a task to the main thread
  main_loop.task_runner()->PostTask(FROM_HERE,
                                    base::BindOnce(&SayHello, "maya"));

  base::TaskScheduler::CreateAndStartWithDefaultParams("task_demo");
  // now can use the APIs in post_task.h
  base::PostTask(FROM_HERE, base::BindOnce(&SayHello, "bogart"));
  printf("post task to scheduler. \n");

  // post a delayed task to scheduler. main thread does not wait for the delayed
  // task and returns before the task is performed.
  base::PostDelayedTask(FROM_HERE, base::BindOnce(&SayHello, "bogart"),
                        base::TimeDelta::FromSeconds(3));
  printf("post delayed task to scheduler. \n");

  base::PostTaskAndReply(FROM_HERE, base::BindOnce(&SayLongHello, "bogart"),
                         base::BindOnce(&HelloToYou2, loop.QuitClosure()));
  printf("posted and replay.\n");

  loop.Run();
  printf("main thread done.\n");
  return 0;
}

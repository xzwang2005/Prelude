// Copyright (c) 2019 The Prelude Authors.
// Use of this source code is governed by MIT license that can be
// found in MIT-LICENSE file.

#include "prelude/excerpt02_mojo/mojo_101/radiation_listener_impl.h"
#include "prelude/excerpt02_mojo/mojo_101/radiation_monitor_impl.h"

#include <iostream>
#include <string>

#include "base/at_exit.h"
#include "base/bind.h"
#include "base/command_line.h"
#include "base/message_loop/message_loop.h"
#include "base/run_loop.h"
#include "base/sequenced_task_runner.h"
#include "mojo/core/embedder/embedder.h"

int main(int argc, char** argv) {
  CHECK(base::CommandLine::Init(argc, argv));
  base::AtExitManager exit_manager_;

  // mojo requires a sequenced context, i.e., a message loop in the thread.
  base::MessageLoop main_loop;
  mojo::core::Init();

  auto monitor = std::make_unique<prelude::RadiationMonitorImpl>();
  auto listener = std::make_unique<prelude::RadiationListenerImpl>();

  // create and bind the mojo interface pointer
  prelude::mojom::RadiationMonitorPtr monitorPtr;
  monitor->BindRequest(mojo::MakeRequest(&monitorPtr));
  prelude::mojom::RadiationListenerPtr listenerPtr;
  listener->BindRequest(mojo::MakeRequest(&listenerPtr));

  // register the listener, note that this is async, note that move is needed
  monitorPtr->RegisterListener(std::move(listenerPtr));

  base::RunLoop loop;
  // listener won't get this message as it's not registered yet
  monitor->Drill();

  base::SequencedTaskRunnerHandle::Get()->PostTask(
      FROM_HERE, base::BindOnce(&prelude::RadiationMonitorImpl::MeltDown,
                                base::Unretained(monitor.get())));

  base::SequencedTaskRunnerHandle::Get()->PostDelayedTask(
      FROM_HERE, base::BindOnce(loop.QuitClosure()),
      base::TimeDelta::FromSeconds(2));

  loop.Run();
  return 0;
}

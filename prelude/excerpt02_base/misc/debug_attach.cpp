// Copyright (c) 2018 The Prelude Authors.
// Use of this source code is governed by MIT license that can be
// found in MIT-LICENSE file.

#include "base/command_line.h"
#include "base/debug/debugger.h"
#include "base/files/file_path.h"
#include "base/stl_util.h" // base::ContainsValue
#include "base/strings/string_split.h"
#include "base/strings/stringprintf.h"
#include "base/strings/utf_string_conversions.h"
#include "build/build_config.h" // defines OS_WIN

#if defined(OS_WIN)
#include <windows.h>
#elif (OS_POSIX)
#include <unistd.h>
#endif

namespace {
// Will wait for a debugger to come to attach to the process.
const char kWaitForMe[] = "wait-for-me";
} // namespace

#if defined(OS_WIN)
int APIENTRY wWinMain(HINSTANCE instance, HINSTANCE, wchar_t *, int) {
  base::CommandLine::Init(0, nullptr);
#else
int main(int argc, char **argv) {
  base::CommandLine::Init(argc, argv);
#endif

  const base::CommandLine *cmd_line = base::CommandLine::ForCurrentProcess();
  if (cmd_line->HasSwitch(kWaitForMe)) {
    std::vector<std::string> apps_to_debug =
        base::SplitString(cmd_line->GetSwitchValueASCII(kWaitForMe), ",",
                          base::TRIM_WHITESPACE, base::SPLIT_WANT_ALL);
    std::string app = "launcher";
    base::FilePath exe_path =
        cmd_line->GetProgram().BaseName().RemoveExtension();
    for (const auto &app_name : apps_to_debug) {
      if (base::FilePath().AppendASCII(app_name) == exe_path) {
        app = app_name;
        break;
      }
    }
    if (apps_to_debug.empty() || base::ContainsValue(apps_to_debug, app)) {

#if defined(OS_WIN)
      base::string16 appw = base::UTF8ToUTF16(app);
      base::string16 message = base::UTF8ToUTF16(
          base::StringPrintf("%s - %ld", app.c_str(), GetCurrentProcessId()));
      MessageBox(NULL, message.c_str(), appw.c_str(), MB_OK | MB_SETFOREGROUND);
#else
      LOG(ERROR) << app << " waiting for GDB. pid: " << getpid();
      base::debug::WaitForDebugger(60, true);
#endif
    }

    printf("starts now.");
  }
  return 0;
}

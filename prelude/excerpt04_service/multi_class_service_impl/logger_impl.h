// Copyright (c) 2018 The Prelude Authors.
// Use of this source code is governed by MIT license that can be
// found in MIT-LICENSE file.

#include "base/macros.h"
#include "mojo/public/cpp/bindings/binding_set.h"
#include "prelude/excerpt04_service/interfaces/logger.mojom.h"

#include <vector>

namespace prelude {
namespace excerpt04 {

class LoggerImpl : public prelude::mojom::Logger {
public:
  LoggerImpl();
  ~LoggerImpl() override;

  void Create(prelude::mojom::LoggerRequest request);

private:

  // Logger
  void Log(const std::string &message) override;
  void GetTail(GetTailCallback callback) override;
  void GetLogCount(GetLogCountCallback callback) override;

  void OnConnectionError();

  mojo::BindingSet<prelude::mojom::Logger> bindings_;
  std::vector<std::string> lines_;

  int log_count{ 0 };

  DISALLOW_COPY_AND_ASSIGN(LoggerImpl);
};

} // namespace excerpt04
} // namespace prelude

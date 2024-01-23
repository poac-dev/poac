#pragma once

#include "../Rustify.hpp"
#include "Global.hpp"

#include <git2/config.h>

namespace git2 {

struct Config : public GlobalState {
private:
  git_config* raw = nullptr;

public:
  Config();
  ~Config();

  explicit Config(git_config* raw);

  Config(const Config&) = delete;
  Config& operator=(const Config&) = delete;
  Config(Config&&) = default;
  Config& operator=(Config&&) = default;

  /// Open the global, XDG and system configuration files.
  Config& openDefault();

  /// Get the value of a string config variable as an owned string.
  String getString(StringRef name);
};

} // end namespace git2

#pragma once

#include "../Rustify.hpp"

#include <git2/config.h>

namespace git2 {

class Config {
  git_config* raw;

public:
  Config();
  ~Config();

  explicit Config(git_config* raw) : raw(raw) {}

  Config(const Config&) = delete;
  Config& operator=(const Config&) = delete;
  Config(Config&&) = default;
  Config& operator=(Config&&) = default;

  /// Open the global, XDG and system configuration files.
  Config& openDefault();

  /// Get the value of a string config variable as an owned string.
  String getString(const StringRef);
};

} // end namespace git2

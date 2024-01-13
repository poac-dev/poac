#pragma once

#include "../Rustify.hpp"
#include "Exception.hpp"
#include "Global.hpp"

#include <git2/config.h>
#include <string>

namespace git2 {

class Config {
  git_config* raw;

public:
  Config() {
    git2::init();
    git2Throw(git_config_new(&this->raw));
  }
  ~Config() {
    git_config_free(this->raw);
  }

  explicit Config(git_config* raw) : raw(raw) {}

  Config(const Config&) = delete;
  Config& operator=(const Config&) = delete;
  Config(Config&&) = default;
  Config& operator=(Config&&) = default;

  /// Get the value of a string config variable as an owned string.
  std::string getString(const StringRef name) {
    git_buf ret = { nullptr, 0, 0 };
    git2Throw(git_config_get_string_buf(&ret, this->raw, name.data()));
    return std::string(ret.ptr, ret.size);
  }
};

} // end namespace git2

#include "Config.hpp"

#include "../Rustify.hpp"
#include "Exception.hpp"

#include <string>

namespace git2 {

Config::Config() {
  git2Throw(git_config_new(&this->raw));
}

Config::Config(git_config* raw) : raw(raw) {}

Config::~Config() {
  git_config_free(this->raw);
}

Config&
Config::openDefault() {
  git2Throw(git_config_open_default(&this->raw));
  return *this;
}

std::string
Config::getString(const StringRef name) {
  git_buf ret = { nullptr, 0, 0 };
  git2Throw(git_config_get_string_buf(&ret, this->raw, name.data()));
  return { ret.ptr, ret.size };
}

} // end namespace git2

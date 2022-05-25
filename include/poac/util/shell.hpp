#ifndef POAC_UTIL_SHELL_HPP_
#define POAC_UTIL_SHELL_HPP_

// std
#include <iostream>
#include <string>

// internal
#include "poac/poac.hpp"

namespace poac::util::shell {

class Cmd {
public:
  inline String
  string() const {
    return cmd;
  }

  Cmd() : cmd() {}
  explicit Cmd(const String& c) : cmd(c) {}

  inline Cmd&
  env(const String& name, const String& value) {
    cmd.insert(0, name + "=" + value + " ");
    return *this;
  }
  inline Cmd&
  stderr_to_stdout() {
    cmd += " 2>&1";
    return *this;
  }
  inline Cmd&
  to_dev_null() {
    cmd += " >/dev/null";
    return *this;
  }
  inline Cmd&
  dump_stdout() {
    cmd += " 1>/dev/null";
    return *this;
  }
  inline Cmd&
  dump_stderr() {
    cmd += " 2>/dev/null";
    return *this;
  }

  // TODO(ken-matsui): Do we need to return result that captures all piped
  //  stderr and stdout? We cannot simultaneously know errors and their
  //  contents.
  Option<String>
  exec() const;

  inline i32
  exec_no_capture() const {
    return std::system(cmd.c_str());
  }

  inline friend std::ostream&
  operator<<(std::ostream& os, const Cmd& c) {
    return (os << c.cmd);
  }

  inline bool
  operator==(const Cmd& rhs) const {
    return this->cmd == rhs.cmd;
  }
  inline bool
  operator==(const String& rhs) const {
    return this->cmd == rhs;
  }

  inline Cmd
  operator&&(const Cmd& rhs) const {
    return Cmd(this->cmd + " && " + rhs.cmd);
  }
  inline Cmd
  operator&&(const String& rhs) const {
    return Cmd(this->cmd + " && " + rhs);
  }

  inline Cmd&
  operator&=(const Cmd& rhs) {
    this->cmd += " && " + rhs.cmd;
    return *this;
  }
  inline Cmd&
  operator&=(const String& rhs) {
    this->cmd += " && " + rhs;
    return *this;
  }

  inline Cmd
  operator||(const Cmd& rhs) const {
    return Cmd(this->cmd + " || " + rhs.cmd);
  }
  inline Cmd
  operator||(const String& rhs) const {
    return Cmd(this->cmd + " || " + rhs);
  }

  inline Cmd&
  operator|=(const Cmd& rhs) {
    this->cmd += " || " + rhs.cmd;
    return *this;
  }
  inline Cmd&
  operator|=(const String& rhs) {
    this->cmd += " || " + rhs;
    return *this;
  }

  inline Cmd
  operator+(const Cmd& rhs) const { // TODO(ken-matsui): should this be "; "?
    return Cmd(this->cmd + " " + rhs.cmd);
  }
  inline Cmd
  operator+(const String& rhs) const {
    return Cmd(this->cmd + " " + rhs);
  }

  inline Cmd&
  operator+=(const Cmd& rhs) {
    this->cmd += " " + rhs.cmd;
    return *this;
  }
  inline Cmd&
  operator+=(const String& rhs) {
    this->cmd += " " + rhs;
    return *this;
  }

private:
  String cmd;
}; // NOLINT(readability/braces)

inline bool
has_command(const String& c) {
  return Cmd("type " + c + " >/dev/null 2>&1").exec().has_value();
}

} // namespace poac::util::shell

#endif // POAC_UTIL_SHELL_HPP_

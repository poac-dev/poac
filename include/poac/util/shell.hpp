#ifndef POAC_UTIL_SHELL_HPP_
#define POAC_UTIL_SHELL_HPP_

// std
#include <array>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <string>

// internal
#include <poac/poac.hpp>

namespace poac::util::shell {

class Cmd {
public:
  String
  string() const {
    return cmd;
  }

  Cmd() : cmd() {}
  explicit Cmd(const String& c) : cmd(c) {}

  Cmd&
  env(const String& name, const String& value) {
    cmd.insert(0, name + "=" + value + " ");
    return *this;
  }
  Cmd&
  stderr_to_stdout() {
    cmd += " 2>&1";
    return *this;
  }
  Cmd&
  to_dev_null() {
    cmd += " >/dev/null";
    return *this;
  }
  Cmd&
  dump_stdout() {
    cmd += " 1>/dev/null";
    return *this;
  }
  Cmd&
  dump_stderr() {
    cmd += " 2>/dev/null";
    return *this;
  }

  // TODO(ken-matsui): Do we need to return result that captures all piped
  //  stderr and stdout? We cannot simultaneously know errors and their
  //  contents.
  Option<String>
  exec() const {
    std::array<char, 128> buffer{};
    String result;

#ifdef _WIN32
    if (FILE* pipe = _popen(cmd.c_str(), "r")) {
#else
    if (FILE* pipe = popen(cmd.c_str(), "r")) {
#endif
      while (std::fgets(buffer.data(), 128, pipe) != nullptr)
        result += buffer.data();
#ifdef _WIN32
      if (_pclose(pipe) != 0) {
#else
      if (pclose(pipe) != 0) {
#endif
        std::cout << result;
        // TODO(ken-matsui): When errored and piped errors to stdout,
        //  I want to return result stored by them.
        return None;
      }
    } else {
      return None;
    }
    return result;
  }

  bool
  exec_ignore() const {
    // EXIT_SUCCESS -> 0 -> false -> true
    // EXIT_FAILURE -> 1 -> true -> false
    return !static_cast<bool>(std::system(cmd.c_str()));
  }

  friend std::ostream&
  operator<<(std::ostream& os, const Cmd& c) {
    return (os << c.cmd);
  }

  bool
  operator==(const Cmd& rhs) const {
    return this->cmd == rhs.cmd;
  }
  bool
  operator==(const String& rhs) const {
    return this->cmd == rhs;
  }

  Cmd
  operator&&(const Cmd& rhs) const {
    return Cmd(this->cmd + " && " + rhs.cmd);
  }
  Cmd
  operator&&(const String& rhs) const {
    return Cmd(this->cmd + " && " + rhs);
  }

  Cmd&
  operator&=(const Cmd& rhs) {
    this->cmd += " && " + rhs.cmd;
    return *this;
  }
  Cmd&
  operator&=(const String& rhs) {
    this->cmd += " && " + rhs;
    return *this;
  }

  Cmd
  operator||(const Cmd& rhs) const {
    return Cmd(this->cmd + " || " + rhs.cmd);
  }
  Cmd
  operator||(const String& rhs) const {
    return Cmd(this->cmd + " || " + rhs);
  }

  Cmd&
  operator|=(const Cmd& rhs) {
    this->cmd += " || " + rhs.cmd;
    return *this;
  }
  Cmd&
  operator|=(const String& rhs) {
    this->cmd += " || " + rhs;
    return *this;
  }

  Cmd
  operator+(const Cmd& rhs) const { // TODO(ken-matsui): should this be "; "?
    return Cmd(this->cmd + " " + rhs.cmd);
  }
  Cmd
  operator+(const String& rhs) const {
    return Cmd(this->cmd + " " + rhs);
  }

  Cmd&
  operator+=(const Cmd& rhs) {
    this->cmd += " " + rhs.cmd;
    return *this;
  }
  Cmd&
  operator+=(const String& rhs) {
    this->cmd += " " + rhs;
    return *this;
  }

private:
  String cmd;
}; // NOLINT(readability/braces)

bool
has_command(const String& c) {
  return Cmd("type " + c + " >/dev/null 2>&1").exec().has_value();
}

} // namespace poac::util::shell

#endif // POAC_UTIL_SHELL_HPP_

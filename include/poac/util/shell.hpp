// This file is header-only and does not depend on "poac/poac.hpp" because of
// being used for UI tests.

#ifndef POAC_UTIL_SHELL_HPP_
#define POAC_UTIL_SHELL_HPP_

// std
#include <array>
#include <iostream>
#include <optional>
#include <string>

namespace poac::util::shell {

class Cmd {
public:
  inline std::string
  string() const {
    return cmd;
  }

  Cmd() : cmd() {}
  explicit Cmd(const std::string& c) : cmd(c) {}

  inline Cmd&
  env(const std::string& name, const std::string& value) {
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
  inline std::optional<std::string>
  exec() const {
    std::array<char, 128> buffer{};
    std::string result;

    if (FILE* pipe = popen(cmd.c_str(), "r")) {
      while (std::fgets(buffer.data(), 128, pipe) != nullptr) {
        result += buffer.data();
      }
      if (const std::int32_t code = pclose(pipe); code != 0) {
        std::cout << result;
        // TODO(ken-matsui): When errored and piped errors to stdout,
        //  I want to return result stored by them.
        return std::nullopt;
      }
    } else {
      return std::nullopt;
    }
    return result;
  }

  inline std::int32_t
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
  operator==(const std::string& rhs) const {
    return this->cmd == rhs;
  }

  inline Cmd
  operator&&(const Cmd& rhs) const {
    return Cmd(this->cmd + " && " + rhs.cmd);
  }
  inline Cmd
  operator&&(const std::string& rhs) const {
    return Cmd(this->cmd + " && " + rhs);
  }

  inline Cmd&
  operator&=(const Cmd& rhs) {
    this->cmd += " && " + rhs.cmd;
    return *this;
  }
  inline Cmd&
  operator&=(const std::string& rhs) {
    this->cmd += " && " + rhs;
    return *this;
  }

  inline Cmd
  operator||(const Cmd& rhs) const {
    return Cmd(this->cmd + " || " + rhs.cmd);
  }
  inline Cmd
  operator||(const std::string& rhs) const {
    return Cmd(this->cmd + " || " + rhs);
  }

  inline Cmd&
  operator|=(const Cmd& rhs) {
    this->cmd += " || " + rhs.cmd;
    return *this;
  }
  inline Cmd&
  operator|=(const std::string& rhs) {
    this->cmd += " || " + rhs;
    return *this;
  }

  inline Cmd
  operator+(const Cmd& rhs) const { // TODO(ken-matsui): should this be "; "?
    return Cmd(this->cmd + " " + rhs.cmd);
  }
  inline Cmd
  operator+(const std::string& rhs) const {
    return Cmd(this->cmd + " " + rhs);
  }

  inline Cmd&
  operator+=(const Cmd& rhs) {
    this->cmd += " " + rhs.cmd;
    return *this;
  }
  inline Cmd&
  operator+=(const std::string& rhs) {
    this->cmd += " " + rhs;
    return *this;
  }

private:
  std::string cmd;
}; // NOLINT(readability/braces)

inline bool
has_command(const std::string& c) {
  return Cmd("type " + c + " >/dev/null 2>&1").exec().has_value();
}

} // namespace poac::util::shell

#endif // POAC_UTIL_SHELL_HPP_

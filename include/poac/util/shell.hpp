// This file is specially header-only and does not depend on "poac/poac.hpp"
// because of being used for UI tests.

#ifndef POAC_UTIL_SHELL_HPP_
#define POAC_UTIL_SHELL_HPP_

// std
#include <array>
#include <iostream>
#include <string>
#include <utility>

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

  struct SimpleResult : std::pair<std::int32_t, std::string> {
    SimpleResult(std::int32_t c, const std::string& r)
        : std::pair<std::int32_t, std::string>({c, r}) {}

    inline bool
    is_ok() const {
      return first == 0;
    }
    inline bool
    is_err() const {
      return !is_ok();
    }
    inline std::string
    output() const {
      return second;
    }
    explicit inline operator bool() const { return is_ok(); }
  };

  inline SimpleResult
  exec() const {
    std::array<char, 128> buffer{};
    std::string result;
    FILE* pipe = popen(cmd.c_str(), "r");
    if (!pipe) {
      return {1, ""};
    }
    while (std::fgets(buffer.data(), 128, pipe) != nullptr) {
      result += buffer.data();
    }

    const std::int32_t code = pclose(pipe);
    if (code != 0) {
      return {code, result};
    }
    return {0, result};
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
  return Cmd("type " + c + " >/dev/null 2>&1").exec().is_ok();
}

} // namespace poac::util::shell

#endif // POAC_UTIL_SHELL_HPP_

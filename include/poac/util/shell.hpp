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
  [[nodiscard]] inline auto string() const -> std::string { return cmd; }

  Cmd() = default; // NOLINT(bugprone-exception-escape)
  explicit Cmd(std::string c) : cmd(std::move(c)) {}

  inline auto env(const std::string& name, const std::string& value) -> Cmd& {
    cmd.insert(0, name + "=" + value + " ");
    return *this;
  }
  inline auto stderr_to_stdout() -> Cmd& {
    cmd += " 2>&1";
    return *this;
  }
  inline auto to_dev_null() -> Cmd& {
    cmd += " >/dev/null";
    return *this;
  }
  inline auto dump_stdout() -> Cmd& {
    cmd += " 1>/dev/null";
    return *this;
  }
  inline auto dump_stderr() -> Cmd& {
    cmd += " 2>/dev/null";
    return *this;
  }

  struct SimpleResult : std::pair<std::int32_t, std::string> {
    SimpleResult(std::int32_t c, const std::string& r)
        : std::pair<std::int32_t, std::string>({c, r}) {}

    [[nodiscard]] inline auto is_ok() const -> bool { return first == 0; }
    [[nodiscard]] inline auto is_err() const -> bool { return !is_ok(); }
    [[nodiscard]] inline auto output() const -> std::string { return second; }
    explicit inline operator bool() const { return is_ok(); }
  };

  [[nodiscard]] inline auto exec() const -> SimpleResult {
    std::array<char, 128> buffer{};
    std::string result;
    FILE* pipe = popen(cmd.c_str(), "r"); // NOLINT(cert-env33-c)
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

  [[nodiscard]] inline auto exec_no_capture() const -> std::int32_t {
    return std::system(cmd.c_str()); // NOLINT(cert-env33-c)
  }

  inline friend auto operator<<(std::ostream& os, const Cmd& c)
      -> std::ostream& {
    return (os << c.cmd);
  }

  inline auto operator==(const Cmd& rhs) const -> bool {
    return this->cmd == rhs.cmd;
  }
  inline auto operator==(const std::string& rhs) const -> bool {
    return this->cmd == rhs;
  }

  inline auto operator&&(const Cmd& rhs) const -> Cmd {
    return Cmd(this->cmd + " && " + rhs.cmd);
  }
  inline auto operator&&(const std::string& rhs) const -> Cmd {
    return Cmd(this->cmd + " && " + rhs);
  }

  inline auto operator&=(const Cmd& rhs) -> Cmd& {
    this->cmd += " && " + rhs.cmd;
    return *this;
  }
  inline auto operator&=(const std::string& rhs) -> Cmd& {
    this->cmd += " && " + rhs;
    return *this;
  }

  inline auto operator||(const Cmd& rhs) const -> Cmd {
    return Cmd(this->cmd + " || " + rhs.cmd);
  }
  inline auto operator||(const std::string& rhs) const -> Cmd {
    return Cmd(this->cmd + " || " + rhs);
  }

  inline auto operator|=(const Cmd& rhs) -> Cmd& {
    this->cmd += " || " + rhs.cmd;
    return *this;
  }
  inline auto operator|=(const std::string& rhs) -> Cmd& {
    this->cmd += " || " + rhs;
    return *this;
  }

  // TODO(ken-matsui): should this be "; "?
  inline auto operator+(const Cmd& rhs) const -> Cmd {
    return Cmd(this->cmd + " " + rhs.cmd);
  }
  inline auto operator+(const std::string& rhs) const -> Cmd {
    return Cmd(this->cmd + " " + rhs);
  }

  inline auto operator+=(const Cmd& rhs) -> Cmd& {
    this->cmd += " " + rhs.cmd;
    return *this;
  }
  inline auto operator+=(const std::string& rhs) -> Cmd& {
    this->cmd += " " + rhs;
    return *this;
  }

private:
  std::string cmd;
}; // NOLINT(readability/braces)

inline auto has_command(const std::string& c) -> bool {
  return Cmd("type " + c + " >/dev/null 2>&1").exec().is_ok();
}

} // namespace poac::util::shell

#endif // POAC_UTIL_SHELL_HPP_

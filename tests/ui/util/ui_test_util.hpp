#ifndef TESTS_UI_UTIL_UI_TEST_UTIL_HPP_
#define TESTS_UI_UTIL_UI_TEST_UTIL_HPP_

// std
#include <filesystem>
#include <fstream>
#include <initializer_list>
#include <iterator>
#include <random>
#include <string>
#include <string_view>

// external
#include <boost/ut.hpp>

// internal
#include "poac/util/shell.hpp"

#ifndef POAC_EXECUTABLE
#  error "POAC_EXECUTABLE should be defined through CMake"
#endif

namespace poac::test {

using namespace boost::ut;
using namespace util::shell;
namespace fs = std::filesystem;

enum class Target {
  Stdout,
  Stderr,
};

inline std::string
random_string() {
  std::string str(
      "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz"
  );
  std::random_device rd;
  std::mt19937 generator(rd());
  std::shuffle(str.begin(), str.end(), generator);
  return str.substr(0, 30);
}

inline fs::path
get_temp_dir() {
  const fs::path temp_dir = fs::temp_directory_path() / random_string();
  fs::create_directories(temp_dir);
  return temp_dir;
}

inline Cmd
move_to_temp(const fs::path& temp_dir) {
  return Cmd("cd " + temp_dir.string());
}

inline void
remove_temp(const fs::path& temp_dir) {
  fs::remove_all(temp_dir);
}

inline Cmd
mk_cmd(const std::string& cmd, const fs::path& temp_dir) {
  return move_to_temp(temp_dir) && POAC_EXECUTABLE + cmd;
}

template <Target target>
Cmd::SimpleResult
dispatch(
    std::initializer_list<std::string_view> args, const fs::path& temp_dir
) {
  std::string cmd_args = " --color never"; // Disable color on tests
  for (std::string_view a : args) {
    cmd_args += " " + std::string(a);
  }

  Cmd cmd = mk_cmd(cmd_args, temp_dir);
  const Cmd::SimpleResult res =
      target == Target::Stdout ? cmd.exec() : cmd.stderr_to_stdout().exec();
  remove_temp(temp_dir);
  return res;
}

inline std::string
readfile_impl(const fs::path& path) {
  std::ifstream input(path);
  if (!input.is_open()) {
    return "";
  }
  return std::string(
      std::istreambuf_iterator<char>(input), std::istreambuf_iterator<char>()
  );
}

// Read an expectation file.
template <Target target>
std::string
readfile(std::string_view name) {
  fs::path input_path = name;
  if (target == Target::Stdout) {
    input_path.replace_extension(".stdout");
  } else {
    input_path.replace_extension(".stderr");
  }
  return readfile_impl(input_path);
}

template <Target target>
void
uitest(
    std::string_view name, std::initializer_list<std::string_view> args,
    const fs::path& temp_dir = get_temp_dir()
) {
  const auto result = dispatch<target>(args, temp_dir);
  if (target == Target::Stdout) {
    expect(result.is_ok());
  } else {
    expect(result.is_err());
  }
  expect(eq(result.output(), readfile<target>(name)));
}

} // namespace poac::test

#define POAC_UITEST_STDOUT(...) \
  poac::test::uitest<poac::test::Target::Stdout>(__FILE__, {__VA_ARGS__})
#define POAC_UITEST_STDERR(...) \
  poac::test::uitest<poac::test::Target::Stderr>(__FILE__, {__VA_ARGS__})

#endif // TESTS_UI_UTIL_UI_TEST_UTIL_HPP_

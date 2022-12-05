#include <boost/ut.hpp>
#include <poac/core/builder/compiler/cxx/clang.hpp>

auto
main() -> int {
  using namespace std::literals::string_literals;
  using namespace boost::ut;

  "test get_compiler_version_impl"_test = [] {
    using poac::core::builder::compiler::cxx::clang::get_compiler_version_impl;
    using poac::util::cfg::compiler;

    auto version = get_compiler_version_impl(
        "Ubuntu clang version 14.0.0-1ubuntu1\n"
        "Target: x86_64-pc-linux-gnu\n"
        "Thread model: posix\n"
        "InstalledDir: /usr/bin\n"
    );
    expect(version.is_ok());
    expect(version.unwrap() == "14.0.0");
  };
}

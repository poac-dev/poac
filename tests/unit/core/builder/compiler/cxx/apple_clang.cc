#include <boost/ut.hpp>
#include <poac/core/builder/compiler/cxx/apple_clang.hpp>

auto
main() -> int {
  using namespace std::literals::string_literals;
  using namespace boost::ut;

  "test get_compiler_version_impl"_test = [] {
    using poac::core::builder::compiler::cxx::apple_clang::
        get_compiler_version_impl;
    using poac::util::cfg::compiler;

    auto version = get_compiler_version_impl(
        "Apple clang version 13.1.6 (clang-1316.0.21.2.5)\n"
        "Target: arm64-apple-darwin21.5.0\n"
        "Thread model: posix\n"
        "InstalledDir: /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin\n"
    );
    expect(version.is_ok());
    expect(version.unwrap() == "13.1.6");
  };
}

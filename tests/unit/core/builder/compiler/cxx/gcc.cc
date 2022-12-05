#include <boost/ut.hpp>
#include <poac/core/builder/compiler/cxx/gcc.hpp>

auto
main() -> int {
  using namespace std::literals::string_literals;
  using namespace boost::ut;

  "test get_compiler_version_impl"_test = [] {
    using poac::core::builder::compiler::cxx::gcc::get_compiler_version_impl;
    using poac::util::cfg::compiler;

    auto version = get_compiler_version_impl(
        "g++-12 (Ubuntu 12-20220319-1ubuntu1) 12.0.1 20220319 (experimental) [master r12-7719-g8ca61ad148f]\n"
        "Error: invalid version found: r12-7719-g8ca61ad148f]\n"
        "Copyright (C) 2022 Free Software Foundation, Inc.\n"
    );
    expect(version.is_ok());
    expect(version.unwrap() == "12.0.1");
  };
}

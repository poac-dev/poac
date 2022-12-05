#include <boost/ut.hpp>
#include <poac/core/builder/compiler/cxx/cxx.hpp>

auto main() -> int {
  using namespace std::literals::string_literals;
  using namespace boost::ut;

  "test get_compiler_ident"_test = [] {
    using poac::core::builder::compiler::cxx::get_compiler_ident;
    using poac::util::cfg::compiler;

    auto ident = get_compiler_ident("clang++", false);
    expect(ident.is_ok());
    expect(ident.unwrap() == compiler::clang) << "clang";
    ident = get_compiler_ident("g++", false);
    expect(ident.is_ok());
    expect(ident.unwrap() == compiler::gcc) << "gcc";

    ident = get_compiler_ident("clang++-14", false);
    expect(ident.is_ok());
    expect(ident.unwrap() == compiler::clang) << "clang with version";
    ident = get_compiler_ident("g++-11", false);
    expect(ident.is_ok());
    expect(ident.unwrap() == compiler::gcc) << "gcc with version";

    ident = get_compiler_ident("/usr/bin/clang++-14", false);
    expect(ident.is_ok());
    expect(ident.unwrap() == compiler::clang) << "clang with path";
    ident = get_compiler_ident("/usr/bin/g++-11", false);
    expect(ident.is_ok());
    expect(ident.unwrap() == compiler::gcc) << "gcc with path";

    ident = get_compiler_ident("unknown", false);
    expect(ident.is_err());
    ident = get_compiler_ident("uclang++wn", false);
    expect(ident.is_ok());
    expect(ident.unwrap() == compiler::clang) << "clang?";
    ident = get_compiler_ident("ung++wn", false);
    expect(ident.is_ok());
    expect(ident.unwrap() == compiler::gcc) << "gcc?";
  };
}

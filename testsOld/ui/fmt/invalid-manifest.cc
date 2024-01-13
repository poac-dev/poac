// std
#include <fstream>

// external
#include <boost/ut.hpp>

// internal
#include "../util/ui_test_util.hpp"

// NOLINTNEXTLINE(bugprone-exception-escape)
auto
main() -> int {
  using namespace boost::ut;
  using namespace poac::util::shell;
  using namespace poac::test;

  if (has_command("clang-format")) {
    cfg<override> = { .tag = { "has-cf" } };
  }

  tag("has-cf") / "invalid manifest"_test = [] {
    const fs::path temp_dir = get_temp_dir();
    std::ofstream ofs(temp_dir / "poac.toml");
    ofs << "[package]";
    ofs.close();
    uitest<Target::Stderr>({ "fmt" }, temp_dir);
  };
}

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

  if (!has_command("clang-format")) {
    cfg<override> = { .tag = { "no-cf" } };
  }

  tag("no-cf")
      / "no clang-format"_test = [] { uitest<Target::Stderr>({ "fmt" }); };
}

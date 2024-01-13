// std
#include <fstream>

// external
#include <boost/ut.hpp>

// internal
#include "../util/ui_test_util.hpp"

auto
main() -> int {
  using namespace boost::ut;
  using namespace poac::util::shell;
  using namespace poac::test;

  "simple case"_test = [] {
    const fs::path temp_dir = get_temp_dir();
    std::ofstream ofs(temp_dir / "poac.toml");
    ofs.close();
    uitest<Target::Stderr>({ "init" }, temp_dir);
  };
}

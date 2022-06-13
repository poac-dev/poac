// external
#include <boost/ut.hpp>

// internal
#include "../util/ui_test_util.hpp"

int
main() {
  using namespace boost::ut;
  using namespace poac::util::shell;
  using namespace poac::test;

  "simple case"_test = [] {
    const fs::path temp_dir = get_temp_dir() / "test";
    fs::create_directories(temp_dir);
    uitest<Target::Stdout>({"init"}, temp_dir, false);
    expect(fs::exists(temp_dir / "poac.toml"));
    remove_temp(temp_dir);
  };
}

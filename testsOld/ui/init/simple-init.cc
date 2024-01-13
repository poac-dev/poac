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
    const fs::path temp_dir = get_temp_dir() / "test";
    fs::create_directories(temp_dir);
    uitest<Target::Stdout>({ "init" }, temp_dir, [](const fs::path& temp_dir) {
      expect(fs::exists(temp_dir / "poac.toml"));
      expect(!fs::is_directory(temp_dir / "poac.toml"));
    });
  };
}

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
    uitest<Target::Stdout>(
        { "create", "hello_world" }, temp_dir,
        [](const fs::path& temp_dir) {
          expect(fs::exists(temp_dir / "hello_world"));
          expect(fs::is_directory(temp_dir / "hello_world"));

          expect(fs::exists(temp_dir / "hello_world" / ".git"));
          expect(fs::is_directory(temp_dir / "hello_world" / ".git"));

          expect(fs::exists(temp_dir / "hello_world" / ".gitignore"));
          expect(!fs::is_directory(temp_dir / "hello_world" / ".gitignore"));

          expect(fs::exists(temp_dir / "hello_world" / "poac.toml"));
          expect(!fs::is_directory(temp_dir / "hello_world" / "poac.toml"));

          expect(fs::exists(temp_dir / "hello_world" / "src"));
          expect(fs::is_directory(temp_dir / "hello_world" / "src"));

          expect(fs::exists(temp_dir / "hello_world" / "src" / "main.cpp"));
          expect(
              !fs::is_directory(temp_dir / "hello_world" / "src" / "main.cpp")
          );
        }
    );
  };
}

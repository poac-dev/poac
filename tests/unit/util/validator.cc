// std
#include <fstream>
#include <tuple> // std::ignore

// external
#include <boost/ut.hpp>

// internal
#include <poac/util/validator.hpp>

int
main() {
  using namespace std::literals::string_literals;
  using namespace boost::ut;

  "test can_create_directory"_test = [] {
    using poac::util::validator::can_create_directory;

    const std::filesystem::path test_dir = "test_dir";
    expect(can_create_directory(test_dir).is_ok());

    std::filesystem::create_directory(test_dir);
    expect(can_create_directory(test_dir).is_ok());

    // NOLINTNEXTLINE(bugprone-unused-raii)
    std::ofstream((test_dir / "test_file").string());
    expect(can_create_directory(test_dir).is_err());

    std::filesystem::remove_all(test_dir);
  };

  "test invalid_characters"_test = [] {
    using poac::util::validator::invalid_characters;

    expect(invalid_characters("na$me").is_err());
    expect(invalid_characters("nam()e").is_err());
    expect(invalid_characters("namße").is_err());

    expect(invalid_characters("poacpm/poac-api").is_ok());
    expect(invalid_characters("poacpm/poac_api").is_ok());
    expect(invalid_characters("poacpm/poac").is_ok());

    expect(invalid_characters("double//slashes").is_err());
    expect(invalid_characters("double--hyphens").is_ok());
    expect(invalid_characters("double__underscores").is_ok());

    expect(invalid_characters("many////////////slashes").is_err());
    expect(invalid_characters("many------------hyphens").is_ok());
    expect(invalid_characters("many________underscores").is_ok());

    expect(invalid_characters("/startWithSlash").is_err());
    expect(invalid_characters("-startWithHyphen").is_err());
    expect(invalid_characters("_startWithUnderscore").is_err());

    expect(invalid_characters("endWithSlash/").is_err());
    expect(invalid_characters("endWithHyphen-").is_err());
    expect(invalid_characters("endWithUnderscore_").is_err());
  };

  "test valid_version"_test = [] {
    using poac::util::validator::valid_version;

    expect(valid_version("v0.1.0").is_err());
    expect(valid_version("0.1.0").is_ok());
  };

  "test valid_athr"_test = [] {
    using poac::util::validator::valid_athr;

    expect(
        valid_athr("Ken Matsui <26405363+ken-matsui@users.noreply.github.com>")
            .is_ok()
    );
    expect(valid_athr("Ken Matsui 26405363+ken-matsui@users.noreply.github.com>"
    )
               .is_err());
    expect(valid_athr("Ken Matsui <26405363+ken-matsui@users.noreply.github.com"
    )
               .is_err());
    expect(valid_athr("Ken Matsui <26405363+ken-matsuiusers.noreply.github.com>"
    )
               .is_err());
    expect(valid_athr("Ken Matsui").is_err());
    expect(valid_athr("").is_err());
  };

  "test valid_authors"_test = [] {
    using poac::util::validator::valid_authors;

    expect(valid_authors(
               {"Ken Matsui <26405363+ken-matsui@users.noreply.github.com>"}
    )
               .is_ok());
    expect(valid_authors({}).is_err());
  };

  "test valid_edition"_test = [] {
    using poac::util::validator::valid_edition;

    expect(valid_edition(1998).is_ok());
    expect(valid_edition(2003).is_ok());
    expect(valid_edition(2011).is_ok());
    expect(valid_edition(2014).is_ok());
    expect(valid_edition(2017).is_ok());
    expect(valid_edition(2020).is_ok());
    expect(valid_edition(2023).is_ok());
    expect(valid_edition(17).is_err());
  };

  "test valid_license"_test = [] {
    using poac::util::validator::valid_license;

    expect(valid_license("AGPL-3.0").is_ok());
    expect(valid_license("GPL-3.0").is_ok());
    expect(valid_license("LGPL-3.0").is_ok());
    expect(valid_license("MPL-2.0").is_ok());
    expect(valid_license("Apache-2.0").is_ok());
    expect(valid_license("MIT").is_ok());
    expect(valid_license("BSL-1.0").is_ok());
    expect(valid_license("Unlicense").is_ok());

    expect(valid_license("Unknown").is_err());
  };

  "test valid_repository"_test = [] {
    using poac::util::validator::valid_repository;

    expect(valid_repository("https://github.com/org/repo/tree/tag").is_ok());
    expect(valid_repository("https://github.com/org/repo/tree/").is_err())
        << "no tag";
    expect(valid_repository("https://github.com/org/repo/tree").is_err())
        << "invalid tree";
    expect(valid_repository("https://github.com/org/repo/").is_err())
        << "no tree";
    expect(valid_repository("https://github.com/org/repo").is_err())
        << "invalid repo";
    expect(valid_repository("https://github.com/org/").is_err()) << "no repo";
    expect(valid_repository("https://github.com/org").is_err())
        << "invalid org";
    expect(valid_repository("https://github.com/").is_err()) << "no org";
    expect(valid_repository("https://github.com").is_err()) << "invalid url";
    expect(valid_repository("https://github.dev/org/repo/tree/tag").is_err())
        << "not github.com";
    expect(valid_repository("http://github.com/org/repo/tree/tag").is_err())
        << "not ssl";

    // Special cases
    expect(valid_repository("https://github.com/org/repo/tree/tag/tag").is_ok())
        << "tag split by slash";
  };

  "test valid_description"_test = [] {
    using poac::util::validator::valid_description;

    expect(valid_description("Poac is a package manager for C++").is_ok());
    expect(valid_description("It's Poac").is_err())
        << "insufficient information";
    expect(valid_description("").is_err()) << "no description";
    expect(valid_description(
               "Poac is a CLI tool to manage various packages written in C++"
               "—we also support C libraries necessary to develop products—"
               "and build packages like Cargo, a package manager for Rust, "
               "does by using Ninja!"
    )
               .is_err()
    ) << "too long";
  };

  "test valid_manifest"_test = [] {
    using poac::util::validator::valid_manifest;
    using namespace toml::literals::toml_literals;

    expect(valid_manifest(u8R"(
[package]
name = "test"
version = "0.1.0"
authors = ["Ken Matsui <26405363+ken-matsui@users.noreply.github.com>"]
edition = 2020
license = "MIT"
repository = "https://github.com/ken-matsui/test/tree/1.0.0"
description = "Manifest Test library"
)"_toml)
               .is_ok()
    ) << "simple case";

    expect(valid_manifest(u8R"(
[package]
name = "test"
version = "0.1.0"
authors = ["Ken Matsui <26405363+ken-matsui@users.noreply.github.com>"]
edition = 2020
license = "MIT"
repository = "https://github.com/ken-matsui/test/tree/1.0.0"
description = "Manifest Test library"

[[bin]]
name = "test"
path = "src/test.rs"
)"_toml)
               .is_ok()
    ) << "extra info but ok";

    expect(throws([]() {
      std::ignore = valid_manifest(
          u8R"(
[package]
name = "test"
version = "0.1.0"
authors = ["Ken Matsui <26405363+ken-matsui@users.noreply.github.com>"]
edition = 2020
license = "MIT"
repository = "https://github.com/ken-matsui/test/tree/1.0.0"
)"_toml
      );
    })) << "missing info";

    expect(throws([]() {
      std::ignore = valid_manifest(
          u8R"(
[package]
name = "test"
version = "0.1.0"
authors = ["Ken Matsui <26405363+ken-matsui@users.noreply.github.com>"]
edition = "2020"
license = "MIT"
repository = "https://github.com/ken-matsui/test/tree/1.0.0"
description = "Manifest Test library"
)"_toml
      );
    })) << "incorrect data type";
  };

  "test valid_profile"_test = [] {
    using poac::util::validator::valid_profile;
    using poac::None;

    {
      auto x = valid_profile(None, None);
      expect(x.is_ok());
      expect(!x.unwrap().has_value());
    }
    {
      auto x = valid_profile(None, false);
      expect(x.is_ok());
      expect(!x.unwrap().has_value());
    }
    {
      auto x = valid_profile(None, true);
      expect(x.is_ok());
      expect(x.unwrap() == "release");
    }
    {
      auto x = valid_profile("debug", None);
      expect(x.is_ok());
      expect(x.unwrap() == "debug");
    }
    {
      auto x = valid_profile("debug", false);
      expect(x.is_ok());
      expect(x.unwrap() == "debug");
    }
    {
      auto x = valid_profile("debug", true);
      expect(x.is_err());
    }
  };
}

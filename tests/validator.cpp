#include <boost/ut.hpp>
#include <fstream>
#include <poac/core/validator.hpp>

int
main() {
  using namespace std::literals::string_literals;
  using namespace boost::ut;

  "test can_crate_directory"_test = [] {
    using poac::core::validator::can_crate_directory;

    const std::filesystem::path test_dir = "test_dir";
    expect(can_crate_directory(test_dir).is_ok());

    std::filesystem::create_directory(test_dir);
    expect(can_crate_directory(test_dir).is_ok());

    std::ofstream((test_dir / "test_file").string());
    expect(can_crate_directory(test_dir).is_err());

    std::filesystem::remove_all(test_dir);
  };

  "test invalid_characters"_test = [] {
    using poac::core::validator::invalid_characters;

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
    using poac::core::validator::valid_version;

    expect(valid_version("v0.1.0").is_err());
    expect(valid_version("0.1.0").is_ok());
  };
}

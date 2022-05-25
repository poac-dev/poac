// external
#include <boost/ut.hpp>

// internal
#include <poac/util/verbosity.hpp>

int
main() {
  using namespace std::literals::string_literals;
  using namespace poac;
  using namespace boost::ut;

  "test is_verbose"_test = [] {
    using util::verbosity::is_verbose;

    spdlog::set_level(spdlog::level::off);
    expect(eq(is_verbose(), false));
    spdlog::set_level(spdlog::level::critical);
    expect(eq(is_verbose(), false));
    spdlog::set_level(spdlog::level::err);
    expect(eq(is_verbose(), false));
    spdlog::set_level(spdlog::level::warn);
    expect(eq(is_verbose(), false));
    spdlog::set_level(spdlog::level::info);
    expect(eq(is_verbose(), false));
    spdlog::set_level(spdlog::level::debug);
    expect(eq(is_verbose(), false));
    spdlog::set_level(spdlog::level::trace);
    expect(eq(is_verbose(), true));
  };

  "test is_quiet"_test = [] {
    using util::verbosity::is_quiet;

    spdlog::set_level(spdlog::level::off);
    expect(eq(is_quiet(), true));
    spdlog::set_level(spdlog::level::critical);
    expect(eq(is_quiet(), false));
    spdlog::set_level(spdlog::level::err);
    expect(eq(is_quiet(), false));
    spdlog::set_level(spdlog::level::warn);
    expect(eq(is_quiet(), false));
    spdlog::set_level(spdlog::level::info);
    expect(eq(is_quiet(), false));
    spdlog::set_level(spdlog::level::debug);
    expect(eq(is_quiet(), false));
    spdlog::set_level(spdlog::level::trace);
    expect(eq(is_quiet(), false));
  };
}

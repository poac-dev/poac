// settings
#define TOML11_COLORIZE_ERROR_MESSAGE

// std
#include <cstdlib>
#include <exception>
#include <optional>

// external
#include <boost/algorithm/string.hpp>
#include <mitama/anyhow/anyhow.hpp>
#include <spdlog/spdlog.h> // NOLINT(build/include_order)
#include <structopt/app.hpp>

// internal
#include <poac/cmd.hpp>
#include <poac/util/termcolor2/literals_extra.hpp>

using namespace termcolor2::color_literals;
namespace subcmd = poac::cmd;
namespace anyhow = mitama::anyhow;

struct Commands {
  /// Use verbose output
  std::optional<bool> verbose = false;
  /// Do not print poac log messages
  std::optional<bool> quiet = false;

  /// Compile a local package and all of its dependencies
  subcmd::build::Options build;

  /// Create a new poac package at <package_name>
  subcmd::create::Options create;

  /// Create a new poac package in an existing directory
  subcmd::init::Options init;

  /// Log in to poac.pm
  subcmd::login::Options login;

  /// Publish a package to poac.pm
  subcmd::publish::Options publish;

  /// Build and run a binary
  subcmd::run::Options run;

  /// Search a package on poac.pm
  subcmd::search::Options search;
};
STRUCTOPT(
    Commands, verbose, quiet, build, create, init, login, publish, run, search
);

inline std::string
colorize_structopt_error(std::string s) {
  boost::replace_all(s, "Error:", "Error:"_bold_red);
  return s;
}

inline std::string
colorize_anyhow_error(std::string s) {
  // `Caused by:` leaves a trailing newline
  if (s.find("Caused by:") != std::string::npos) {
    boost::replace_all(s, "Caused by:", "Caused by:"_yellow);
    boost::replace_last(s, "\n", "");
  }
  return s;
}

inline std::string
colorize_help(std::string s) {
  boost::replace_all(s, "USAGE:", "USAGE:"_yellow);
  boost::replace_all(s, "FLAGS:", "FLAGS:"_yellow);
  boost::replace_all(s, "OPTIONS:", "OPTIONS:"_yellow);
  boost::replace_all(s, "SUBCOMMANDS:", "SUBCOMMANDS:"_yellow);
  boost::replace_last(s, "\n", "");
  return s;
}

[[nodiscard]] anyhow::result<void>
exec(const structopt::app& app, const Commands& args) {
  if (args.build.has_value()) {
    return subcmd::build::exec(args.build);
  } else if (args.create.has_value()) {
    return subcmd::create::exec(args.create);
  } else if (args.init.has_value()) {
    return subcmd::init::exec(args.init);
  } else if (args.login.has_value()) {
    return subcmd::login::exec(args.login);
  } else if (args.publish.has_value()) {
    return subcmd::publish::exec(args.publish);
  } else if (args.run.has_value()) {
    return subcmd::run::exec(args.run);
  } else if (args.search.has_value()) {
    return subcmd::search::exec(args.search);
  } else {
    spdlog::info("{}", colorize_help(app.help()));
    return mitama::success();
  }
}

int
main(const int argc, char* argv[]) {
  spdlog::set_pattern("%v");
  auto app = structopt::app("poac", POAC_VERSION);

  try {
    const auto args = app.parse<Commands>(argc, argv);

    // Global options
    if (args.verbose.value()) {
      spdlog::set_level(spdlog::level::trace);
    } else if (args.quiet.value()) {
      spdlog::set_level(spdlog::level::off);
    }

    // Subcommands
    return exec(app, args)
        .map_err([](const auto& e) {
          spdlog::error(
              "{} {}", "Error:"_bold_red,
              colorize_anyhow_error(fmt::format("{}", e->what()))
          );
        })
        .is_err();
  } catch (const structopt::exception& e) {
    spdlog::error(
        "{}\n\nFor more information, try {}",
        colorize_structopt_error(e.what()), "--help"_green
    );
    return EXIT_FAILURE;
  } catch (const std::exception& e) {
    std::string err = e.what();
    boost::replace_first(err, "[error] ", ""); // for toml11
    spdlog::error("{} {}", "Error:"_bold_red, err);
    return EXIT_FAILURE;
  } catch (...) {
    spdlog::error(
        "{} Unknown error occurred\n\n"
        "Please open an issue with reproducible information at:\n"
        "https://github.com/poacpm/poac/issues",
        "Error:"_bold_red
    );
    return EXIT_FAILURE;
  }
}

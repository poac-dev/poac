// std
#include <cstdlib>
#include <exception>

// external
#include <boost/algorithm/string.hpp>
#include <boost/preprocessor/seq/for_each.hpp>
#include <boost/preprocessor/stringize.hpp>
#include <boost/preprocessor/variadic/to_seq.hpp>
#include <spdlog/sinks/stdout_sinks.h> // NOLINT(build/include_order)
#include <spdlog/spdlog.h>             // NOLINT(build/include_order)
#include <structopt/app.hpp>

// internal
#include <poac/cmd.hpp>
#include <poac/util/lev_distance.hpp>
#include <poac/util/termcolor2/literals_extra.hpp>

#ifndef POAC_VERSION
#  error "POAC_VERSION is not defined"
#endif

using namespace termcolor2::color_literals;
using namespace poac;
namespace anyhow = mitama::anyhow;

struct Commands {
  /// Use verbose output
  Option<bool> verbose = false;
  /// Do not print poac log messages
  Option<bool> quiet = false;
  /// Coloring: auto, always, never
  Option<String> color = "auto";

  /// Compile a local package and all of its dependencies
  cmd::build::Options build;

  /// Create a new poac package at <package_name>
  cmd::create::Options create;

  /// Format source code with clang-format (default `LLVM`)
  cmd::fmt::Options fmt;

  /// Create a new poac package in an existing directory
  cmd::init::Options init;

  /// Run cpplint
  cmd::lint::Options lint;

  /// Log in to poac.pm
  cmd::login::Options login;

  /// Publish a package to poac.pm
  cmd::publish::Options publish;

  /// Build and run a binary
  cmd::run::Options run;

  /// Search a package on poac.pm
  cmd::search::Options search;
};

#define STRINGIFY(r, data, elem) BOOST_PP_STRINGIZE(elem) ,

#define TO_STRINGS(...) \
  BOOST_PP_SEQ_FOR_EACH(STRINGIFY, hoge, BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__))

#define DECL_CMDS(...) \
  inline const Vec<StringRef> command_list { TO_STRINGS(__VA_ARGS__) }

#define structopt(...)                                     \
  STRUCTOPT(Commands, verbose, quiet, color, __VA_ARGS__); \
  DECL_CMDS(__VA_ARGS__)

structopt(build, create, fmt, init, lint, login, publish, run, search);

inline String
colorize_structopt_error(String s) {
  boost::replace_all(s, "Error:", "Error:"_bold_red);
  return s;
}

inline String
colorize_anyhow_error(String s) {
  // `Caused by:` leaves a trailing newline
  if (s.find("Caused by:") != SNone) {
    boost::replace_all(s, "Caused by:", "Caused by:"_yellow);
    boost::replace_last(s, "\n", "");
  }
  return s;
}

inline String
colorize_help(String s) {
  boost::replace_all(s, "USAGE:", "USAGE:"_yellow);
  boost::replace_all(s, "FLAGS:", "FLAGS:"_yellow);
  boost::replace_all(s, "OPTIONS:", "OPTIONS:"_yellow);
  boost::replace_all(s, "SUBCOMMANDS:", "SUBCOMMANDS:"_yellow);
  boost::replace_last(s, "\n", "");
  return s;
}

String
uncolorize_toml11_error(String s) {
  using namespace termcolor2;

  const String colors[] = {
      gray_v(),     red_v(),       green_v(),   yellow_v(),     blue_v(),
      magenta_v(),  cyan_v(),      white_v(),   on_gray_v(),    on_red_v(),
      on_green_v(), on_yellow_v(), on_blue_v(), on_magenta_v(), on_cyan_v(),
      on_white_v(), bold_v(),      dark_v(),    underline_v(),  blink_v(),
      reverse_v(),  concealed_v(), reset_v(),
  };
  for (StringRef c : colors) {
    boost::replace_all(s, c, "");
  }
  return s;
}

using ColorError = poac::Error<
    "argument for --color must be `auto`, `always`, or `never`, but found `{}`",
    StringRef>;

[[nodiscard]] Result<void>
set_color_mode(StringRef color_mode) {
  if (color_mode == "auto") {
    termcolor2::set_color_mode(spdlog::color_mode::automatic);
  } else if (color_mode == "always") {
    termcolor2::set_color_mode(spdlog::color_mode::always);
  } else if (color_mode == "never") {
    termcolor2::set_color_mode(spdlog::color_mode::never);
  } else {
    return Err<ColorError>(color_mode);
  }
  return Ok();
}

[[nodiscard]] Result<void>
exec(const structopt::app& app, const Commands& args) {
  Try(set_color_mode(args.color.value()));

  if (args.build.has_value()) {
    return cmd::build::exec(args.build);
  } else if (args.create.has_value()) {
    return cmd::create::exec(args.create);
  } else if (args.fmt.has_value()) {
    return cmd::fmt::exec(args.fmt);
  } else if (args.init.has_value()) {
    return cmd::init::exec(args.init);
  } else if (args.lint.has_value()) {
    return cmd::lint::exec(args.lint);
  } else if (args.login.has_value()) {
    return cmd::login::exec(args.login);
  } else if (args.publish.has_value()) {
    return cmd::publish::exec(args.publish);
  } else if (args.run.has_value()) {
    return cmd::run::exec(args.run);
  } else if (args.search.has_value()) {
    return cmd::search::exec(args.search);
  } else {
    spdlog::info("{}", colorize_help(app.help()));
    return Ok();
  }
}

int
main(const int argc, char* argv[]) {
  spdlog::set_pattern("%v");
  auto err_logger = spdlog::stderr_logger_st("stderr");

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
        .map_err([err_logger](const auto& e) {
          log::error(
              err_logger, colorize_anyhow_error(format("{}", e->what()))
          );
        })
        .is_err();
  } catch (const structopt::exception& e) {
    if (argc > 1) {
      // try correcting typo
      if (const auto sugg =
              util::lev_distance::find_similar_str(argv[1], command_list)) {
        err_logger->error(
            "{}\n"
            "  --> Did you mean `{}`?\n\n"
            "For more information, try {}",
            colorize_structopt_error(e.what()), sugg.value(), "--help"_green
        );
        return EXIT_FAILURE;
      }
    }
    err_logger->error(
        "{}\n\nFor more information, try {}",
        colorize_structopt_error(e.what()), "--help"_green
    );
    return EXIT_FAILURE;
  } catch (const std::exception& e) {
    if (termcolor2::should_color()) {
      log::error(err_logger, e.what());
    } else {
      log::error(err_logger, uncolorize_toml11_error(e.what()));
    }
    return EXIT_FAILURE;
  } catch (...) {
    err_logger->error(
        "{} Unknown error occurred\n\n"
        "Please open an issue with reproducible information at:\n"
        "https://github.com/poacpm/poac/issues",
        "Error:"_bold_red
    );
    return EXIT_FAILURE;
  }
}

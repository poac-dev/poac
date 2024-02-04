#include "Version.hpp"

#include "../Cli.hpp"
#include "../CurlVersion.hpp"
#include "../Git2/Version.hpp"
#include "../Logger.hpp"
#include "../Rustify.hpp"

#include <cstdlib>
#include <iostream>
#include <span>

#ifndef POAC_PKG_VERSION
#  error "POAC_PKG_VERSION is not defined"
#endif
#ifndef POAC_COMMIT_SHORT_HASH
#  error "POAC_COMMIT_SHORT_HASH is not defined"
#endif
#ifndef POAC_COMMIT_HASH
#  error "POAC_COMMIT_HASH is not defined"
#endif
#ifndef POAC_COMMIT_DATE
#  error "POAC_COMMIT_DATE is not defined"
#endif

const Subcmd VERSION_CMD = //
    Subcmd{ "version" }
        .setDesc("Show version information")
        .setMainFn(versionMain);

static consteval StringRef
checkAvailability(const StringRef str) noexcept {
  return str.empty() ? "unavailable" : str;
}

static constinit const StringRef COMMIT_SHORT_HASH =
    checkAvailability(POAC_COMMIT_SHORT_HASH);
static constinit const StringRef COMMIT_HASH =
    checkAvailability(POAC_COMMIT_HASH);
static constinit const StringRef COMMIT_DATE =
    checkAvailability(POAC_COMMIT_DATE);

static consteval char
firstMonthChar(const StringRef month) noexcept {
  return (month[0] == 'O' || month[0] == 'N' || month[0] == 'D') ? '1' : '0';
}
static consteval char
secondMonthChar(const StringRef month) noexcept {
  if (month[0] == 'J') {
    if (month[1] == 'a') {
      // Jan
      return '1';
    } else if (month[2] == 'n') {
      // Jun
      return '6';
    } else {
      // Jul
      return '7';
    }
  } else if (month[0] == 'F') {
    // Feb
    return '2';
  } else if (month[0] == 'M') {
    if (month[1] == 'a') {
      // Mar
      return '3';
    } else {
      // May
      return '5';
    }
  } else if (month[0] == 'A') {
    if (month[1] == 'p') {
      // Apr
      return '4';
    } else {
      // Aug
      return '8';
    }
  } else if (month[0] == 'S') {
    // Sep
    return '9';
  } else if (month[0] == 'O') {
    // Oct
    return '0';
  } else if (month[0] == 'N') {
    // Nov
    return '1';
  } else if (month[0] == 'D') {
    // Dec
    return '2';
  } else {
    // Error
    return '0';
  }
}

// NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays,modernize-avoid-c-arrays)
static constinit const char COMPILE_DATE[] = {
  // Year
  __DATE__[7], __DATE__[8], __DATE__[9], __DATE__[10],

  '-',

  // Month
  firstMonthChar(__DATE__), secondMonthChar(__DATE__),

  '-',

  // Day
  __DATE__[4] == ' ' ? '0' : __DATE__[4], __DATE__[5],

  '\0'
};

int
versionMain(const std::span<const StringRef> args) noexcept {
  // Parse args
  for (auto itr = args.begin(); itr != args.end(); ++itr) {
    if (const auto res = Cli::handleGlobalOpts(itr, args.end(), "version")) {
      if (res.value() == -1) {
        continue;
      } else {
        return res.value();
      }
    } else {
      return VERSION_CMD.noSuchArg(*itr);
    }
  }

  std::cout << "poac " << POAC_PKG_VERSION;
  if (COMMIT_SHORT_HASH == "unavailable" && COMMIT_DATE == "unavailable") {
    std::cout << '\n';
  } else if (COMMIT_SHORT_HASH == "unavailable") {
    std::cout << " (" << COMMIT_DATE << ")\n";
  } else if (COMMIT_DATE == "unavailable") {
    std::cout << " (" << COMMIT_SHORT_HASH << ")\n";
  } else {
    std::cout << " (" << COMMIT_SHORT_HASH << ' ' << COMMIT_DATE << ")\n";
  }

  if (isVerbose()) {
    std::cout << "release: " << POAC_PKG_VERSION << '\n'
              << "commit-hash: " << COMMIT_HASH << '\n'
              << "commit-date: " << COMMIT_DATE << '\n'
              << "compiler: " << __VERSION__ << '\n'
              << "compile-date: " << COMPILE_DATE << '\n'
              << "libgit2: " << git2::Version() << '\n'
              << "libcurl: " << curl::Version() << '\n';
  }

  return EXIT_SUCCESS;
}

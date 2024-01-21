#include "Version.hpp"

#include "../CurlVersion.hpp"
#include "../Git2/Version.hpp"
#include "../Logger.hpp"
#include "Global.hpp"

#include <cstdlib>
#include <curl/curl.h>
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

static constexpr auto versionCli =
    Subcommand<0>("version").setDesc(versionDesc).setUsage("[OPTIONS]");

void
versionHelp() noexcept {
  versionCli.printHelp();
}

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
firstMonthChar(const char m1) noexcept {
  return (m1 == 'O' || m1 == 'N' || m1 == 'D') ? '1' : '0';
}
static consteval char
secondMonthChar(const char m1, const char m2, const char m3) noexcept {
  if (m1 == 'J') {
    if (m2 == 'a') {
      // Jan
      return '1';
    } else if (m3 == 'n') {
      // Jun
      return '6';
    } else {
      // Jul
      return '7';
    }
  } else if (m1 == 'F') {
    // Feb
    return '2';
  } else if (m1 == 'M') {
    if (m2 == 'a') {
      // Mar
      return '3';
    } else {
      // May
      return '5';
    }
  } else if (m1 == 'A') {
    if (m2 == 'p') {
      // Apr
      return '4';
    } else {
      // Aug
      return '8';
    }
  } else if (m1 == 'S') {
    // Sep
    return '9';
  } else if (m1 == 'O') {
    // Oct
    return '0';
  } else if (m1 == 'N') {
    // Nov
    return '1';
  } else if (m1 == 'D') {
    // Dec
    return '2';
  } else {
    // Error
    return '0';
  }
}

// NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays,modernize-avoid-c-arrays)
static inline constinit const char COMPILE_DATE[] = {
  // Year
  __DATE__[7], __DATE__[8], __DATE__[9], __DATE__[10],

  '-',

  // Month
  firstMonthChar(__DATE__[0]),
  secondMonthChar(__DATE__[0], __DATE__[1], __DATE__[2]),

  '-',

  // Day
  __DATE__[4] == ' ' ? '0' : __DATE__[4], __DATE__[5],

  '\0'
};

int
versionMain(const std::span<const StringRef> args) noexcept {
  // Parse args
  for (usize i = 0; i < args.size(); ++i) {
    const StringRef arg = args[i];
    HANDLE_GLOBAL_OPTS({ { "version" } })

    else {
      return versionCli.noSuchArg(arg);
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

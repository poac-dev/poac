#ifndef POAC_CORE_RESOLVER_HPP_
#define POAC_CORE_RESOLVER_HPP_

// std
#include <fstream>
#include <stdexcept>
#include <string>
#include <tuple>
#include <utility>

// external
#include <boost/algorithm/string.hpp>
#include <boost/range/adaptor/filtered.hpp>
#include <boost/range/adaptor/map.hpp>
#include <spdlog/spdlog.h> // NOLINT(build/include_order)
#include <toml.hpp>

// internal
#include "poac/config.hpp"
#include "poac/core/resolver/resolve.hpp"
#include "poac/core/resolver/sat.hpp"
#include "poac/data/lockfile.hpp"
#include "poac/poac.hpp"
#include "poac/util/archive.hpp"
#include "poac/util/meta.hpp"
#include "poac/util/misc.hpp"
#include "poac/util/net.hpp"
#include "poac/util/sha256.hpp"
#include "poac/util/termcolor2/literals_extra.hpp"
#include "poac/util/termcolor2/termcolor2.hpp"

namespace poac::core::resolver {

using ResolvedDeps = resolve::UniqDeps<resolve::WithDeps>;

using FailedToParseConfig =
    Error<"parsing the value of the `dependencies` key in poac.toml failed">;
using FailedToResolveDeps = Error<"failed to resolve dependencies">;
using FailedToResolveDepsWithCause =
    Error<"failed to resolve dependencies:\n{}", String>;
using FailedToCreateDirs = Error<"failed to create directories:\n{}", String>;
using FailedToRename =
    Error<"failed to rename a downloaded package: `{}: {}`", String, String>;
using FailedToFetch =
    Error<"failed to fetch a package: `{}: {}`", String, String>;
using IncorrectSha256sum = Error<
    "the sha256sum when published did not match one when downloaded.\n"
    "  published: `{}` != downloaded: `{}\n"
    "Since the downloaded package might contain malicious codes, it "
    "was removed from this PC. We highly recommend submitting an "
    "issue on GitHub of the package and stopping using this package:\n"
    "  {}: {}",
    String, String, String, String>;
using Unknown = Error<"unknown error occurred: {}", String>;

inline String
get_install_name(const resolve::Package& package) {
  return boost::replace_first_copy(package.name, "/", "-") + "-" +
         package.version_rq;
}

inline Path
get_extracted_path(const resolve::Package& package) {
  return config::path::extract_dir / get_install_name(package);
}

/// Rename unknown extracted directory to easily access when building.
[[nodiscard]] Result<void>
rename_extracted_directory(
    const resolve::Package& package, StringRef extracted_directory_name
) noexcept {
  const Path temporarily_extracted_path =
      config::path::extract_dir / extracted_directory_name;
  const Path extracted_path = get_extracted_path(package);

  std::error_code ec{};
  fs::rename(temporarily_extracted_path, extracted_path, ec);
  if (ec) {
    return Err<FailedToRename>(package.name, package.version_rq);
  }
  return Ok();
}

Path
get_archive_path(const resolve::Package& package) {
  fs::create_directories(config::path::archive_dir);
  return config::path::archive_dir / (get_install_name(package) + ".tar.gz");
}

String
convert_to_download_link(StringRef repository) {
  // repository should be like =>
  //   https://github.com/boostorg/winapi/tree/boost-1.66.0
  // convert it to =>
  //   https://github.com/boostorg/winapi/archive/boost-1.66.0.tar.gz
  //
  // We should prevent being broken with a package name used `tree` here.
  // Correct:
  //    https://github.com/tree/tree/tree/tree/v0.1.0
  // => https://github.com/tree/tree/archive/tree/v0.1.0

  // The first `tree/` is organization name.
  usize start =
      repository.find('/', 19) + 1; // 19: size of `https://github.com/`
  // The next `tree/` is repository name.
  start = repository.find('/', start) + 1;
  // `start` is now pointing `t` after the repository name.
  // So, find the end of `tree/`.
  const usize end = repository.find('/', start);
  // Retrieve both sides: `https://github.com/tree/tree/`
  StringRef left = repository.substr(0, start);
  // `/tree/v0.1.0`: this side is just a tag.
  // Mostly, we do not include `tree`, but we can.
  StringRef right = repository.substr(end);
  return format("{}archive{}.tar.gz", left, right);
}

[[nodiscard]] Result<std::pair<String, String>, String>
get_download_link(const resolve::Package& package) {
  const auto [repository, sha256sum] =
      Try(util::net::api::repoinfo(package.name, package.version_rq));
  return Ok(std::make_pair(convert_to_download_link(repository), sha256sum));
}

[[nodiscard]] Result<std::pair<Path, String>>
fetch_impl(const resolve::Package& package) noexcept {
  try {
    const auto [download_link, sha256sum] =
        Try(get_download_link(package).map_err(to_anyhow));
    log::debug("downloading from `{}`", download_link);
    const Path archive_path = get_archive_path(package);
    log::debug("writing to `{}`", archive_path.string());

    std::ofstream archive(archive_path);
    const auto [host, target] = util::net::parse_url(download_link);
    const util::net::Requests requests{host};
    std::ignore = requests.get(target, {}, std::move(archive));

    return Ok(std::make_pair(archive_path, sha256sum));
  } catch (const std::exception& e) {
    return Result<std::pair<Path, String>>(Err<Unknown>(e.what()))
        .with_context([&package] {
          return Err<FailedToFetch>(package.name, package.version_rq).get();
        });
  } catch (...) {
    return Err<FailedToFetch>(package.name, package.version_rq);
  }
}

[[nodiscard]] Result<void>
fetch(const resolve::UniqDeps<resolve::WithoutDeps>& deps) noexcept {
  for (const auto& [name, version_rq] : deps) {
    const resolve::Package package{name, version_rq};

    const auto [installed_path, sha256sum] = Try(fetch_impl(package));
    // Check if sha256sum of the downloaded package is the same with one
    // stored in DB.
    if (const String actual_sha256sum = Try(util::sha256::sum(installed_path));
        sha256sum != actual_sha256sum) {
      fs::remove(installed_path);
      return Err<IncorrectSha256sum>(
          sha256sum, actual_sha256sum, package.name, package.version_rq
      );
    }

    const String extracted_directory_name =
        Try(util::archive::extract(installed_path, config::path::extract_dir)
                .map_err(to_anyhow));
    Try(rename_extracted_directory(package, extracted_directory_name));

    log::status(
        "Downloaded"_bold_green, "{} v{}", package.name, package.version_rq
    );
  }
  return Ok();
}

bool
is_not_installed(const resolve::Package& package) noexcept {
  std::error_code ec{};
  bool exists = fs::exists(get_archive_path(package), ec);
  if (ec) {
    return false;
  }
  return !exists;
}

resolve::UniqDeps<resolve::WithoutDeps>
get_not_installed_deps(const ResolvedDeps& deps) noexcept {
  return deps | boost::adaptors::map_keys |
         boost::adaptors::filtered(is_not_installed)
       // ref: https://stackoverflow.com/a/42251976
       | boost::adaptors::transformed([](const resolve::Package& package) {
           return std::make_pair(package.name, package.version_rq);
         }) |
         util::meta::containerized;
}

[[nodiscard]] Result<void>
download_deps(const ResolvedDeps& deps) noexcept {
  const auto not_installed_deps = get_not_installed_deps(deps);
  if (not_installed_deps.empty()) {
    // all resolved packages already have been installed
    return Ok();
  }

  log::status("Downloading"_bold_green, "packages ...");
  try {
    fs::create_directories(config::path::cache_dir);
  } catch (const std::exception& e) {
    return Err<FailedToCreateDirs>(e.what());
  }
  Try(fetch(not_installed_deps));
  return Ok();
}

[[nodiscard]] Result<ResolvedDeps>
do_resolve(const resolve::UniqDeps<resolve::WithoutDeps>& deps) noexcept {
  try {
    const auto duplicate_deps =
        Try(resolve::gather_all_deps(deps).map_err(to_anyhow));
    if (!resolve::duplicate_loose(duplicate_deps)) {
      // When all dependencies are composed of one package and one version,
      // backtrack is not needed. Therefore, the `duplicate_loose`
      // function just needs to check whether the gathered dependencies
      // have multiple packages with the same name or not. If found multiple
      // packages with the same name, then it means this package trying
      // to be built depends on multiple versions of the same package.
      // At the condition (the else clause), gathered dependencies
      // should be in backtrack loop.
      return Ok(ResolvedDeps(duplicate_deps.cbegin(), duplicate_deps.cend()));
    } else {
      return resolve::backtrack_loop(duplicate_deps).map_err(to_anyhow);
    }
  } catch (const std::exception& e) {
    return Err<FailedToResolveDepsWithCause>(e.what());
  } catch (...) {
    return Err<FailedToResolveDeps>();
  }
}

[[nodiscard]] Result<resolve::UniqDeps<resolve::WithoutDeps>>
to_resolvable_deps(const toml::table& dependencies) noexcept {
  try {
    // TOML tables should guarantee uniqueness.
    resolve::UniqDeps<resolve::WithoutDeps> resolvable_deps{};
    for (const auto& d : dependencies) {
      const String name = d.first;
      const String version = toml::get<String>(d.second);
      resolvable_deps.emplace(name, version);
    }
    return Ok(resolvable_deps);
  } catch (...) {
    return Err<FailedToParseConfig>();
  }
}

[[nodiscard]] Result<ResolvedDeps>
get_resolved_deps(const toml::value& manifest) {
  toml::table deps = toml::find<toml::table>(manifest, "dependencies");
  append(deps, toml::find<toml::table>(manifest, "dev-dependencies"));
  const auto resolvable_deps = Try(to_resolvable_deps(deps));
  const auto resolved_deps = Try(do_resolve(resolvable_deps));
  return Ok(resolved_deps);
}

// If lockfile is not outdated, read it.
[[nodiscard]] Result<Option<ResolvedDeps>>
try_to_read_lockfile() {
  if (!data::lockfile::is_outdated(config::path::cur_dir)) {
    return data::lockfile::read(config::path::cur_dir);
  } else {
    return Ok(None);
  }
}

[[nodiscard]] Result<ResolvedDeps>
resolve_deps(const toml::value& manifest) {
  const Option<ResolvedDeps> locked_deps = Try(try_to_read_lockfile());
  if (locked_deps.has_value()) {
    // Lockfile exists and is not outdated.
    return Ok(locked_deps.value());
  } else {
    // Lockfile not found or outdated. Resolve dependencies from manifest file.
    log::status("Resolving"_bold_green, "dependencies ...");
    return get_resolved_deps(manifest);
  }
}

[[nodiscard]] Result<ResolvedDeps>
install_deps(const toml::value& manifest) {
  if (!manifest.contains("dependencies") &&
      !manifest.contains("dev-dependencies")) {
    const ResolvedDeps empty_deps{};
    Try(data::lockfile::generate(empty_deps));
    return Ok(empty_deps);
  }

  const ResolvedDeps resolved_deps = Try(resolve_deps(manifest));
  Try(download_deps(resolved_deps));
  Try(data::lockfile::generate(resolved_deps)); // when lockfile is old

  return Ok(resolved_deps);
}

} // namespace poac::core::resolver

#endif // POAC_CORE_RESOLVER_HPP_

#pragma once

// std
#include <utility>

// external
#include <boost/algorithm/string.hpp>
#include <toml.hpp>

// internal
#include "poac/config.hpp"
#include "poac/core/resolver/resolve.hpp"
#include "poac/core/resolver/types.hpp"
#include "poac/util/format.hpp"
#include "poac/util/log.hpp"
#include "poac/util/net.hpp"
#include "poac/util/result.hpp"
#include "poac/util/rustify.hpp"

namespace poac::core::resolver {

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
using RedefinePredefinedRegistryEntry = Error<
    "Registry entry named `{}` is predefined and can't be overwritten.\n",
    String>;
using DuplicateRegistryEntry =
    Error<"Registry entry named `{}` is duplicated.\n", String>;
using UnknownRegistryType = Error<
    "Registry entry named `{}` has unknown registry type `{}`.\n", String,
    String>;
using Unknown = Error<"unknown error occurred: {}", String>;

inline Fn get_install_name(const resolve::Package& package)->String {
  return boost::replace_first_copy(package.name, "/", "-") + "-"
         + package.dep_info.version_rq;
}

inline Fn get_extracted_path(const resolve::Package& package)->Path {
  return config::default_registry_dir / get_install_name(package);
}

/// Rename unknown extracted directory to easily access when building.
[[nodiscard]] Fn rename_extracted_directory(
    const resolve::Package& package, StringRef extracted_directory_name
)
    ->Result<void>;

inline Fn get_archive_path(const resolve::Package& package)->Path {
  fs::create_directories(config::default_registry_dir);
  return config::default_registry_dir / (get_install_name(package) + ".tar.gz");
}

inline Fn convert_to_download_link(StringRef repository)->String {
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
  const StringRef left = repository.substr(0, start);
  // `/tree/v0.1.0`: this side is just a tag.
  // Mostly, we do not include `tree`, but we can.
  StringRef right = repository.substr(end);
  return format("{}archive{}.tar.gz", left, right);
}

[[nodiscard]] inline Fn get_download_link(const resolve::Package& package)
    ->Result<std::pair<String, String>, String> {
  Let[repository, sha256sum] =
      Try(util::net::api::repoinfo(package.name, package.dep_info.version_rq));
  return Ok(std::make_pair(convert_to_download_link(repository), sha256sum));
}

[[nodiscard]] Fn fetch_impl(const resolve::Package& package) noexcept
    -> Result<std::pair<Path, String>>;

[[nodiscard]] Fn fetch(const resolve::UniqDeps<resolve::WithoutDeps>& deps)
    ->Result<void>;

Fn is_not_installed(const resolve::Package& package)->bool;

Fn get_not_installed_deps(const ResolvedDeps& deps)
    ->resolve::UniqDeps<resolve::WithoutDeps>;

[[nodiscard]] Fn download_deps(const ResolvedDeps& deps)->Result<void>;

[[nodiscard]] Fn do_resolve(const resolve::UniqDeps<resolve::WithoutDeps>& deps
) noexcept -> Result<ResolvedDeps>;

[[nodiscard]] Fn to_resolvable_deps(const toml::table& dependencies) noexcept
    -> Result<resolve::UniqDeps<resolve::WithoutDeps>>;

[[nodiscard]] Fn get_resolved_deps(const toml::value& manifest)
    ->Result<ResolvedDeps>;

// If lockfile is not outdated, read it.
[[nodiscard]] Fn try_to_read_lockfile()->Result<Option<ResolvedDeps>>;

[[nodiscard]] Fn resolve_deps(const toml::value& manifest)
    ->Result<ResolvedDeps>;

[[nodiscard]] Fn install_deps(const toml::value& manifest)
    ->Result<ResolvedDeps>;

} // namespace poac::core::resolver

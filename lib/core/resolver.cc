// std
#include <fstream>
#include <stdexcept>

// external
#include <boost/range/adaptor/filtered.hpp>
#include <boost/range/adaptor/map.hpp>

// internal
#include "poac/core/resolver.hpp"
#include "poac/data/lockfile.hpp"
#include "poac/util/archive.hpp"
#include "poac/util/meta.hpp"
#include "poac/util/sha256.hpp"

namespace poac::core::resolver {

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

using resolve::UniqDeps;
using resolve::WithoutDeps;

[[nodiscard]] Result<void>
fetch(const UniqDeps<WithoutDeps>& deps) noexcept {
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

    log::status("Downloaded", "{} v{}", package.name, package.version_rq);
  }
  return Ok();
}

bool
is_not_installed(const resolve::Package& package) {
  return !fs::exists(get_archive_path(package));
}

UniqDeps<WithoutDeps>
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
  const UniqDeps<WithoutDeps> not_installed_deps = get_not_installed_deps(deps);
  if (not_installed_deps.empty()) {
    // all resolved packages already have been installed
    return Ok();
  }

  log::status("Downloading", "packages ...");
  try {
    fs::create_directories(config::path::cache_dir);
  } catch (const std::exception& e) {
    return Err<FailedToCreateDirs>(e.what());
  }
  Try(fetch(not_installed_deps));
  return Ok();
}

[[nodiscard]] Result<ResolvedDeps>
do_resolve(const UniqDeps<WithoutDeps>& deps) noexcept {
  try {
    const resolve::DupDeps<resolve::WithDeps> duplicate_deps =
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

[[nodiscard]] Result<UniqDeps<WithoutDeps>>
to_resolvable_deps(const HashMap<String, String>& dependencies) noexcept {
  try {
    // TOML tables should guarantee uniqueness.
    UniqDeps<WithoutDeps> resolvable_deps{};
    for (const auto& [name, version] : dependencies) {
      resolvable_deps.emplace(name, version);
    }
    return Ok(resolvable_deps);
  } catch (...) {
    return Err<FailedToParseConfig>();
  }
}

[[nodiscard]] Result<ResolvedDeps>
get_resolved_deps(const toml::value& manifest) {
  auto deps = toml::find<HashMap<String, String>>(manifest, "dependencies");
  if (manifest.contains("dev-dependencies")) {
    append(
        deps, toml::find<HashMap<String, String>>(manifest, "dev-dependencies")
    );
  }
  const UniqDeps<WithoutDeps> resolvable_deps = Try(to_resolvable_deps(deps));
  const ResolvedDeps resolved_deps = Try(do_resolve(resolvable_deps));
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
    log::status("Resolving", "dependencies ...");
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

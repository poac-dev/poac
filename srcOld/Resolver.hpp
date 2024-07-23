module;

// std
#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>

// external
#include <boost/algorithm/string.hpp>
#include <boost/predef.h> // NOLINT(build/include_order)
#include <boost/range/adaptor/filtered.hpp>
#include <boost/range/adaptor/map.hpp>
#include <spdlog/spdlog.h> // NOLINT(build/include_order)
#include <toml.hpp>

// internal
#include "../util/result-macros.hpp"

export module poac.core.resolver;

import poac.config;
import poac.core.resolver.resolve;
import poac.core.resolver.types;
import poac.util.format;
import poac.util.log;
import poac.util.net;
import poac.util.result;
import poac.util.rustify;
import poac.core.resolver.registry;
import poac.data.manifest;
import poac.data.lockfile;
import poac.util.archive;
import poac.util.file;
import poac.util.meta;
import poac.util.registry.conan.v1.resolver;
import poac.util.sha256;
import poac.util.shell;

namespace poac::core::resolver {

using namespace std::literals::string_view_literals;

inline auto
get_install_name(const resolve::Package& package) -> std::string {
  return boost::replace_first_copy(package.name, "/", "-") + "-"
         + package.dep_info.version_rq;
}

export inline auto
get_extracted_path(const resolve::Package& package) -> fs::path {
  return config::default_registry_dir / get_install_name(package);
}

/// Rename unknown extracted directory to easily access when building.
[[nodiscard]] auto
rename_extracted_directory(
    const resolve::Package& package, std::string_view extracted_directory_name
) -> Result<void> {
  const fs::path temporarily_extracted_path =
      config::default_registry_dir / extracted_directory_name;
  const fs::path extracted_path = get_extracted_path(package);

  std::error_code ec{};
  fs::rename(temporarily_extracted_path, extracted_path, ec);
  if (ec) {
    return Err<FailedToRename>(package.name, package.dep_info.version_rq);
  }
  return Ok();
}

inline auto
get_archive_path(const resolve::Package& package) -> fs::path {
  fs::create_directories(config::default_registry_dir);
  return config::default_registry_dir / (get_install_name(package) + ".tar.gz");
}

inline auto
convert_to_download_link(std::string_view repository) -> std::string {
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
  const std::string_view left = repository.substr(0, start);
  // `/tree/v0.1.0`: this side is just a tag.
  // Mostly, we do not include `tree`, but we can.
  std::string_view right = repository.substr(end);
  return format("{}archive{}.tar.gz", left, right);
}

[[nodiscard]] inline auto
get_download_link(const resolve::Package& package
) -> Result<std::pair<std::string, std::string>, std::string> {
  const auto [repository, sha256sum] =
      Try(util::net::api::repoinfo(package.name, package.dep_info.version_rq));
  return Ok(std::make_pair(convert_to_download_link(repository), sha256sum));
}

[[nodiscard]] auto
fetch_impl(const resolve::Package& package
) noexcept -> Result<std::pair<fs::path, std::string>> {
  try {
    const auto [download_link, sha256sum] =
        Try(get_download_link(package).map_err(to_anyhow));
    log::debug("downloading from `{}`", download_link);
    const fs::path archive_path = get_archive_path(package);
    log::debug("writing to `{}`", archive_path.string());

    std::ofstream archive(archive_path);
    const auto [host, target] = util::net::parse_url(download_link);
    const util::net::Requests requests{ host };
    std::ignore = requests.get(target, {}, std::move(archive));

    return Ok(std::make_pair(archive_path, sha256sum));
  } catch (const std::exception& e) {
    return Result<std::pair<fs::path, std::string>>(Err<Unknown>(e.what()))
        .with_context([&package] {
          return Err<FailedToFetch>(package.name, package.dep_info.version_rq)
              .get();
        });
  } catch (...) {
    return Err<FailedToFetch>(package.name, package.dep_info.version_rq);
  }
}

using resolve::UniqDeps;
using resolve::WithoutDeps;

[[nodiscard]] auto
fetch(const resolve::UniqDeps<resolve::WithoutDeps>& deps) -> Result<void> {
  std::vector<resolve::Package> conan_packages;

  for (const auto& [name, dep_info] : deps) {
    const resolve::Package package{ name, dep_info };

    if (poac::util::registry::conan::v1::resolver::is_conan(package)) {
      conan_packages.push_back(package);
      continue;
    }

    const auto [installed_path, sha256sum] = Try(fetch_impl(package));
    // Check if sha256sum of the downloaded package is the same with one
    // stored in DB.
    if (const std::string actual_sha256sum =
            Try(util::sha256::sum(installed_path));
        sha256sum != actual_sha256sum) {
      fs::remove(installed_path);
      return Err<IncorrectSha256sum>(
          sha256sum, actual_sha256sum, package.name, package.dep_info.version_rq
      );
    }

    const std::string extracted_directory_name =
        Try(util::archive::extract(installed_path, config::default_registry_dir)
                .map_err(to_anyhow));
    Try(rename_extracted_directory(package, extracted_directory_name));

    log::status(
        "Downloaded", "{} v{}", package.name, package.dep_info.version_rq
    );
  }

  if (!conan_packages.empty()) {
    const auto toml_last_modified =
        data::manifest::poac_toml_last_modified(config::cwd);
    const auto conan_lockfile = config::conan_deps_dir / "conan.lock";
    if (!fs::exists(conan_lockfile)
        || fs::last_write_time(conan_lockfile) < toml_last_modified) {
      Try(poac::util::registry::conan::v1::resolver::fetch_conan_packages(
          conan_packages
      ));
    }
  }

  return Ok();
}

auto
is_not_installed(const resolve::Package& package) -> bool {
  return !fs::exists(get_archive_path(package));
}

auto
get_not_installed_deps(const ResolvedDeps& deps
) -> resolve::UniqDeps<resolve::WithoutDeps> {
  return deps | boost::adaptors::map_keys
         | boost::adaptors::filtered(is_not_installed)
         // ref: https://stackoverflow.com/a/42251976
         | boost::adaptors::transformed([](const resolve::Package& package) {
             return std::make_pair(package.name, package.dep_info);
           })
         | util::meta::CONTAINERIZED;
}

[[nodiscard]] auto
download_deps(const ResolvedDeps& deps) -> Result<void> {
  const UniqDeps<WithoutDeps> not_installed_deps = get_not_installed_deps(deps);
  if (not_installed_deps.empty()) {
    // all resolved packages already have been installed
    return Ok();
  }

  log::status("Downloading", "packages ...");
  try {
    fs::create_directories(config::cache_dir);
  } catch (const std::exception& e) {
    return Err<FailedToCreateDirs>(e.what());
  }
  Try(fetch(not_installed_deps));
  return Ok();
}

[[nodiscard]] auto
do_resolve(const resolve::UniqDeps<resolve::WithoutDeps>& deps
) noexcept -> Result<ResolvedDeps> {
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

[[nodiscard]] auto
to_resolvable_deps(
    const toml::table& dependencies, const registry::Registries& registries
) noexcept -> Result<resolve::UniqDeps<resolve::WithoutDeps>> {
  try {
    // TOML tables should guarantee uniqueness.
    UniqDeps<WithoutDeps> resolvable_deps{};
    for (const auto& [name, table] : dependencies) {
      poac::core::resolver::resolve::DependencyInfo info = { .index = "poac",
                                                             .type = "poac" };
      if (table.is_table()) {
        const toml::table& entries = table.as_table();
        for (const auto& [n, v] : entries) {
          if (n == "version"sv) {
            info.version_rq = v.as_string();
          } else if (n == "registry"sv) {
            const auto& entry = registries.at(v.as_string());
            info.index = entry.index;
            info.type = entry.type;
          } else {
            return Err<FailedToParseConfig>();
          }
        }
      } else {
        info.version_rq = table.as_string();
      }
      resolvable_deps.emplace(name, std::move(info));
    }
    return Ok(resolvable_deps);
  } catch (...) {
    return Err<FailedToParseConfig>();
  }
}

[[nodiscard]] auto
get_registries(const toml::value& manifest) -> Result<registry::Registries> {
  registry::Registries regs = { { "poac", { .index = "poac", .type = "poac" } },
                                { "conan-v1",
                                  { .index = "conan", .type = "conan-v1" } } };
  if (!manifest.contains("registries")) {
    return Ok(regs);
  }
  const auto& regs_table = toml::find(manifest, "registries").as_table();
  for (const auto& [name, table] : regs_table) {
    if (regs.contains(name)) {
      if (name == "poac" || name == "conan-v1") {
        return Err<RedefinePredefinedRegistryEntry>(name);
      } else {
        return Err<DuplicateRegistryEntry>(name);
      }
    }
    std::string index = toml::find<std::string>(table, "index");
    std::string type = toml::find<std::string>(table, "type");
    if (type != "poac" && type != "conan-v1") {
      return Err<UnknownRegistryType>(name, type);
    }
    regs.emplace(
        name,
        registry::Registry{ .index = std::move(index), .type = std::move(type) }
    );
  }
  return Ok(regs);
}

[[nodiscard]] auto
get_resolved_deps(const toml::value& manifest) -> Result<ResolvedDeps> {
  const registry::Registries registries = Try(get_registries(manifest));

  auto deps = toml::find(manifest, "dependencies").as_table();
  if (manifest.contains("dev-dependencies")) {
    append(deps, toml::find(manifest, "dev-dependencies").as_table());
  }
  const UniqDeps<WithoutDeps> resolvable_deps =
      Try(to_resolvable_deps(deps, registries));
  const ResolvedDeps resolved_deps = Try(do_resolve(resolvable_deps));
  return Ok(resolved_deps);
}

// If lockfile is not outdated, read it.
[[nodiscard]] auto
try_to_read_lockfile() -> Result<std::optional<ResolvedDeps>> {
  if (!data::lockfile::is_outdated(config::cwd)) {
    return data::lockfile::read(config::cwd);
  } else {
    return Ok(std::nullopt);
  }
}

[[nodiscard]] auto
resolve_deps(const toml::value& manifest) -> Result<ResolvedDeps> {
  const std::optional<ResolvedDeps> locked_deps = Try(try_to_read_lockfile());
  if (locked_deps.has_value()) {
    // Lockfile exists and is not outdated.
    return Ok(locked_deps.value());
  } else {
    // Lockfile not found or outdated. Resolve dependencies from manifest file.
    log::status("Resolving", "dependencies ...");
    return get_resolved_deps(manifest);
  }
}

export [[nodiscard]] auto
install_deps(const toml::value& manifest) -> Result<ResolvedDeps> {
  if (!manifest.contains("dependencies")
      && !manifest.contains("dev-dependencies")) {
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

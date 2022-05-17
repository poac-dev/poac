#ifndef POAC_CORE_RESOLVER_HPP
#define POAC_CORE_RESOLVER_HPP

// std
#include <fstream>
#include <string>
#include <stdexcept>
#include <utility>
#include <unordered_map>
#include <vector>
#include <optional>

// external
#include <boost/algorithm/string.hpp>
#include <boost/range/adaptor/filtered.hpp>
#include <boost/range/adaptor/map.hpp>
#include <fmt/core.h>
#include <mitama/result/result.hpp>
#include <mitama/anyhow/anyhow.hpp>
#include <mitama/thiserror/thiserror.hpp>
#include <spdlog/spdlog.h>
#include <toml.hpp>

// internal
#include <poac/core/resolver/resolve.hpp>
#include <poac/core/resolver/sat.hpp>
#include <poac/data/lockfile.hpp>
#include <poac/util/archive.hpp>
#include <poac/util/termcolor2/termcolor2.hpp>
#include <poac/util/termcolor2/literals_extra.hpp>
#include <poac/util/meta.hpp>
#include <poac/util/misc.hpp>
#include <poac/util/net.hpp>
#include <poac/config.hpp>

namespace poac::core::resolver {
    namespace anyhow = mitama::anyhow;
    namespace thiserror = mitama::thiserror;

    using resolved_deps_t = resolve::unique_deps_t<resolve::with_deps>;

    class Error {
        template <thiserror::fixed_string S, class ...T>
        using error = thiserror::error<S, T...>;

    public:
        using FailedToParseConfig =
            error<"parsing the value of the `dependencies` key in poac.toml failed">;

        using FailedToResolveDeps =
            error<"failed to resolve dependencies">;

        using FailedToResolveDepsWithCause =
            error<"failed to resolve dependencies:\n{0}", std::string>;

        using FailedToCreateDirs =
            error<"failed to create directories:\n{0}", std::string>;

        using FailedToRename =
            error<"failed to rename a downloaded package: `{0}: {1}`", std::string, std::string>;

        using FailedToFetch =
            error<"failed to fetch a package: `{0}: {1}`", std::string, std::string>;

        using Unknown =
            error<"unknown error occurred: {0}", std::string>;
    };

    inline std::string
    get_install_name(const resolve::package_t& package) {
        return boost::replace_first_copy(
            resolve::get_name(package), "/", "-"
        ) + "-" + resolve::get_version(package);
    }

    inline std::filesystem::path
    get_extracted_path(const resolve::package_t& package) {
        return config::path::extract_dir / get_install_name(package);
    }

    /// Rename unknown extracted directory to easily access when building.
    [[nodiscard]] anyhow::result<void>
    rename_extracted_directory(
        const resolve::package_t& package,
        std::string_view extracted_directory_name) noexcept
    {
        const std::filesystem::path temporarily_extracted_path =
            config::path::extract_dir / extracted_directory_name;
        const std::filesystem::path extracted_path = get_extracted_path(package);

        std::error_code ec{};
        std::filesystem::rename(temporarily_extracted_path, extracted_path, ec);
        if (ec) {
            return anyhow::failure<Error::FailedToRename>(package.first, package.second);
        }
        return mitama::success();
    }

    std::filesystem::path
    get_archive_path(const resolve::package_t& package) {
        std::filesystem::create_directories(config::path::archive_dir);
        return config::path::archive_dir / (get_install_name(package) + ".tar.gz");
    }

    std::string
    convert_to_download_link(std::string repository) {
        // repository is like =>
        //   https://github.com/boostorg/winapi/tree/boost-1.66.0
        // convert it to =>
        //   https://github.com/boostorg/winapi/archive/boost-1.66.0.tar.gz
        boost::replace_all(repository, "tree", "archive");
        return repository + ".tar.gz";
    }

    [[nodiscard]] mitama::result<std::string, std::string>
    get_download_link(const resolve::package_t& package) {
        const std::string repository =
            MITAMA_TRY(util::net::api::repository(
                resolve::get_name(package), resolve::get_version(package)
            ));
        return mitama::success(convert_to_download_link(repository));
    }

    [[nodiscard]] anyhow::result<std::filesystem::path>
    fetch_impl(const resolve::package_t& package) noexcept {
        try {
            const std::string download_link = MITAMA_TRY(
                get_download_link(package)
                    .map_err([](const std::string& e){ return anyhow::anyhow(e); })
            );
            spdlog::debug("downloading from `{}`", download_link);
            const std::filesystem::path archive_path = get_archive_path(package);
            spdlog::debug("writing to `{}`", archive_path.string());

            std::ofstream archive(archive_path);
            const auto [host, target] = util::net::parse_url(download_link);
            const util::net::requests requests{ host };
            static_cast<void>(requests.get(target, {}, std::move(archive)));

            return mitama::success(archive_path);
        } catch (const std::exception& e) {
            return anyhow::result<std::filesystem::path>(anyhow::failure<Error::Unknown>(e.what()))
                .with_context([&name=package.first, &ver=package.second] {
                    return anyhow::failure<Error::FailedToFetch>(name, ver).get();
                });
        } catch (...) {
            return anyhow::failure<Error::FailedToFetch>(package.first, package.second);
        }
    }

    [[nodiscard]] anyhow::result<void>
    fetch(const resolve::unique_deps_t<resolve::without_deps>& deps) noexcept {
        for (const auto& package : deps) {
            const std::filesystem::path installed_path = MITAMA_TRY(fetch_impl(package));
            const std::string extracted_directory_name = MITAMA_TRY(
                util::archive::extract(installed_path, config::path::extract_dir)
                    .map_err([](const std::string& e){ return anyhow::anyhow(e); })
            );
            MITAMA_TRY(
                rename_extracted_directory(package, extracted_directory_name)
            );

            using termcolor2::color_literals::operator""_bold_green;
            spdlog::info(
                "{:>25} {} v{}", "Downloaded"_bold_green,
                resolve::get_name(package),
                resolve::get_version(package)
            );
        }
        return mitama::success();
    }

    bool
    is_not_installed(const resolve::package_t& package) noexcept {
        std::error_code ec{};
        bool exists = std::filesystem::exists(get_archive_path(package), ec);
        if (ec) {
            return false;
        }
        return !exists;
    }

    resolve::unique_deps_t<resolve::without_deps>
    get_not_installed_deps(const resolved_deps_t& deps) noexcept {
        return
            deps
            | boost::adaptors::map_keys
            | boost::adaptors::filtered(is_not_installed)
            // ref: https://stackoverflow.com/a/42251976
            | boost::adaptors::transformed(
                [](const resolve::package_t& package){
                    return std::make_pair(
                        resolve::get_name(package),
                        resolve::get_version(package)
                    );
                })
            | util::meta::containerized;
    }

    [[nodiscard]] anyhow::result<void>
    download_deps(const resolved_deps_t& deps) noexcept {
        const auto not_installed_deps = get_not_installed_deps(deps);
        if (not_installed_deps.empty()) {
            // all resolved packages already have been installed
            return mitama::success();
        }

        using termcolor2::color_literals::operator""_bold_green;
        spdlog::info("{:>25} packages ...", "Downloading"_bold_green);
        try {
            std::filesystem::create_directories(config::path::cache_dir);
        } catch (const std::exception& e) {
            return anyhow::failure<Error::FailedToCreateDirs>(e.what());
        }
        return fetch(not_installed_deps);
    }

    [[nodiscard]] anyhow::result<resolved_deps_t>
    do_resolve(const resolve::unique_deps_t<resolve::without_deps>& deps) noexcept {
        try {
            const auto duplicate_deps = MITAMA_TRY(
                resolve::gather_all_deps(deps)
                    .map_err([](const std::string& e){ return anyhow::anyhow(e); })
            );
            if (!resolve::duplicate_loose(duplicate_deps)) {
                // When all dependencies are composed of one package and one version,
                // a backtrack is not needed. Therefore, the duplicate_loose
                // function just needs to check whether the gathered dependencies
                // have multiple packages with the same name. If found multiple
                // packages with the same name, then it means this package trying
                // building depends on multiple versions of the same package.
                // At the condition (the else clause), gathered dependencies
                // should be in the backtrack loop.
                return mitama::success(
                    resolved_deps_t(
                        duplicate_deps.cbegin(), duplicate_deps.cend()
                    ));
            } else {
                return resolve::backtrack_loop(duplicate_deps)
                    .map_err([](const std::string& e){ return anyhow::anyhow(e); });
            }
        } catch (const std::exception& e) {
            return anyhow::failure<Error::FailedToResolveDepsWithCause>(e.what());
        } catch (...) {
            return anyhow::failure<Error::FailedToResolveDeps>();
        }
    }

    [[nodiscard]] anyhow::result<resolve::unique_deps_t<resolve::without_deps>>
    to_resolvable_deps(const toml::value& deps) noexcept {
        try {
            resolve::unique_deps_t<resolve::without_deps> resolvable_deps{};
            for (const auto& dep : toml::get<toml::table>(deps)) {
                const std::string version = toml::get<std::string>(dep.second);
                resolvable_deps.emplace(dep.first, version);
            }
            return mitama::success(resolvable_deps);
        } catch (...) {
            return anyhow::failure<Error::FailedToParseConfig>();
        }
    }

    [[nodiscard]] anyhow::result<std::optional<resolved_deps_t>>
    try_to_read_lockfile(const toml::value& config) {
        if (data::lockfile::is_outdated(config::path::current)) {
            const toml::value deps = toml::get<toml::table>(config).at("dependencies");
            const auto resolvable_deps = MITAMA_TRY(to_resolvable_deps(deps));
            const auto resolved_deps = MITAMA_TRY(do_resolve(resolvable_deps));
            return mitama::success(resolved_deps);
        } else {
            return data::lockfile::read(config::path::current);
        }
    }

    [[nodiscard]] anyhow::result<resolved_deps_t>
    get_resolved_deps(const toml::value& config) {
        const auto resolved_deps = MITAMA_TRY(try_to_read_lockfile(config));
        if (resolved_deps.has_value()) {
            return mitama::success(resolved_deps.value());
        } else {
            // Resolve dependencies from manifest file.
            const toml::value deps = toml::get<toml::table>(config).at("dependencies");
            const auto resolvable_deps = MITAMA_TRY(to_resolvable_deps(deps));
            return do_resolve(resolvable_deps);
        }
    }

    [[nodiscard]] anyhow::result<resolved_deps_t>
    install_deps(const toml::value& manifest) {
        if (!manifest.contains("dependencies")) {
            const auto empty_deps = resolved_deps_t{};
            MITAMA_TRY(data::lockfile::generate(empty_deps));
            return mitama::success(empty_deps);
        }

        const auto resolved_deps = MITAMA_TRY(get_resolved_deps(manifest));
        MITAMA_TRY(download_deps(resolved_deps));
        MITAMA_TRY(data::lockfile::generate(resolved_deps)); // when lockfile is old

        return mitama::success(resolved_deps);
    }
}

#endif // POAC_CORE_RESOLVER_HPP

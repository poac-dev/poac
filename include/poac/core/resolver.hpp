#ifndef POAC_CORE_RESOLVER_HPP
#define POAC_CORE_RESOLVER_HPP

// std
#include <fstream>
#include <string>
#include <stdexcept>
#include <utility>
#include <unordered_map>
#include <vector>

// external
#include <boost/algorithm/string.hpp>
#include <boost/range/adaptor/filtered.hpp>
#include <boost/range/adaptor/map.hpp>
#include <fmt/core.h>
#include <mitama/result/result.hpp>
#include <plog/Log.h>
#include <toml.hpp>

// internal
#include <poac/core/resolver/resolve.hpp>
#include <poac/core/resolver/sat.hpp>
#include <poac/util/archive.hpp>
#include <poac/util/termcolor2/termcolor2.hpp>
#include <poac/util/meta.hpp>
#include <poac/util/misc.hpp>
#include <poac/util/net.hpp>
#include <poac/config.hpp>

namespace poac::core::resolver {
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
    [[nodiscard]] mitama::result<void, std::string>
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
            return mitama::failure("renaming a downloaded package failed");
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
            MITAMA_TRY(util::net::api::package_repository(
                resolve::get_name(package), resolve::get_version(package)
            ));
        return mitama::success(convert_to_download_link(repository));
    }

    [[nodiscard]] mitama::result<std::filesystem::path, std::string>
    fetch_impl(const resolve::package_t& package) noexcept {
        try {
            const std::string download_link = MITAMA_TRY(get_download_link(package));
            PLOG_DEBUG << fmt::format("downloading from `{}`", download_link);
            const std::filesystem::path archive_path = get_archive_path(package);
            PLOG_DEBUG << fmt::format("writing to `{}`", archive_path);

            std::ofstream archive(archive_path);
            const auto [host, target] = util::net::parse_url(download_link);
            const util::net::requests requests{ host };
            requests.get(target, {}, std::move(archive));

            return mitama::success(archive_path);
        } catch (...) {
            return mitama::failure("fetching packages failed");
        }
    }

    [[nodiscard]] mitama::result<void, std::string>
    fetch(const resolve::unique_deps_t<resolve::without_deps>& deps) noexcept {
        for (const auto& package : deps) {
            const std::filesystem::path installed_path = MITAMA_TRY(fetch_impl(package));
            const std::string extracted_directory_name =
                MITAMA_TRY(util::archive::extract(installed_path, config::path::extract_dir));
            MITAMA_TRY(rename_extracted_directory(package, extracted_directory_name));

            using termcolor2::color_literals::operator""_green;
            PLOG_INFO << fmt::format(
                "{:>21} {} v{}", "Downloaded"_green,
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
    get_not_installed_deps(const resolve::unique_deps_t<resolve::with_deps>& deps) noexcept {
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

    [[nodiscard]] mitama::result<void, std::string>
    download_deps(const resolve::unique_deps_t<resolve::with_deps>& deps) noexcept {
        const auto not_installed_deps = get_not_installed_deps(deps);
        if (not_installed_deps.empty()) {
            // all resolved packages already have been installed
            return mitama::success();
        }

        using termcolor2::color_literals::operator""_green;
        PLOG_INFO << fmt::format("{:>21} packages ...", "Downloading"_green);
        try {
            std::filesystem::create_directories(config::path::cache_dir);
        } catch (...) {
            return mitama::failure("creating directories failed");
        }
        return fetch(not_installed_deps);
    }

    [[nodiscard]] mitama::result<resolve::unique_deps_t<resolve::with_deps>, std::string>
    do_resolve(const resolve::unique_deps_t<resolve::without_deps>& deps) noexcept {
        try {
            const auto duplicate_deps = MITAMA_TRY(resolve::gather_all_deps(deps));
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
                    resolve::unique_deps_t<resolve::with_deps>(
                        duplicate_deps.cbegin(), duplicate_deps.cend()
                    ));
            } else {
                return resolve::backtrack_loop(duplicate_deps);
            }
        } catch (const core::except::error& e) {
            return mitama::failure(e.what());
        } catch (...) {
            return mitama::failure("resolving packages failed");
        }
    }

    [[nodiscard]] mitama::result<resolve::unique_deps_t<resolve::without_deps>, std::string>
    to_resolvable_deps(const toml::value& deps) noexcept {
        try {
            resolve::unique_deps_t<resolve::without_deps> resolvable_deps{};
            for (const auto& dep : toml::get<toml::table>(deps)) {
                const std::string version = toml::get<std::string>(dep.second);
                resolvable_deps.emplace(dep.first, version);
            }
            return mitama::success(resolvable_deps);
        } catch (...) {
            return mitama::failure(
                "parsing the value of the `dependencies` key in poac.toml failed"
            );
        }
    }

    [[nodiscard]] mitama::result<resolve::unique_deps_t<resolve::with_deps>, std::string>
    install_deps(const toml::value& config) noexcept {
        if (!config.contains("dependencies")) {
            return mitama::success(resolve::unique_deps_t<resolve::with_deps>{});
        }
        const toml::value deps = toml::get<toml::table>(config).at("dependencies");
        const auto resolvable_deps = MITAMA_TRY(to_resolvable_deps(deps));
        const auto resolved_deps = MITAMA_TRY(do_resolve(resolvable_deps));
        MITAMA_TRY(download_deps(resolved_deps));
        return mitama::success(resolved_deps);
    }
}

#endif // POAC_CORE_RESOLVER_HPP

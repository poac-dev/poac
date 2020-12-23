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
#include <fmt/core.h>
#include <mitama/result/result.hpp>
#include <plog/Log.h>
#include <toml.hpp>

// internal
#include <poac/core/resolver/resolve.hpp>
#include <poac/core/resolver/sat.hpp>
#include <poac/io/net.hpp>
#include <poac/io/path.hpp>
#include <poac/util/termcolor2/termcolor2.hpp>
#include <poac/util/misc.hpp>

namespace poac::core::resolver {
    std::filesystem::path
    get_install_path(const resolve::package_t& package) {
        std::filesystem::create_directories(io::path::archive_dir);
        const std::string install_name =
            boost::replace_first_copy(
                resolve::get_name(package), "/", "-"
            ) + "-" + resolve::get_version(package) + ".tar.gz";
        return io::path::archive_dir / install_name;
    }

    std::string
    convert_to_download_link(std::string repository) {
        // repository is like =>
        //   https://github.com/boostorg/winapi/tree/boost-1.66.0
        // convert repository to like =>
        //   https://github.com/boostorg/winapi/archive/boost-1.66.0.tar.gz
        boost::replace_all(repository, "tree", "archive");
        return repository + ".tar.gz";
    }

    [[nodiscard]] mitama::result<std::string, std::string>
    get_download_link(const resolve::package_t& package) {
        const std::string repository =
            MITAMA_TRY(io::net::api::package_repository(
                resolve::get_name(package), resolve::get_version(package)
            ));
        return mitama::success(convert_to_download_link(repository));
    }

    [[nodiscard]] mitama::result<std::filesystem::path, std::string>
    fetch_impl(const resolve::package_t& package) noexcept {
        try {
            const std::string download_link = MITAMA_TRY(get_download_link(package));
            PLOG_DEBUG << fmt::format("downloading from `{}`", download_link);
            const std::filesystem::path install_path = get_install_path(package);
            PLOG_DEBUG << fmt::format("writing to `{}`", install_path);

            std::ofstream archive(install_path);
            const auto [host, target] = io::net::parse_url(download_link);
            const io::net::requests requests{ host };
            requests.get(target, {}, std::move(archive));

            return mitama::success(install_path);
        } catch (...) {
            return mitama::failure("fetching packages failed");
        }
    }

    [[nodiscard]] mitama::result<void, std::string>
    fetch(const resolve::unique_deps_t<resolve::with_deps>& deps) noexcept {
        for (const auto& [package, deps] : deps) {
            const std::filesystem::path installed_path = MITAMA_TRY(fetch_impl(package));
//            result = !result && copy_to_current(cache_name, current_name);

            using termcolor2::color_literals::operator""_green;
            PLOG_INFO << fmt::format(
                "{:>21} {} v{}", "Downloaded"_green,
                resolve::get_name(package),
                resolve::get_version(package)
            );
        }
        return mitama::success();
    }

    [[nodiscard]] mitama::result<void, std::string>
    download_deps(const resolve::unique_deps_t<resolve::with_deps>& deps) noexcept {
        using termcolor2::color_literals::operator""_green;
        PLOG_INFO << fmt::format("{:>21} packages ...", "Downloading"_green);
        try {
            std::filesystem::create_directories(io::path::cache_dir);
        } catch (...) {
            return mitama::failure("creating directories failed");
        }
        return fetch(deps);
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
                return mitama::success(resolve::activated_to_backtracked(duplicate_deps));
            } else {
                return mitama::success(resolve::backtrack_loop(duplicate_deps));
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
                "parsing the `dependencies` key in poac.toml failed"
            );
        }
    }

    [[nodiscard]] mitama::result<void, std::string>
    install_deps(const toml::value& config) noexcept {
        if (!config.contains("dependencies")) {
            return mitama::success();
        }
        const toml::value deps = toml::get<toml::table>(config).at("dependencies");
        const auto resolvable_deps = MITAMA_TRY(to_resolvable_deps(deps));
        const auto resolved_deps = MITAMA_TRY(do_resolve(resolvable_deps));
        return download_deps(resolved_deps);
    }
}

#endif // POAC_CORE_RESOLVER_HPP

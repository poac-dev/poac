#ifndef POAC_CORE_RESOLVER_HPP
#define POAC_CORE_RESOLVER_HPP

// std
#include <string>
#include <stdexcept>
#include <utility>
#include <unordered_map>
#include <vector>

// external
#include <fmt/core.h>
#include <mitama/result/result.hpp>
#include <plog/Log.h>
#include <toml.hpp>

// internal
#include <poac/core/resolver/resolve.hpp>
#include <poac/core/resolver/sat.hpp>
#include <poac/util/termcolor2/termcolor2.hpp>

namespace poac::core::resolver {
    [[nodiscard]] mitama::result<void, std::string>
    fetch(const resolve::unique_deps_t<resolve::with_deps>& deps) {
        for (const auto& [package, deps] : deps) {
//            const std::string cache_name = core::name::to_cache(name, package.version);

//            util::shell clone_cmd(core::resolver::resolve::github::clone_command(name, package.version));
//            clone_cmd += (io::path::poac_cache_dir / cache_name).string();
//            clone_cmd = clone_cmd.to_dev_null().stderr_to_stdout();

//            bool result = clone_cmd.exec().has_value(); // true == error
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
            std::filesystem::create_directories(io::path::poac_cache_dir);
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
                // When all dependencies are one package and one version,
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
            return mitama::failure("parsing poac.toml failed");
        }
    }

    [[nodiscard]] mitama::result<void, std::string>
    install_deps(const toml::value& config) noexcept {
        try {
            const toml::value deps = toml::get<toml::table>(config).at("dependencies");
            const resolve::unique_deps_t<resolve::without_deps> resolvable_deps =
                MITAMA_TRY(to_resolvable_deps(deps));
            const resolve::unique_deps_t<resolve::with_deps> resolved_deps =
                MITAMA_TRY(do_resolve(resolvable_deps));
            return download_deps(resolved_deps);
        } catch (const std::out_of_range&) {
            return mitama::failure(
                "required key `dependencies` is not found in poac.toml"
            );
        }
    }
}

#endif // POAC_CORE_RESOLVER_HPP

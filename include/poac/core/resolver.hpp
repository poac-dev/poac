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
    fetch(const core::resolver::resolve::NoDuplicateDeps& deps) {
        for (const auto& [name, package] : deps) {
//            const std::string cache_name = core::name::to_cache(name, package.version);

//            util::shell clone_cmd(core::resolver::resolve::github::clone_command(name, package.version));
//            clone_cmd += (io::path::poac_cache_dir / cache_name).string();
//            clone_cmd = clone_cmd.to_dev_null().stderr_to_stdout();

//            bool result = clone_cmd.exec().has_value(); // true == error
//            result = !result && copy_to_current(cache_name, current_name);

            using termcolor2::color_literals::operator""_green;
            PLOG_INFO << fmt::format(
                "{:>21} {} v{}", "Downloaded"_green, name, package.version
            );
        }
        return mitama::success();
    }

    [[nodiscard]] mitama::result<void, std::string>
    download_deps(const resolve::NoDuplicateDeps& unique_deps) noexcept {
        using termcolor2::color_literals::operator""_green;
        PLOG_INFO << fmt::format("{:>21} packages ...", "Downloading"_green);
        try {
            std::filesystem::create_directories(io::path::poac_cache_dir);
        } catch (...) {
            return mitama::failure("creating directories failed");
        }
        return fetch(unique_deps);
    }

    [[nodiscard]] mitama::result<resolve::NoDuplicateDeps, std::string>
    to_resolvable_deps(const toml::value& deps) noexcept {
        try {
            resolve::NoDuplicateDeps resolvable_deps{};
            for (const auto& dep : toml::get<toml::table>(deps)) {
                const resolve::Package package(
                    toml::get<std::string>(dep.second)
                );
                resolvable_deps.emplace(dep.first, package);
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
            const auto resolvable_deps = MITAMA_TRY(to_resolvable_deps(deps));
            const auto resolved_deps = MITAMA_TRY(resolve::resolve(resolvable_deps));
            return download_deps(resolved_deps.no_duplicate_deps);

//            const auto duplicate_deps = MITAMA_TRY(gather_all_deps(deps));
//            const auto unique_deps = MITAMA_TRY(to_unique_deps(duplicate_deps));
//            return download_deps(unique_deps);
        } catch (const std::out_of_range&) {
            return mitama::failure(
                "required key `dependencies` is not found in poac.toml"
            );
        }
    }
}

#endif // POAC_CORE_RESOLVER_HPP

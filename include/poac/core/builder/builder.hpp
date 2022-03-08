#ifndef POAC_CORE_BUILDER_BUILDER_HPP
#define POAC_CORE_BUILDER_BUILDER_HPP

// std
#include <cstdint>
#include <chrono>
#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>
#include <optional>

// external
#include <fmt/core.h>
#include <mitama/result/result.hpp>
#include <spdlog/spdlog.h>
#include <spdlog/stopwatch.h>
#include <toml.hpp>

// internal
#include <poac/core/resolver.hpp>
#include <poac/util/termcolor2/termcolor2.hpp>
#include <poac/util/termcolor2/literals_extra.hpp>

namespace poac::core::builder {
    enum class mode_t {
        debug,
        release,
    };

    std::string
    to_string(mode_t mode) {
        switch (mode) {
            case mode_t::debug:
                return "dev";
            case mode_t::release:
                return "release";
#if BOOST_COMP_GNUC
            __builtin_unreachable();
#endif
        }
    }

    std::ostream&
    operator<<(std::ostream& os, mode_t mode) {
        return (os << to_string(mode));
    }

    template <class T>
    std::string make_definition(std::string_view key, T&& value) {
        return fmt::format("-D{}=\\\"{}\\\"", key, std::forward<T>(value));
    }

    using resolved_deps_t =
        resolver::resolve::unique_deps_t<resolver::resolve::with_deps>;

    [[nodiscard]] mitama::result<std::filesystem::path, std::string>
    build_impl(const toml::value&, const mode_t&, const resolved_deps_t&) {
        return mitama::failure("build system is not implemented yet");
    }

    [[nodiscard]] mitama::result<std::filesystem::path, std::string>
    build(const toml::value& config, const mode_t& mode, const resolved_deps_t& resolved_deps) {
        using termcolor2::color_literals::operator""_bold_green;
        spdlog::info(
            "{:>25} {} v{} ({})",
            "Compiling"_bold_green,
            toml::find<std::string>(config, "package", "name"),
            toml::find<std::string>(config, "package", "version"),
            std::filesystem::current_path().string()
        );

        spdlog::stopwatch sw;
        const std::filesystem::path output_path = MITAMA_TRY(
            build_impl(config, mode, resolved_deps)
        );

        spdlog::info(
            "{:>25} {} target(s) in {}",
            "Finished"_bold_green,
            to_string(mode),
            util::pretty::to_time(sw.elapsed().count())
        );
        return mitama::success(output_path);
    }

//    struct Builder {
//        // Prohibit copy.
//        Builder(const Builder&) = delete;
//        Builder& operator=(const Builder&) = delete;
//        Builder(Builder&&) = default;
//        Builder& operator=(Builder&&) = default;
//        Builder() = delete;
//        ~Builder() = default;
//
////    builder::options::compile compile_conf;
////    builder::options::link link_conf;
////    builder::options::static_lib static_lib_conf;
////    builder::options::dynamic_lib dynamic_lib_conf;
//
////    std::string project_name;
//        std::optional<io::config::Config> config;
//        Mode mode;
//        bool verbose;
//        std::filesystem::path base_dir;
//        std::string compiler;
//
//        [[nodiscard]] std::optional<core::except::Error>
//        build() {
//            using namespace std::filesystem::path_literals;
//
//            const auto start = std::chrono::system_clock::now();
//
//            io::term::echo_compiling(this->config, this->base_dir);
//
//            io::config::ProfileBase profile;
//            switch (this->mode) {
//                case Mode::Debug:
//                    profile = config->profile->dev.value();
//                    break;
//                case Mode::Release:
//                    profile = config->profile->release.value();
//                    break;
//            }
//
//            options::compile compile_conf;
//            compile_conf.system = compiler;
//            compile_conf.std_version = standard::convert(config->package.cpp, compiler, false);
//            compile_conf.opt_level = profile.opt_level;
//
//            compile_conf.source_file = "src"_path / "main.cpp";
//
//            compile_conf.include_search_path.emplace_back("include");
//
//
//
//            if (profile.include_directories.has_value()) {
//                compile_conf.include_search_path.insert(
//                        compile_conf.include_search_path.end(),
//                        profile.include_directories->cbegin(),
//                        profile.include_directories->cend());
//            }
//            // TODO: 消す
//            compile_conf.include_search_path.emplace_back("/usr/local/opt/openssl/include");
//
//            compile_conf.base_dir = this->base_dir;
//            compile_conf.output_root = "target";
//            switch (this->mode) {
//                case Mode::Debug:
//                    compile_conf.output_root /= "debug";
//                    break;
//                case Mode::Release:
//                    compile_conf.output_root /= "release";
//                    break;
//            }
//            compile_conf.output_root /= "build";
//
//            // TODO: 修正, POAC_POAC_VERSION_MINOR
//            compile_conf.definitions.emplace_back(make_definition("POAC_VERSION", "0.3.0"));
//            if (profile.definitions.has_value()) {
//                compile_conf.definitions.insert(
//                        compile_conf.definitions.end(),
//                        profile.definitions->cbegin(),
//                        profile.definitions->cend());
//            }
//            // poac automatically define the absolute path of the project's root directory.
////        definitions.emplace_back(make_definition("POAC_PROJECT_ROOT", std::filesystem::current.string()));
////    const auto version = semver::Version(config->version);
////    macro_defns.emplace_back(make_macro_defn("POAC_VERSION", version.get_full()));
////    macro_defns.emplace_back(make_macro_defn("POAC_MAJOR_VERSION", version.major));
////    macro_defns.emplace_back(make_macro_defn("POAC_MINOR_VERSION", version.minor));
////    macro_defns.emplace_back(make_macro_defn("POAC_PATCH_VERSION", version.patch));
//
////            if (const auto result = compile(compile_conf, this->verbose)) {
////            }
//
//            std::cout << io::term::finished << this->mode;
//            std::cout << " [";
//            if (profile.opt_level == "0") {
//                if (profile.debug) {
//                    std::cout << "unoptimized + debuginfo";
//                } else {
//                    std::cout << "unoptimized";
//                }
//            } else if (profile.opt_level == "g"
//                       || profile.opt_level == "1"
//                       || profile.opt_level == "2"
//                       || profile.opt_level == "3"
//                       || profile.opt_level == "s") {
//                if (profile.debug) {
//                    std::cout << "optimized + debuginfo";
//                } else {
//                    std::cout << "optimized";
//                }
//            }
//            std::cout << "] target(s) in ";
//            const auto end = std::chrono::system_clock::now();
//            const std::chrono::duration<double> elapsed = end - start;
//            std::cout << std::fixed << std::setprecision(2) << elapsed.count() << "s";
//            std::cout << std::endl;
//
//            return std::nullopt;
//        }
//
//        Builder(std::optional<io::config::Config>&& config,
//                Mode mode, const bool verbose,
//                const std::filesystem::path& base_dir = io::path::current)
//                : config(std::move(config)), mode(mode), verbose(verbose), base_dir(base_dir)
//                , compiler(standard::detect_command())
//        {
//            if (!config.has_value()) {
//                // TODO: error
//            }
//        }
//    };
} // end namespace

#endif // POAC_CORE_BUILDER_BUILDER_HPP

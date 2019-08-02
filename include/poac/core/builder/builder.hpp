#ifndef POAC_CORE_BUILDER_BUILDER_HPP
#define POAC_CORE_BUILDER_BUILDER_HPP

#include <iostream>
#include <string>
#include <vector>
#include <optional>

#include <boost/filesystem.hpp>

#include <poac/core/builder/compiler.hpp>
#include <poac/core/builder/detect.hpp>
#include <poac/core/builder/standard.hpp>
#include <poac/core/builder/options.hpp>
#include <poac/core/except.hpp>
#include <poac/core/name.hpp>
#include <poac/core/project.hpp>
#include <poac/io/config.hpp>
#include <poac/io/path.hpp>
#include <poac/io/term.hpp>
#include <poac/util/semver/semver.hpp>

namespace poac::core {
    namespace builder {
        enum class Mode {
            Debug,
            Release,
        };
    }

    struct Builder {
        // Prohibit copy.
        Builder(const Builder&) = delete;
        Builder& operator=(const Builder&) = delete;
        Builder(Builder&&) noexcept = default;
        Builder& operator=(Builder&&) noexcept = default;
        Builder() = delete;
        ~Builder() = default;

//        builder::options::compile compile_conf;
//        builder::options::link link_conf;
//        builder::options::static_lib static_lib_conf;
//        builder::options::dynamic_lib dynamic_lib_conf;

//        std::string project_name;
        std::optional<io::config::Config> config;
        builder::Mode mode;
        bool verbose;
        boost::filesystem::path base_directory;
        std::string compiler;

        [[nodiscard]] std::optional<core::except::Error>
        build() {
            const auto [repo, name] = core::project::name();
            const int base_size = 12;
            std::cout << std::right << std::setw(base_size + termcolor2::green<>.size() + termcolor2::reset<>.size())
                      << termcolor2::to_green("Compiling ") << name << " v" << core::project::version() << std::endl;

            builder::options::compile compile_conf;
            compile_conf.system = compiler;
            compile_conf.std_version = builder::standard::convert(config->cpp.value(), compiler, false);
            compile_conf.source_file = "src/main.cpp";
            compile_conf.include_search_path.emplace_back("include");
            compile_conf.base_dir = this->base_directory;
            compile_conf.output_root = "target/debug/build"; // TODO: incremental?

//            boost::filesystem::create_directories("target/debug/build")

            if (const auto result = builder::compiler::compile(compile_conf, this->verbose)) {

            }

            return std::nullopt;
        }

        explicit Builder(
                const std::optional<io::config::Config>& config,
                builder::Mode mode,
                const bool verbose,
                const boost::filesystem::path& base_dir = io::path::current)
        : config(config)
        , mode(mode)
        , verbose(verbose)
        , base_directory(base_dir)
        , compiler(builder::standard::detect_command())
        {}
    };
} // end namespace
#endif // POAC_CORE_BUILDER_BUILDER_HPP

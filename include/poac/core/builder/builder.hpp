#ifndef POAC_CORE_BUILDER_BUILDER_HPP
#define POAC_CORE_BUILDER_BUILDER_HPP

#include <iostream>
#include <string>
#include <vector>
#include <optional>

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
        Builder(Builder&&) = default;
        Builder& operator=(Builder&&) = default;
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
        io::filesystem::path base_dir;
        std::string compiler;

        [[nodiscard]] std::optional<core::except::Error>
        build() {
            using namespace io::filesystem::path_literals;

            io::term::echo_compiling(config, this->base_dir);

            builder::options::compile compile_conf;
            compile_conf.system = compiler;
            compile_conf.std_version = builder::standard::convert(config->package.cpp, compiler, false);
            compile_conf.source_file = ("src"_path / "main.cpp").string();
            compile_conf.include_search_path.emplace_back("include");
            compile_conf.base_dir = this->base_dir;
            compile_conf.output_root = "target";
            switch (this->mode) {
                case builder::Mode::Debug:
                    compile_conf.output_root /= "debug";
                    break;
                case builder::Mode::Release:
                    compile_conf.output_root /= "release";
                    break;
            }
            compile_conf.output_root /= "build";

            if (const auto result = builder::compiler::compile(compile_conf, this->verbose)) {

            }

            std::cout << io::term::finished << std::endl;
            return std::nullopt;
        }

        Builder(std::optional<io::config::Config>&& config,
                builder::Mode mode, const bool verbose,
                const io::filesystem::path& base_dir = io::filesystem::current)
            : config(std::move(config)), mode(mode), verbose(verbose), base_dir(base_dir)
            , compiler(builder::standard::detect_command())
        {
            if (!config.has_value()) {
                // TODO: error
            }
        }
    };
} // end namespace
#endif // POAC_CORE_BUILDER_BUILDER_HPP

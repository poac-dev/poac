#ifndef POAC_CORE_BUILDER_BUILDER_HPP
#define POAC_CORE_BUILDER_BUILDER_HPP

#include <chrono>
#include <iostream>
#include <string>
#include <vector>
#include <optional>

#include <poac/core/builder/compiler.hpp>
#include <poac/core/builder/standard.hpp>
#include <poac/core/except.hpp>
#include <poac/core/name.hpp>
#include <poac/core/project.hpp>
#include <poac/io/config.hpp>
#include <poac/io/filesystem.hpp>
#include <poac/io/term.hpp>
#include <poac/util/pretty.hpp>
#include <poac/util/semver/semver.hpp>

namespace poac::core::builder {
    enum class Mode {
        Debug,
        Release,
    };

    std::ostream&
    operator<<(std::ostream& os, Mode mode) {
        switch (mode) {
            case Mode::Debug:
                return (os << "dev");
            case Mode::Release:
                return (os << "release");
        }
    }

    std::string
    make_definition(const std::string& first, const std::string& second) {
        return "-D" + first + "=" + R"(\")" + second + R"(\")";
    }
    std::string
    make_definition(const std::string& first, const std::uint64_t& second) {
        std::ostringstream oss;
        oss << second;
        return make_definitions(first, oss.str());
    }

    std::vector<std::string>
    make_definitions(const io::config::Config&) {
        std::vector<std::string> definitions;
        // poac automatically define the absolute path of the project's root directory.
        definitions.emplace_back(make_definition("POAC_PROJECT_ROOT", io::filesystem::current.string()));
//    const auto version = semver::Version(config->version); // TODO: versionが無い
//    macro_defns.emplace_back(make_macro_defn("POAC_VERSION", version.get_full()));
//    macro_defns.emplace_back(make_macro_defn("POAC_MAJOR_VERSION", version.major));
//    macro_defns.emplace_back(make_macro_defn("POAC_MINOR_VERSION", version.minor));
//    macro_defns.emplace_back(make_macro_defn("POAC_PATCH_VERSION", version.patch));
        return definitions;
    }

    struct Builder {
        // Prohibit copy.
        Builder(const Builder&) = delete;
        Builder& operator=(const Builder&) = delete;
        Builder(Builder&&) = default;
        Builder& operator=(Builder&&) = default;
        Builder() = delete;
        ~Builder() = default;

//    builder::options::compile compile_conf;
//    builder::options::link link_conf;
//    builder::options::static_lib static_lib_conf;
//    builder::options::dynamic_lib dynamic_lib_conf;

//    std::string project_name;
        std::optional<io::config::Config> config;
        Mode mode;
        bool verbose;
        io::filesystem::path base_dir;
        std::string compiler;

        [[nodiscard]] std::optional<core::except::Error>
        build() {
            using namespace io::filesystem::path_literals;

            const auto start = std::chrono::system_clock::now();

            io::term::echo_compiling(this->config, this->base_dir);

            io::config::ProfileBase profile;
            switch (this->mode) {
                case Mode::Debug:
                    profile = config->profile->dev.value();
                    break;
                case Mode::Release:
                    profile = config->profile->release.value();
                    break;
            }

            options::compile compile_conf;
            compile_conf.system = compiler;
            compile_conf.std_version = standard::convert(config->package.cpp, compiler, false);
            compile_conf.opt_level = profile.opt_level;

            compile_conf.source_file = "src"_path / "main.cpp";

            compile_conf.include_search_path.emplace_back("include");



            if (profile.include_directories.has_value()) {
                compile_conf.include_search_path.insert(
                        compile_conf.include_search_path.end(),
                        profile.include_directories->cbegin(),
                        profile.include_directories->cend());
            }
            // TODO: 消す
            compile_conf.include_search_path.emplace_back("/usr/local/opt/openssl/include");

            compile_conf.base_dir = this->base_dir;
            compile_conf.output_root = "target";
            switch (this->mode) {
                case Mode::Debug:
                    compile_conf.output_root /= "debug";
                    break;
                case Mode::Release:
                    compile_conf.output_root /= "release";
                    break;
            }
            compile_conf.output_root /= "build";

            // TODO: 修正, POAC_POAC_VERSION_MINOR
            compile_conf.definitions.emplace_back(make_definition("POAC_VERSION", "0.3.0"));
            if (profile.definitions.has_value()) {
                compile_conf.definitions.insert(
                        compile_conf.definitions.end(),
                        profile.definitions->cbegin(),
                        profile.definitions->cend());
            }

            if (const auto result = compile(compile_conf, this->verbose)) {

            }

            std::cout << io::term::finished << this->mode;
            std::cout << " [";
            if (profile.opt_level == "0") {
                if (profile.debug) {
                    std::cout << "unoptimized + debuginfo";
                } else {
                    std::cout << "unoptimized";
                }
            } else if (profile.opt_level == "g"
                       || profile.opt_level == "1"
                       || profile.opt_level == "2"
                       || profile.opt_level == "3"
                       || profile.opt_level == "s") {
                if (profile.debug) {
                    std::cout << "optimized + debuginfo";
                } else {
                    std::cout << "optimized";
                }
            }
            std::cout << "] target(s) in ";
            const auto end = std::chrono::system_clock::now();
            const std::chrono::duration<double> elapsed = end - start;
            std::cout << std::fixed << std::setprecision(2) << elapsed.count() << "s";
            std::cout << std::endl;

            return std::nullopt;
        }

        Builder(std::optional<io::config::Config>&& config,
                Mode mode, const bool verbose,
                const io::filesystem::path& base_dir = io::filesystem::current)
                : config(std::move(config)), mode(mode), verbose(verbose), base_dir(base_dir)
                , compiler(standard::detect_command())
        {
            if (!config.has_value()) {
                // TODO: error
            }
        }
    };
} // end namespace
namespace poac::core {
    using Builder = builder::Builder;
}
#endif // POAC_CORE_BUILDER_BUILDER_HPP

#ifndef POAC_CMD_CREATE_HPP
#define POAC_CMD_CREATE_HPP

// std
#include <filesystem>
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <map>
#include <algorithm>
#include <optional>

// external
#include <fmt/core.h>
#include <git2-cpp/git2.hpp>
#include <mitama/result/result.hpp>
#include <mitama/anyhow/anyhow.hpp>
#include <mitama/thiserror/thiserror.hpp>
#include <spdlog/spdlog.h>
#include <structopt/app.hpp>

// internal
#include <poac/core/validator.hpp>
#include <poac/data/manifest.hpp>
#include <poac/util/termcolor2/termcolor2.hpp>
#include <poac/util/termcolor2/literals_extra.hpp>
#include <poac/util/misc.hpp>

namespace poac::cmd::create {
    namespace anyhow = mitama::anyhow;
    namespace thiserror = mitama::thiserror;

    struct Options: structopt::sub_command {
        /// Package name to create a new poac package
        std::string package_name;

        /// Use a binary (application) template [default]
        std::optional<bool> bin = false;
        /// Use a library template
        std::optional<bool> lib = false;
    };

    class Error {
        template <thiserror::fixed_string S, class ...T>
        using error = thiserror::error<S, T...>;

    public:
        using PassingBothBinAndLib =
            error<"cannot specify both lib and binary outputs">;
    };

    enum class ProjectType {
        Bin,
        Lib,
    };

    std::ostream&
    operator<<(std::ostream& os, ProjectType kind) {
        switch (kind) {
            case ProjectType::Bin:
                return (os << "binary (application)");
            case ProjectType::Lib:
                return (os << "library");
            default:
                throw std::logic_error(
                        "To access out of range of the "
                        "enumeration values is undefined behavior."
                );
        }
    }

    template <typename T>
    ProjectType
    opts_to_project_type(T&& opts) {
        opts.bin.value(); // Just check opts has a `.bin` member
        return opts.lib.value() ? ProjectType::Lib : ProjectType::Bin;
    }

    namespace files {
        inline std::string
        poac_toml(std::string_view project_name) {
            return fmt::format(
                "[package]\n"
                "name = \"{}\"\n"
                "version = \"0.1.0\"\n"
                "authors = []\n"
                "cpp = 17",
                project_name
            );
        }

        inline const std::string main_cpp(
            "#include <iostream>\n\n"
            "int main(int argc, char** argv) {\n"
            "    std::cout << \"Hello, world!\" << std::endl;\n"
            "}"
        );

        inline std::string include_hpp(std::string_view project_name) {
            std::string project_name_upper_cased{};
            std::transform(
                project_name.cbegin(),
                project_name.cend(),
                std::back_inserter(project_name_upper_cased),
                ::toupper
            );

            return fmt::format(
                "#ifndef {0}_HPP\n"
                "#define {0}_HPP\n\n"
                "namespace {1} {{\n}}\n\n"
                "#endif // !{0}_HPP\n",
                project_name_upper_cased,
                project_name
            );
        }
    }

    void write_to_file(std::ofstream& ofs, const std::string& fname, std::string_view text) {
        ofs.open(fname);
        if (ofs.is_open()) {
            ofs << text;
        }
        ofs.close();
        ofs.clear();
    }

    std::map<std::filesystem::path, std::string>
    create_template_files(const ProjectType& type, const std::string& package_name) {
        using util::misc::path_literals::operator""_path;

        switch (type) {
            case ProjectType::Bin:
                std::filesystem::create_directories(package_name / "src"_path);
                return {
                    { ".gitignore", "/target" },
                    { data::manifest::manifest_file_name, files::poac_toml(package_name) },
                    { "src"_path / "main.cpp", files::main_cpp }
                };
            case ProjectType::Lib:
                std::filesystem::create_directories(
                    package_name / "include"_path / package_name
                );
                return {
                    { ".gitignore", "/target\npoac.lock" },
                    { data::manifest::manifest_file_name, files::poac_toml(package_name) },
                    { "include"_path / package_name / (package_name + ".hpp"),
                        files::include_hpp(package_name)
                    },
                };
            default:
                throw std::logic_error(
                        "To access out of range of the "
                        "enumeration values is undefined behavior."
                );
        }
    }

    [[nodiscard]] anyhow::result<void>
    create(const Options& opts) {
        std::ofstream ofs;
        const ProjectType type = opts_to_project_type(opts);
        for (auto&& [name, text] : create_template_files(type, opts.package_name)) {
            const std::string& file_path = (opts.package_name / name).string();
            spdlog::trace("Creating {}", file_path);
            write_to_file(ofs, file_path, text);
        }

        spdlog::trace(
            "Initializing git repository at {}", opts.package_name
        );
        git2::repository().init(opts.package_name);

        using termcolor2::color_literals::operator""_bold_green;
        spdlog::info(
            "{:>25} {} `{}` package",
            "Created"_bold_green,
            type,
            opts.package_name
        );
        return mitama::success();
    }

    [[nodiscard]] anyhow::result<void>
    exec(const Options& opts) {
        if (opts.bin.value() && opts.lib.value()) {
            return anyhow::failure<Error::PassingBothBinAndLib>();
        }

        spdlog::trace("Validating the `{}` directory exists", opts.package_name);
        MITAMA_TRY(
            core::validator::can_crate_directory(opts.package_name)
                .map_err([](const std::string& e){ return anyhow::anyhow(e); })
        );

        spdlog::trace("Validating the package name `{}`", opts.package_name);
        MITAMA_TRY(
            core::validator::valid_package_name(opts.package_name)
                .map_err([](const std::string& e){ return anyhow::anyhow(e); })
        );

        return create(opts);
    }
} // end namespace

STRUCTOPT(poac::cmd::create::Options, package_name, bin, lib);

#endif // !POAC_CMD_CREATE_HPP

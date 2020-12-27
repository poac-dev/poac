#ifndef POAC_CMD_NEW_HPP
#define POAC_CMD_NEW_HPP

// std
#include <filesystem>
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <map>
#include <algorithm>

// external
#include <fmt/core.h>
#include <mitama/result/result.hpp>
#include <plog/Log.h>

// internal
#include <poac/core/validator.hpp>
#include <poac/util/git2-cpp/git2.hpp>
#include <poac/util/termcolor2/termcolor2.hpp>

namespace poac::cmd::_new {
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

    struct Options {
        ProjectType type;
        std::string package_name;
    };

    void write_to_file(std::ofstream& ofs, const std::string& fname, std::string_view text) {
        ofs.open(fname);
        if (ofs.is_open()) {
            ofs << text;
        }
        ofs.close();
        ofs.clear();
    }

    std::map<std::filesystem::path, std::string>
    create_template_files(const _new::Options& opts) {
        using io::path::path_literals::operator""_path;

        switch (opts.type) {
            case ProjectType::Bin:
                std::filesystem::create_directories(opts.package_name / "src"_path);
                return {
                    { ".gitignore", "/target" },
                    { "poac.toml", files::poac_toml(opts.package_name) },
                    { "src"_path / "main.cpp", files::main_cpp }
                };
            case ProjectType::Lib:
                std::filesystem::create_directories(
                    opts.package_name / "include"_path / opts.package_name
                );
                return {
                    { ".gitignore", "/target\npoac.lock" },
                    { "poac.toml", files::poac_toml(opts.package_name) },
                    { "include"_path / opts.package_name / (opts.package_name + ".hpp"),
                        files::include_hpp(opts.package_name)
                    },
                };
            default:
                throw std::logic_error(
                        "To access out of range of the "
                        "enumeration values is undefined behavior."
                );
        }
    }

    [[nodiscard]] mitama::result<void, std::string>
    _new(_new::Options&& opts) {
        using termcolor2::color_literals::operator""_green;

        std::ofstream ofs;
        for (auto&& [name, text] : create_template_files(opts)) {
            const std::string& file_path = (opts.package_name / name).string();
            PLOG_VERBOSE << fmt::format("Creating {}", file_path);
            write_to_file(ofs, file_path, text);
        }

        PLOG_VERBOSE << fmt::format(
            "Initializing git repository at {}", opts.package_name
        );
        git2::repository().init(opts.package_name);

        PLOG_INFO << fmt::format(
            "{}{} `{}` package",
            "Created: "_green,
            opts.type,
            opts.package_name
        );
        return mitama::success();
    }

    [[nodiscard]] mitama::result<void, std::string>
    exec(Options&& opts) {
        PLOG_VERBOSE <<
            fmt::format(
                "Validating the `{}` directory exists",
                opts.package_name
            );
        MITAMA_TRY(core::validator::can_crate_directory(opts.package_name));

        PLOG_VERBOSE <<
            fmt::format(
                "Validating the package name `{}`",
                opts.package_name
            );
        MITAMA_TRY(core::validator::valid_package_name(opts.package_name));

        return _new(std::move(opts));
    }
} // end namespace

#endif // !POAC_CMD_NEW_HPP

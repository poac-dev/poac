#ifndef POAC_CMD_NEW_HPP
#define POAC_CMD_NEW_HPP

// Std
#include <filesystem>
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <map>
#include <algorithm>
#include <vector>
#include <optional>

// External
#include <fmt/core.h>

// Internal
#include <poac/core/except.hpp>
#include <poac/core/name.hpp>
#include <poac/io/path.hpp>
#include <poac/io/term.hpp>
#include <poac/util/argparse.hpp>
#include <poac/util/clap/clap.hpp>
#include <poac/util/git2-cpp/git2.hpp>
#include <poac/util/termcolor2/termcolor2.hpp>

namespace poac::cmd::_new {
    namespace files {
        inline std::string
        poac_toml(const std::string& project_name) {
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

        inline std::string include_hpp(const std::string& project_name) {
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
        using std::filesystem::path_literals::operator""_path;

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

    [[nodiscard]] std::optional<core::except::Error>
    check_name(std::string_view name) {
        // Ban keywords
        // https://en.cppreference.com/w/cpp/keyword
        std::vector<std::string_view> blacklist{
            "alignas", "alignof", "and", "and_eq", "asm", "atomic_cancel", "atomic_commit", "atomic_noexcept",
            "auto", "bitand", "bitor", "bool", "break", "case", "catch", "char", "char8_t", "char16_t", "char32_t",
            "class", "compl", "concept", "const", "consteval", "constexpr", "const_cast", "continue", "co_await",
            "co_return", "co_yield", "decltype", "default", "delete", "do", "double", "dynamic_cast", "else", "enum",
            "explicit", "export", "extern", "false", "float", "for", "friend", "goto", "if", "inline", "int", "long",
            "mutable", "namespace", "new", "noexcept", "not", "not_eq", "nullptr", "operator", "or", "or_eq", "private",
            "protected", "public", "reflexpr", "register", "reinterpret_cast", "requires", "return", "short", "signed",
            "sizeof", "static", "static_assert", "static_cast", "struct", "switch", "synchronized", "template", "this",
            "thread_local", "throw", "true", "try", "typedef", "typeid", "typename", "union", "unsigned", "using",
            "virtual", "void", "volatile", "wchar_t", "while", "xor", "xor_eq",
        };
        if (std::find(blacklist.begin(), blacklist.end(), name) != blacklist.end()) {
            return core::except::Error::General{
                fmt::format(
                    "`{}` is a keyword, so it cannot be used as a package name",
                    name
                    )
            };
        }
        return std::nullopt;
    }

    [[nodiscard]] std::optional<core::except::Error>
    validate(const _new::Options& opts) {
        if (const auto error = core::name::validate_package_name(opts.package_name)) {
            return error;
        }
        if (io::path::validate_dir(opts.package_name)) {
            return core::except::Error::General{
                fmt::format(
                    "The `{}` directory already exists", opts.package_name
                    )
            };
        }
        if (const auto error = check_name(opts.package_name)) {
            return error;
        }
        return std::nullopt;
    }

    [[nodiscard]] std::optional<core::except::Error>
    _new(_new::Options&& opts) {
        using termcolor2::color_literals::operator""_green;

        if (const auto error = validate(opts)) {
            return error;
        }
        std::ofstream ofs;
        for (auto&& [name, text] : create_template_files(opts)) {
            write_to_file(ofs, (opts.package_name / name).string(), text);
        }
        git2::repository().init(opts.package_name);
        fmt::print(
            "{}{} `{}` package\n",
            "Created: "_green,
            opts.type,
            opts.package_name
        );
        return std::nullopt;
    }

    [[nodiscard]] std::optional<core::except::Error>
    exec(Options&& opts) {
        return _new(std::move(opts));
    }
} // end namespace

#endif // !POAC_CMD_NEW_HPP

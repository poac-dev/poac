#ifndef POAC_OPTS_NEW_HPP
#define POAC_OPTS_NEW_HPP

#include <iostream>
#include <fstream>
#include <string>
#include <string_view>
#include <map>
#include <regex>
#include <algorithm>
#include <vector>
#include <optional>

#include <boost/filesystem.hpp>

#include <poac/core/except.hpp>
#include <poac/core/name.hpp>
#include <poac/io/term.hpp>
#include <poac/io/path.hpp>
#include <poac/util/argparse.hpp>
#include <poac/util/shell.hpp>
#include <poac/util/termcolor2.hpp>

namespace poac::opts::_new {
    constexpr auto summary = termcolor2::make_string("Create a new poac project");
    constexpr auto options = termcolor2::make_string("[<project-name> | -b, --bin | -l, --lib]");

    namespace files {
        namespace bin {
            const std::string _gitignore(
                    "/target"
            );
            const std::string poac_yml(
                    "cpp_version: 17\n"
                    "build:\n"
                    "  bin: true"
            );
            const std::string main_cpp(
                    "#include <iostream>\n\n"
                    "int main(int argc, char** argv) {\n"
                    "    std::cout << \"Hello, world!\" << std::endl;\n"
                    "}"
            );
        }
        namespace lib {
            const std::string _gitignore(
                    "/target\n"
                    "poac.lock"
            );
            const std::string poac_yml(
                    "cpp_version: 17"
            );
            std::string include_hpp(std::string project_name) {
                std::transform(project_name.cbegin(), project_name.cend(), project_name.begin(), ::toupper);
                return "#ifndef " + project_name + "_HPP\n"
                       "#define " + project_name + "_HPP\n\n"
                       "#endif // !" + project_name + "_HPP";
            }
        }
    }

    struct Options {
        bool lib;
        bool bin;
        std::string project_name;
    };

    std::map<boost::filesystem::path, std::string>
    create_template_files(const _new::Options& opts) {
        namespace fs = boost::filesystem;
        using io::path::path_literals::operator""_path;

        if (opts.bin) {
            fs::create_directories(opts.project_name / "src"_path);
            return {
                { ".gitignore", files::bin::_gitignore },
                { "poac.yml", files::bin::poac_yml },
                { "src"_path / "main.cpp", files::bin::main_cpp }
            };
        } else {
            fs::create_directories(opts.project_name / "include"_path / opts.project_name);
            return {
                { ".gitignore", files::lib::_gitignore },
                { "poac.yml", files::lib::poac_yml },
                { "include"_path / opts.project_name / (opts.project_name + ".hpp"),
                    files::lib::include_hpp(opts.project_name)
                },
            };
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
                "`", name, "` is a keyword, so it cannot be used as a package name."
            };
        }
        return std::nullopt;
    }

    [[nodiscard]] std::optional<core::except::Error>
    validate(const _new::Options& opts) {
        if (const auto error = core::name::validate_package_name(opts.project_name)) {
            return error;
        }
        if (io::path::validate_dir(opts.project_name)) {
            return core::except::Error::General{
                core::except::msg::already_exist("The `" + opts.project_name + "` directory")
            };
        }
        if (const auto error = check_name(opts.project_name)) {
            return error;
        }
        return std::nullopt;
    }

    [[nodiscard]] std::optional<core::except::Error>
    _new(_new::Options&& opts) {
        namespace fs = boost::filesystem;
        using termcolor2::color_literals::operator""_green;

        if (const auto error = validate(opts)) {
            return error;
        }

        std::ofstream ofs;
        for (auto&& [name, text] : create_template_files(opts)) {
            io::path::write_to_file(ofs, (opts.project_name / name).string(), text);
        }

        std::cout << "Created: "_green;
        if (opts.bin) {
            std::cout << "application ";
        } else {
            std::cout << "library ";
        }
        std::cout << "`" << opts.project_name << "` "
                  << "project"
                  << std::endl;

        if (util::_shell::has_command("git")) {
            const std::string git_init = "git init " + opts.project_name;
            util::shell(git_init).exec();
            std::cout << "Running: "_green << git_init << std::endl;
        }
        return std::nullopt;
    }

    [[nodiscard]] std::optional<core::except::Error>
    exec(std::optional<io::yaml::Config>&&, std::vector<std::string>&& args) {
        _new::Options opts{};
        opts.lib = util::argparse::use_rm(args, "-l", "--lib");
        // If lib is false then bin is true. If both are specified, bin takes precedence.
        opts.bin = !opts.lib || util::argparse::use_rm(args, "-b", "--bin");
        if (args.size() != 1) {
            return core::except::Error::InvalidSecondArg::New;
        }
        opts.project_name = args[0];
        return _new::_new(std::move(opts));
    }
} // end namespace
#endif // !POAC_OPTS_NEW_HPP

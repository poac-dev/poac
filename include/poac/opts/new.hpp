#ifndef POAC_OPTS_NEW_HPP
#define POAC_OPTS_NEW_HPP

#include <iostream>
#include <fstream>
#include <string>
#include <string_view>
#include <map>
#include <algorithm>
#include <vector>
#include <optional>

#include <boost/filesystem.hpp>

#include <poac/core/except.hpp>
#include <poac/core/name.hpp>
#include <poac/io/term.hpp>
#include <poac/io/path.hpp>
#include <poac/util/argparse.hpp>
#include <poac/util/termcolor2.hpp>
#include <poac/util/git2-cpp/git2.hpp>
#include <poac/util/clap/clap.hpp>

namespace poac::opts::_new {
    const clap::subcommand cli =
            clap::subcommand("new")
                .about("Create a new poac package at <path>")
                .arg(clap::opt("quiet", "No output printed to stdout").short_("q"))
                .arg(clap::arg("path").required(true))
                .arg(clap::opt("registry", "Registry to use").value_name("REGISTRY"))
                .arg(
                    clap::opt(
                        "vcs",
                        "Initialize a new repository for the given version "
                        "control system (git, hg, pijul, or fossil) or do not "
                        "initialize any version control at all (none), overriding "
                        "a global configuration."
                    )
                    .value_name("VCS")
                    .possible_values({"git", "hg", "pijul", "fossil", "none"})
                )
                .arg(clap::opt("bin", "Use a binary (application) template [default]"))
                .arg(clap::opt("lib", "Use a library template"))
                .arg(
                    clap::opt("cpp", "Edition to set for the package generated")
                    .possible_values({"98", "3", "11", "14", "17", "20"})
                    .value_name("VERSION")
                )
                .arg(
                    clap::opt(
                        "name",
                        "Set the resulting package name, defaults to the directory name"
                    )
                    .value_name("NAME")
                )
            ;

    namespace files {
        namespace bin {
            std::string poac_toml(const std::string&) {
                return "cpp = 17";
            }
            const std::string main_cpp(
                    "#include <iostream>\n\n"
                    "int main(int argc, char** argv) {\n"
                    "    std::cout << \"Hello, world!\" << std::endl;\n"
                    "}"
            );
        }
        namespace lib {
            const std::string poac_toml(
                    "cpp = 17"
            );
            std::string include_hpp(std::string project_name) {
                std::transform(project_name.cbegin(), project_name.cend(), project_name.begin(), ::toupper);
                return "#ifndef " + project_name + "_HPP\n"
                       "#define " + project_name + "_HPP\n\n"
                       "#endif // !" + project_name + "_HPP";
            }
        }
    }

    enum class NewProjectKind {
        Bin,
        Lib,
    };

    std::ostream& operator<<(std::ostream& os, NewProjectKind kind) {
        switch (kind) {
            case NewProjectKind::Bin:
                os << "binary (application)";
                break;
            case NewProjectKind::Lib:
                os << "library";
                break;
        }
        return os;
    }

    struct Options {
        NewProjectKind kind;
        std::string project_name;
    };

    std::map<boost::filesystem::path, std::string>
    create_template_files(const _new::Options& opts) {
        namespace fs = boost::filesystem;
        using io::path::path_literals::operator""_path;

        switch (opts.kind) {
            case NewProjectKind::Bin:
                fs::create_directories(opts.project_name / "src"_path);
                return {
                    { ".gitignore", "/target" },
                    { "poac.toml", files::bin::poac_toml(opts.project_name) },
                    { "src"_path / "main.cpp", files::bin::main_cpp }
                };
            case NewProjectKind::Lib:
                fs::create_directories(opts.project_name / "include"_path / opts.project_name);
                return {
                    { ".gitignore", "/target\npoac.lock" },
                    { "poac.toml", files::lib::poac_toml },
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
        using termcolor2::color_literals::operator""_green;

        if (const auto error = validate(opts)) {
            return error;
        }
        std::ofstream ofs;
        for (auto&& [name, text] : create_template_files(opts)) {
            io::path::write_to_file(ofs, (opts.project_name / name).string(), text);
        }
        git2::repository().init(opts.project_name);
        std::cout << "Created: "_green << opts.kind
                  << " `" << opts.project_name << "` " << "package" << std::endl;
        return std::nullopt;
    }

    [[nodiscard]] std::optional<core::except::Error>
    exec(std::optional<io::config::Config>&&, std::vector<std::string>&& args) {
        _new::Options opts{};
        const bool bin = util::argparse::use_rm(args, "-b", "--bin");
        const bool lib = util::argparse::use_rm(args, "-l", "--lib");
        if (bin && lib) {
            return core::except::Error::General{
                "You cannot specify both lib and binary outputs."
            };
        } else if (!bin && lib) {
            opts.kind = NewProjectKind::Lib;
        } else {
            opts.kind = NewProjectKind::Bin;
        }

        if (args.size() != 1) {
            return core::except::Error::InvalidSecondArg::New;
        }
        opts.project_name = args[0];
        return _new::_new(std::move(opts));
    }
} // end namespace
#endif // !POAC_OPTS_NEW_HPP

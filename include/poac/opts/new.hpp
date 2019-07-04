#ifndef POAC_OPTS_NEW_HPP
#define POAC_OPTS_NEW_HPP

#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <regex>
#include <algorithm>
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
    constexpr auto options = termcolor2::make_string("<project-name>, (-b | --bin) | (-l | --lib)");

    namespace files {
        namespace bin {
            const std::string _gitignore(
                    "/deps\n"
                    "/_build\n"
            );
        }
        namespace lib {
            const std::string _gitignore(
                    "/deps\n"
                    "/_build\n"
                    "poac.lock\n"
            );
        }

        std::string README_md(const std::string& project_name) {
            return "# " + project_name + "\n"
                   "**TODO: Add description**\n"
                   "\n"
                   "---\n"
                   "This project uses [poac](https://github.com/poacpm/poac).\n"
                   "\n"
                   "For more information on poac please see below:\n"
                   "* https://poac.pm\n"
                   "* https://github.com/poacpm\n"
                   "* https://github.com/poacpm/poac#readme\n"
                   "\n"
                   "## Build\n"
                   "\n"
                   "```bash\n"
                   "$ poac build # or run\n"
                   "```\n"
                   "\n"
                   "## Installation\n"
                   "\n"
                   "To install `" + project_name + "`, add it to the dependency list of `poac.yml`:\n"
                   "\n"
                   "```yaml\n"
                   "deps:\n"
                   "  " + project_name + ": \">=0.1.0 and <1.0.0\"\n"
                   "```\n"
                   "\n"
                   "After that, execute `poac install` command to install `" + project_name + "`.\n"
                   ;
        }
        std::string poac_yml(const std::string& project_name, const std::string& type) {
            return "name: " + project_name + "\n"
                   "version: 0.1.0\n"
                   "cpp_version: 17\n"
                   "description: \"**TODO: Add description**\"\n"
                   "owners:\n"
                   "  - \"Your ID\"\n"
                   "build:\n"
                   "  system: poac\n" +
                   "  " + type + ": true\n";
        }
        const std::string main_cpp(
                "#include <iostream>\n"
                "\n"
                "int main(int argc, char** argv) {\n"
                "    std::cout << \"Hello, world!\" << std::endl;\n"
                "}\n"
        );
        std::string include_hpp(const std::string& project_name) {
            return "#include <iostream>\n"
                   "\n"
                   "namespace " + project_name + " {\n"
                   "\n"
                   "}\n";
        }
    }

    std::optional<core::except::Error>
    exec(const std::vector<std::string> &argv) {
        namespace except = core::except;
        namespace fs = boost::filesystem;
        namespace path = io::path;
        namespace cli = io::term;
        namespace name = core::name;
        using termcolor2::color_literals::operator""_green;
        using io::path::path_literals::operator""_path;

        std::vector<std::string> argv_cpy = argv;
        bool lib = util::argparse::use_rm(argv_cpy, "-l", "--lib");
        // libが存在しないならどちらにせよ，binが選択される．
        // libが存在し，binも存在するなら，binが優先される．
        const bool bin = !lib || util::argparse::use_rm(argv_cpy, "-b", "--bin");
        // libとbinを引数から抜いた時点で，1じゃなかったらエラーになる．
        if (argv_cpy.size() != 1) {
            return except::Error::InvalidSecondArg::New{};
        }

        const std::string project_name = argv_cpy[0];
        const fs::path project_path = fs::path(project_name);
        name::validate_package_name(project_name);
        if (io::path::validate_dir(project_name)) {
            return except::Error::General{
                    except::msg::already_exist("The `" + project_name + "` directory")
            };
        }

        fs::create_directories(project_name);
        std::ofstream ofs;
        std::map<fs::path, std::string> file;
        if (bin) {
            file = {
                    { ".gitignore", files::bin::_gitignore },
                    { "README.md",  files::README_md(project_name) },
                    { "poac.yml",   files::poac_yml(project_name, "bin") },
                    { "main.cpp",   files::main_cpp }
            };
        } else {
            fs::create_directories(project_path / "include" / project_name);
            file = {
                    { ".gitignore", files::lib::_gitignore },
                    { "README.md",  files::README_md(project_name) },
                    { "poac.yml",   files::poac_yml(project_name, "lib") },
                    { "include"_path / project_name / (project_name + ".hpp"), files::include_hpp(project_name) },
            };
        }
        for (const auto& [name, text] : file) {
            path::write_to_file(ofs, (project_path / name).string(), text);
        }
        std::cout << "Created: "_green;
        if (bin) {
            std::cout << "application ";
        } else {
            std::cout << "library ";
        }
        std::cout << "`" << project_name << "` "
                  << "project"
                  << std::endl;

        if (util::_shell::has_command("git")) {
            const std::string git_init = "git init " + project_name;
            util::shell(git_init).exec();
            std::cout << "Running: "_green << git_init << std::endl;
        }

        return std::nullopt;
    }
} // end namespace
#endif // !POAC_OPTS_NEW_HPP

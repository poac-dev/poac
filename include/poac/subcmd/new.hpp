#ifndef POAC_SUBCMD_NEW_HPP
#define POAC_SUBCMD_NEW_HPP

#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <algorithm>

#include <boost/filesystem.hpp>

#include "../core/exception.hpp"
#include "../io.hpp"
#include "../util/ftemplate.hpp"


namespace poac::subcmd {
    namespace _new {
        void echo_info(const std::string& str) {
            std::cout << io::cli::bold
                      << "\n"
                         "Your \"" + str + "\" project was created successfully.\n"
                         "\n"
                         "\n"
                         "Go into your project by running:\n"
                         "    $ cd " + str + "\n"
                         "\n"
                         "Start your project with:\n"
                         "    $ poac run\n"
                         "\n"
                      << io::cli::reset;
        }

        void exec_new(const std::string& dirname) {
            namespace except = core::exception;
            namespace fs = boost::filesystem;
            namespace path = io::file::path;
            namespace ftmpl = util::ftemplate;

            const auto is_slash = [](const char c) {
                return c == '/';
            };
            // /name
            if (is_slash(dirname[0])) {
                throw except::error("Invalid name.\n"
                                    "It is prohibited to add /(slash)\n"
                                    " at the begenning of a project name.");
            }
            // org/name/sub
            else if (std::count_if(dirname.begin(), dirname.end(), is_slash) > 1) {
                throw except::error("Invalid name.\n"
                                    "It is prohibited to use two\n"
                                    " /(slashes) in a project name.");
            }

            fs::create_directories(dirname);
            std::ofstream ofs;
            std::map<fs::path, std::string> file{
                {".gitignore", ftmpl::_gitignore},
                {"main.cpp",   ftmpl::main_cpp},
                {"poac.yml",   ftmpl::poac_yml(dirname)},
                {"README.md",  ftmpl::README_md(dirname)}
            };
            for (const auto& [name, text] : file)
                path::write_to_file(ofs, (fs::path(dirname) / name).string(), text);
            echo_info(dirname);
        }

        template<typename VS, typename = std::enable_if_t<std::is_rvalue_reference_v<VS&&>>>
        void _main(VS&& argv) {
            namespace fs = boost::filesystem;
            exec_new(argv[0]);
        }

        void check_arguments(const std::vector<std::string>& argv) {
            namespace except = core::exception;
            if (argv.size() != 1)
                throw except::invalid_second_arg("new");
            else if (io::file::path::validate_dir(argv[0]))
                throw except::error("The " + argv[0] + " directory already exists.");
        }
    }

    struct new_ {
        static const std::string summary() {
            return "Create a new poacpm project";
        }
        static const std::string options() {
            return "<project-name>";
        }
        template<typename VS, typename = std::enable_if_t<std::is_rvalue_reference_v<VS&&>>>
        void operator()(VS&& argv) {
            _new::check_arguments(argv);
            _new::_main(std::move(argv));
        }
    };
} // end namespace
#endif // !POAC_SUBCMD_NEW_HPP

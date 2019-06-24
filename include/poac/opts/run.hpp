#ifndef POAC_OPTS_RUN_HPP
#define POAC_OPTS_RUN_HPP

#include <iostream>
#include <string>
#include <regex>
#include <cstdlib>

#include <boost/filesystem.hpp>

#include "./build.hpp"
#include "../core/except.hpp"
#include "../core/stroite/utils/absorb.hpp"
#include "../io/path.hpp"
#include "../io/tar.hpp"
#include "../io/yaml.hpp"
#include "../io/cli.hpp"
#include "../util/shell.hpp"
#include "../util/termcolor2.hpp"


namespace poac::opts {
    namespace _run {
        template<typename VS>
        int _main(VS&& argv) {
            namespace fs = boost::filesystem;

            using termcolor2::color_literals::operator""_green;


            // stroite::core::Builder bs(fs::current_directory());
            // bs.build(verbose); -> if EXIT_SUCCESS ->
            // return bs.build(verbose) && bs.run(verbose); -> 短絡評価される？されない？

            std::vector<std::string> program_args;
            // poac run -v -- -h build
            auto result = std::find(argv.begin(), argv.end(), "--");
            if (result != argv.end()) {
                // -h build
                program_args = std::vector<std::string>(result + 1, argv.end());
            }
            // -v
            _build::check_arguments(std::vector<std::string>(argv.begin(), result));
            if (_build::_main(std::vector<std::string>{}) == EXIT_FAILURE) {
                return EXIT_FAILURE;
            }

            // TODO: このexecutableなパスをもう一度取ってくるのが二度手間感がある．-> build systemに，runも付ける？ -> そうすれば，下のログ表示も，二分されないので，便利では？
            const std::string project_name = io::yaml::load_config("name").as<std::string>();
            const std::string bin_name = project_name + core::stroite::utils::absorb::binary_extension;
            const fs::path executable_path = fs::relative(io::path::current_build_bin_dir / bin_name);
            const std::string executable = executable_path.string();
            util::shell cmd(executable);
            for (const auto& s : program_args) {
                cmd += s;
            }

            std::cout << "Running: "_green
                      << "`" + executable + "`"
                      << std::endl;
            if (const auto ret = cmd.exec()) {
                std::cout << *ret;
            }
            else {
                std::cout << project_name + " returned 1" << std::endl;
            }

            return EXIT_SUCCESS;
        }
    }

    struct run {
        static std::string summary() {
            return "Build project and exec it";
        }
        static std::string options() {
            return "[-v | --verbose | -- [program args]]";
        }
        template <typename VS>
        int operator()(VS&& argv) {
            return _run::_main(std::forward<VS>(argv));
        }
    };
} // end namespace
#endif // !POAC_OPTS_RUN_HPP

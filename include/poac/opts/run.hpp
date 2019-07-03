#ifndef POAC_OPTS_RUN_HPP
#define POAC_OPTS_RUN_HPP

#include <iostream>
#include <string>
#include <regex>
#include <cstdlib>

#include <boost/filesystem.hpp>

#include <poac/opts/build.hpp>
#include <poac/core/except.hpp>
#include <poac/core/builder/absorb.hpp>
#include <poac/io/path.hpp>
#include <poac/io/tar.hpp>
#include <poac/io/yaml.hpp>
#include <poac/io/cli.hpp>
#include <poac/util/shell.hpp>
#include <poac/util/termcolor2.hpp>

namespace poac::opts::run {
    constexpr auto summary = termcolor2::make_string("Build project and exec it");
    constexpr auto options = termcolor2::make_string("[-v | --verbose | -- [program args]]");

    std::optional<core::except::Error>
    _main(const std::vector<std::string>& argv) {
        namespace fs = boost::filesystem;

        using termcolor2::color_literals::operator""_green;


        // builder::core::Builder bs(fs::current_directory());
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
        build::check_arguments(std::vector<std::string>(argv.begin(), result));
        if (const auto result = build::_main(std::vector<std::string>{})) {
            return result;
        }

        // TODO: このexecutableなパスをもう一度取ってくるのが二度手間感がある．-> build systemに，runも付ける？ -> そうすれば，下のログ表示も，二分されないので，便利では？
        const std::string project_name = io::yaml::load_config("name").as<std::string>();
        const std::string bin_name = project_name + core::builder::absorb::binary_extension;
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

        return std::nullopt;
    }
} // end namespace
#endif // !POAC_OPTS_RUN_HPP

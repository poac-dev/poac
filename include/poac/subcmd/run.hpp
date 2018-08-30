#ifndef POAC_SUBCMD_RUN_HPP
#define POAC_SUBCMD_RUN_HPP

#include <iostream>
#include <string>
#include <regex>

#include <boost/filesystem.hpp>
#include <boost/range/iterator_range.hpp>

#include "../core/exception.hpp"
#include "../io/file.hpp"
#include "../io/cli.hpp"
#include "../util/command.hpp"
#include "../util/package.hpp"
#include "./build.hpp"


namespace poac::subcmd { struct run {
    static const std::string summary() { return "Beta: Build project and exec it."; }
    static const std::string options() { return "[-v | --verbose | -- [program args]]"; }


    auto check_requirements() {
        namespace fs     = boost::filesystem;
        namespace except = core::exception;

        // TODO: I want use Result type like rust-lang.
        if (const auto op_filename = io::file::yaml::exists_setting_file()) {
            if (const auto op_node = io::file::yaml::load(*op_filename)) {
                if (const auto op_select_node = io::file::yaml::get_by_width(*op_node, "name")) {
                    return *op_select_node;
                }
                else {
                    throw except::error("Required key does not exist in poac.yml");
                }
            }
            else {
                throw except::error("Could not load poac.yml");
            }
        }
        else {
            throw except::error("poac.yml does not exists");
        }
    }

    template <typename VS, typename = std::enable_if_t<std::is_rvalue_reference_v<VS&&>>>
    void operator()(VS&& argv) { _main(argv); }
    template <typename VS>
    void _main(VS&& argv) {
        namespace fs     = boost::filesystem;
        namespace except = core::exception;

        check_arguments(argv);
        const auto node = check_requirements();

        std::vector<std::string> program_args;
        // poac run -v -- -h build
        if (const auto result = std::find(argv.begin(), argv.end(), "--"); result != argv.end()) {
            // -h build
            program_args = std::vector<std::string>(result+1, argv.end());
            // -v
            subcmd::build{}(std::vector<std::string>(argv.begin(), result));
        }
        else {
            subcmd::build{}(std::move(argv));
        }

        const std::string project_name = node.at("name").as<std::string>();
        const std::string executable = fs::relative(io::file::path::current_build_bin_dir / project_name).string();
        util::command cmd(executable);
        for (const auto& s : program_args) {
            cmd += s;
        }

        std::cout << io::cli::green << "Running: " << io::cli::reset
                  << "`" + executable + "`"
                  << std::endl;
        if (const auto ret = cmd.run())
            std::cout << *ret;
        else // TODO: errorの時も文字列が欲しい．
            std::cout << project_name + " returned 1" << std::endl;
    }

    void check_arguments([[maybe_unused]] const std::vector<std::string>& argv) {
        namespace except = core::exception;

//        if (argv.size() >= 2)
//            throw exception::invalid_second_arg("run");
    }
};} // end namespace
#endif // !POAC_SUBCMD_RUN_HPP

#ifndef POAC_SUBCMD_RUN_HPP
#define POAC_SUBCMD_RUN_HPP

#include <iostream>
#include <string>
#include <regex>

#include <boost/filesystem.hpp>

#include "../core/exception.hpp"
#include "../io/file.hpp"
#include "../io/cli.hpp"
#include "../util/command.hpp"
#include "../core/naming.hpp"
#include "./build.hpp"


namespace poac::subcmd { struct run {
    static const std::string summary() { return "Build project and exec it."; }
    static const std::string options() { return "[-v | --verbose | -- [program args]]"; }


    template <typename VS, typename = std::enable_if_t<std::is_rvalue_reference_v<VS&&>>>
    void operator()(VS&& argv) { _main(std::move(argv)); }
    template <typename VS, typename = std::enable_if_t<std::is_rvalue_reference_v<VS&&>>>
    void _main(VS&& argv) {
        namespace fs     = boost::filesystem;
        namespace except = core::exception;

        check_arguments(argv);
        const auto node = io::file::yaml::load_config("name");

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
        const fs::path executable_path = fs::relative(io::file::path::current_build_bin_dir / project_name);
        if (!fs::exists(executable_path)) {
            return;
        }

        const std::string executable = executable_path.string();
        util::command cmd(executable);
        for (const auto& s : program_args) {
            cmd += s;
        }

        std::cout << io::cli::green << "Running: " << io::cli::reset
                  << "`" + executable + "`"
                  << std::endl;
        if (const auto ret = cmd.exec())
            std::cout << *ret;
        else
            std::cout << project_name + " returned 1" << std::endl;
    }

    void check_arguments([[maybe_unused]] const std::vector<std::string>& argv) {
        namespace except = core::exception;

//        if (argv.size() >= 2)
//            throw exception::invalid_second_arg("run");
    }
};} // end namespace
#endif // !POAC_SUBCMD_RUN_HPP

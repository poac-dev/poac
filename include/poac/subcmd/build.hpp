#ifndef POAC_SUBCMD_BUILD_HPP
#define POAC_SUBCMD_BUILD_HPP

#include <iostream>
#include <string>
#include <map>
#include <regex>
#include <stdexcept>

#include <boost/filesystem.hpp>
#include <boost/range/iterator_range.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>

#include "../core/exception.hpp"
#include "../io/file.hpp"
#include "../io/cli.hpp"
#include "../util/buildsystem.hpp"
#include "../util/package.hpp"
#include "../util/argparse.hpp"


namespace poac::subcmd { struct build {
    static const std::string summary() { return "Beta: Compile all sources that depend on this project."; }
    static const std::string options() { return "[-v | --verbose]"; } // TODO: --release


    template <typename VS, typename = std::enable_if_t<std::is_rvalue_reference_v<VS&&>>>
    void operator()(VS&& argv) { _main(std::move(argv)); }
    template <typename VS, typename = std::enable_if_t<std::is_rvalue_reference_v<VS&&>>>
    void _main(VS&& argv) {
        namespace fs     = boost::filesystem;
        namespace except = core::exception;

        check_arguments(argv);
        check_requirements();
        const auto node = io::file::yaml::load_setting_file("build");
        const bool verbose = util::argparse::use(argv, "-v", "--verbose");

        util::buildsystem bs;

        if (const auto bin = io::file::yaml::get_by_width(node.at("build"), "bin")) {
            if (const auto op_bin = io::file::yaml::get<bool>((*bin).at("bin"))) {
                if (*op_bin) {
                    if (build_bin(bs, true, verbose)) {}
                    else { /* compile or link error */ }
                }
            }
        }
        if (const auto lib = io::file::yaml::get_by_width(node.at("build"), "lib")) {
            if (const auto op_lib = io::file::yaml::get<bool>((*lib).at("lib"))) {
                if (*op_lib) {
                    if (build_link_libs(bs, verbose)) {}
                    else { /* compile or gen error */ }
                }
            }
        }
    }

    boost::optional<std::string> build_bin(
        util::buildsystem& bs,
        const bool usemain=false,
        const bool verbose=false )
    {
        namespace fs = boost::filesystem;

        bs.configure_compile(usemain, verbose);
        // Since the obj file already exists and has not been changed as a result
        //  of verification of the hash file, return only the list of existing obj_files
        //  and do not compile.
        // There is no necessity of linking that there is no change completely.
        if (bs.compile_conf.source_files.empty()) { // No need for compile and link
            const std::string bin_path =
                    (io::file::path::current_build_bin_dir / bs.project_name).string();
            // Dealing with an error which is said to have cache even though it is not going well.
            if (fs::exists(bin_path)) {
                std::cout << io::cli::yellow << "Warning: " << io::cli::reset
                          << "There is no change. Binary exists in `" +
                             fs::relative(bin_path).string() + "`."
                          << std::endl;
            }
            return bin_path;
        }
        else {
            if (const auto obj_files_path = bs._compile()) {
                bs.configure_link(*obj_files_path, verbose);
                if (const auto bin_path = bs._link()) {
                    std::cout << io::cli::green << "Compiled: " << io::cli::reset
                              << "Output to `" +
                                 fs::relative(*bin_path).string() +
                                 "`"
                              << std::endl;
                    return bin_path;
                }
                else { // Link failure
                    return boost::none;
                }
            }
            else { // Compile failure
                return boost::none;
            }
        }
    }

    boost::optional<std::string> build_link_libs(
        util::buildsystem& bs,
        const bool verbose = false )
    {
        namespace fs = boost::filesystem;

        bs.configure_compile(false, verbose);
        if (bs.compile_conf.source_files.empty()) { // No need for compile and link
            const std::string lib_path =
                    (io::file::path::current_build_lib_dir / bs.project_name).string();
            if (fs::exists(lib_path)) {
                std::cout << io::cli::yellow << "Warning: " << io::cli::reset
                          << "There is no change. Static link library exists in `" +
                             fs::relative(lib_path).string() +
                             ".a" + "`."
                          << std::endl;
                std::cout << io::cli::yellow << "Warning: " << io::cli::reset
                          << "There is no change. Dynamic link library exists in `" +
                             fs::relative(lib_path).string() +
                             ".dylib" + "`."
                          << std::endl;
            }
            return lib_path;
        }
        if (const auto obj_files_path = bs._compile()) {
            bs.configure_static_lib(*obj_files_path, verbose);
            if (const auto stlib_path = bs._gen_static_lib()) {
                std::cout << io::cli::green << "Generated: " << io::cli::reset
                          << "Output to `" +
                             fs::relative(*stlib_path).string() +
                             "`"
                          << std::endl;
//                return lib_path;
            }
            else { // Static link library generation failed
//                return boost::none;
            }

            bs.configure_dynamic_lib(*obj_files_path, verbose);
            if (const auto dylib_path = bs._gen_dynamic_lib()) {
                std::cout << io::cli::green << "Generated: " << io::cli::reset
                          << "Output to `" +
                             fs::relative(*dylib_path).string() +
                             "`"
                          << std::endl;
//                return lib_path;
            }
            else {
                // Dynamic link library generation failed
//                return boost::none;
            }

            // TODO:
            return boost::none;
        }
        else { // Compile failure
            return boost::none;
        }
    }


    void check_requirements() {
        namespace fs     = boost::filesystem;
        namespace except = core::exception;
    }

    void check_arguments(const std::vector<std::string>& argv) {
        namespace except = core::exception;

        if (argv.size() > 1)
            throw except::invalid_second_arg("build");
    }
};} // end namespace
#endif // !POAC_SUBCMD_BUILD_HPP

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

        const auto first = argv.begin(), last = argv.end();
        const bool verbose = (std::find(first, last, "-v") != last || std::find(first, last, "--verbose") != last);

        util::buildsystem bs;

        if (const auto bin = io::file::yaml::get_by_width(node.at("build"), "bin")) {
            if (const auto op_bin = io::file::yaml::get<bool>((*bin).at("bin"))) {
                if (*op_bin) {
                    if (bs.build_bin(true, verbose)) {}
                    else { /* compile or link error */ }
                }
            }
        }
        if (const auto lib = io::file::yaml::get_by_width(node.at("build"), "lib")) {
            if (const auto op_lib = io::file::yaml::get<bool>((*lib).at("lib"))) {
                if (*op_lib) {
                    if (bs.build_link_libs(verbose)) {}
                    else { /* compile or gen error */ }
                }
            }
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

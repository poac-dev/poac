#ifndef POAC_SUBCMD_BUILD_HPP
#define POAC_SUBCMD_BUILD_HPP

#include <iostream>
#include <string>
#include <regex>

#include <boost/filesystem.hpp>
#include <boost/range/iterator_range.hpp>

#include "../core/except.hpp"
#include "../io/file.hpp"


namespace poac::subcmd { struct build {
    static const std::string summary() { return "Beta: Compile all sources that depend on this project."; }
    static const std::string options() { return "<options>"; }

    template <typename VS>
    void operator()(VS&& argv) { _main(argv); }
    template <typename VS>
    void _main(VS&& argv) {
        namespace fs     = boost::filesystem;
        namespace except = core::except;

        check_arguments(argv);
    }

    void check_arguments(const std::vector<std::string>& argv) {
        namespace except = core::except;

        if (!argv.empty())
            throw except::invalid_second_arg("build");
    }
};} // end namespace
#endif // !POAC_SUBCMD_BUILD_HPP

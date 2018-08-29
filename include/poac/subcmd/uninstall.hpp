// Beta: Do not resolve dependencies
#ifndef POAC_SUBCMD_UNINSTALL_HPP
#define POAC_SUBCMD_UNINSTALL_HPP

#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <algorithm>

#include <boost/filesystem.hpp>
#include <boost/range/iterator_range.hpp>

#include "../core/exception.hpp"
#include "../io/file/path.hpp"
#include "../io/cli.hpp"


namespace poac::subcmd { struct uninstall {
    static const std::string summary() { return "Beta: Uninstall packages."; }
    static const std::string options() { return "[<pkg-names>]"; }
    const std::map<std::string, std::string> opts{
            { "-v", "--version" },
            { "", "--with-args" }
    };

    template <typename VS, typename = std::enable_if_t<std::is_rvalue_reference_v<VS&&>>>
    void operator()(VS&& argv) { _main(argv); }
    template <typename VS>
    void _main(VS&& argv) {
        namespace fs = boost::filesystem;
        // Check if the deps directory exists.
        // Perform dependency resolution.
        // Uninstall packages in serial or parallel to the deps directory.
        //   If there is no package, it displays an error.
        check_arguments(argv);

        for (const auto& v : argv) {
            const fs::path pkg = io::file::path::current_deps_dir / v;
            if (io::file::path::validate_dir(pkg))
                fs::remove_all(pkg);
            else
                std::cout << io::cli::red << v << " not found" << io::cli::reset << std::endl;
        }
    }

    void check_arguments(const std::vector<std::string>& argv) {
        namespace except = core::exception;
        if (argv.empty()) throw except::invalid_second_arg("uninstall");
    }
};} // end namespace
#endif // !POAC_SUBCMD_UNINSTALL_HPP

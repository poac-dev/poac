#ifndef POAC_OPTS_ROOT_HPP
#define POAC_OPTS_ROOT_HPP

#include <iostream>
#include <string>
#include <optional>

#include <boost/dll/runtime_symbol_info.hpp>

#include <poac/core/except.hpp>
#include <poac/util/termcolor2.hpp>

namespace poac::opts::root {
    constexpr auto summary = termcolor2::make_string("Display the root installation directory");
    constexpr auto options = termcolor2::make_string("<Nothing>");

    // Reference: https://www.boost.org/doc/libs/1_65_1/doc/html/boost/dll/program_location.html
    std::optional<core::except::Error>
    exec(const std::vector<std::string>&) {
        namespace fs = boost::filesystem;
        namespace except = core::except;

        boost::system::error_code error;
        const auto loc = boost::dll::program_location(error);
        if (error) {
            return except::Error::General{
                    "Could not get root installation directory"
            };
        }

        const auto ln = fs::read_symlink(loc, error);
        if (!error) {
            std::cout << ln.parent_path().string() << std::endl;
        }
        else {
            std::cout << loc.parent_path().string() << std::endl;
        }
        return std::nullopt;
    }
} // end namespace
#endif // !POAC_OPTS_ROOT_HPP

#ifndef POAC_OPTS_ROOT_HPP
#define POAC_OPTS_ROOT_HPP

#include <future>
#include <iostream>
#include <optional>
#include <string>
#include <vector>

#include <boost/dll/runtime_symbol_info.hpp>

#include <poac/core/except.hpp>
#include <poac/io/config.hpp>
#include <poac/io/path.hpp>

namespace poac::opts::root {
    const clap::subcommand cli =
            clap::subcommand("root")
                .about("Display the root installation directory")
            ;

    // Reference: https://www.boost.org/doc/libs/1_65_1/doc/html/boost/dll/program_location.html
    [[nodiscard]] std::optional<core::except::Error>
    exec(std::future<std::optional<io::config::Config>>&&, std::vector<std::string>&&) {
        boost::system::error_code ec;
        const auto loc = boost::dll::program_location(ec);
        if (ec) {
            return core::except::Error::General{
                "Could not get root installation directory"
            };
        }

        try {
            const auto ln = std::filesystem::read_symlink(loc);
            std::cout << ln.parent_path().string() << std::endl;
        } catch (...) {
            std::cout << loc.parent_path().string() << std::endl;
        }
        return std::nullopt;
    }
} // end namespace
#endif // !POAC_OPTS_ROOT_HPP

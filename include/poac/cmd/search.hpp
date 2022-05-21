#ifndef POAC_CMD_SEARCH_HPP
#define POAC_CMD_SEARCH_HPP

// std
#include <iostream>
#include <sstream>

// external
#include <boost/algorithm/string.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <spdlog/spdlog.h>
#include <structopt/app.hpp>

// internal
#include <poac/poac.hpp>
#include <poac/util/net.hpp>
#include <poac/util/pretty.hpp>
#include <poac/util/termcolor2/termcolor2.hpp>
#include <poac/util/verbosity.hpp>
#include <poac/config.hpp>

namespace poac::cmd::search {
    struct Options: structopt::sub_command {
        /// Package name to search
        String package_name;
    };

    class Error {
        template <thiserror::fixed_string S, class ...T>
        using error = thiserror::error<S, T...>;

    public:
        using NotFound =
            error<"No packages found for `{}`", String>;
    };

    [[nodiscard]] Result<void>
    search(const Options& opts) {
        const boost::property_tree::ptree pt =
            tryi(
                util::net::api::search(opts.package_name, 20)
                    .map_err(to_anyhow)
            );
        if (util::verbosity::is_verbose()) {
            boost::property_tree::json_parser::write_json(std::cout, pt);
        }

        const auto children = pt.get_child("data");
        if (children.empty()) {
            return Err<Error::NotFound>(opts.package_name);
        }
        for (const boost::property_tree::ptree::value_type& child : children) {
            const boost::property_tree::ptree& hits = child.second;
            const auto package = format(
                "{} = \"{}\"",
                hits.get<String>("name"),
                hits.get<String>("version")
            );

            String description = hits.get<String>("description");
            description = util::pretty::clip_string(description, 100);
            // If util::pretty::clip_string clips last \n, \n should be removed
            description.erase(std::remove(description.begin(), description.end(), '\n'), description.end());

            spdlog::info("{:<40}# {}", package, description);
        }
        return Ok();
    }

    [[nodiscard]] Result<void>
    exec(const Options& opts) {
        return search(opts);
    }
} // end namespace

STRUCTOPT(poac::cmd::search::Options, package_name);

#endif // !POAC_CMD_SEARCH_HPP

#ifndef POAC_CMD_SEARCH_HPP
#define POAC_CMD_SEARCH_HPP

// std
#include <iostream>
#include <string>
#include <string_view>
#include <sstream>

// external
#include <boost/algorithm/string.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <fmt/core.h>
#include <mitama/result/result.hpp>
#include <spdlog/spdlog.h>

// internal
#include <poac/util/net.hpp>
#include <poac/util/pretty.hpp>
#include <poac/util/termcolor2/termcolor2.hpp>
#include <poac/config.hpp>

namespace poac::cmd::search {
    struct Options {
        std::string package_name;
    };

    [[nodiscard]] mitama::result<void, std::string>
    search(Options&& opts) {
        const boost::property_tree::ptree pt =
            MITAMA_TRY(util::net::api::search(opts.package_name, 20));
        if (const auto nb_hits = pt.get_optional<int>("nbHits")) {
            if (nb_hits.value() <= 0) {
                return mitama::failure(
                    fmt::format(
                        "{} package is not found", opts.package_name
                    )
                );
            }
        }
        if (spdlog::should_log(spdlog::level::trace)) {
            boost::property_tree::json_parser::write_json(std::cout, pt);
        }

        for (const boost::property_tree::ptree::value_type& child : pt.get_child("hits")) {
            const boost::property_tree::ptree& hits = child.second;

            std::string name = hits.get<std::string>("_highlightResult.package.name.value");
            boost::replace_all(name, "<em>", termcolor2::green);
            boost::replace_all(name, "</em>", termcolor2::reset);
            const auto package = fmt::format(
                "{} = \"{}\"",
                name,
                hits.get<std::string>("package.version")
            );

            std::string description = hits.get<std::string>("package.description");
            description = util::pretty::clip_string(description, 100);
            // If util::pretty::clip_string clips last \n, \n should be removed
            description.erase(std::remove(description.begin(), description.end(), '\n'), description.end());

            spdlog::info("{:<40}# {}", package, description);
        }
        return mitama::success();
    }

    [[nodiscard]] mitama::result<void, std::string>
    exec(Options&& opts) {
        return search(std::move(opts));
    }
} // end namespace

#endif // !POAC_CMD_SEARCH_HPP

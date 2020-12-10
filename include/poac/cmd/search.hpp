#ifndef POAC_CMD_SEARCH_HPP
#define POAC_CMD_SEARCH_HPP

// std
#include <iostream>
#include <string>
#include <string_view>
#include <sstream>

// external
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <fmt/core.h>
#include <mitama/result/result.hpp>
#include <plog/Log.h>

// internal
#include <poac/io/net.hpp>
#include <poac/util/pretty.hpp>
#include <poac/util/termcolor2/termcolor2.hpp>
#include <poac/config.hpp>

namespace poac::cmd::search {
    struct Options {
        std::string package_name;
    };

    unsigned int
    replace(std::string& s, std::string_view from, std::string_view target) {
        const auto from_length = from.size();
        const auto target_length = target.size();
        std::size_t pos = 0;
        unsigned int count = 0;
        while ((pos = s.find(from, pos)) != std::string::npos) {
            s.replace(pos, from_length, target);
            pos += target_length;
            ++count;
        }
        return count;
    }

    [[nodiscard]] mitama::result<void, std::string>
    search(Options&& opts) {
        const boost::property_tree::ptree pt =
            MITAMA_TRY(io::net::api::search(opts.package_name, 20));
        if (const auto nb_hits = pt.get_optional<int>("nbHits")) {
            if (nb_hits.value() <= 0) {
                return mitama::failure(
                    fmt::format(
                        "{} package is not found", opts.package_name
                    )
                );
            }
        }

        IF_PLOG(plog::verbose) {
            boost::property_tree::json_parser::write_json(std::cout, pt);
        }

        for (const boost::property_tree::ptree::value_type& child : pt.get_child("hits")) {
            const boost::property_tree::ptree& hits = child.second;

            std::string name = hits.get<std::string>("_highlightResult.package.name.value");
            replace(name, "<em>", termcolor2::green.to_string());
            replace(name, "</em>", termcolor2::reset.to_string());
            const auto package = fmt::format(
                "{} = \"{}\"",
                name,
                hits.get<std::string>("package.version")
            );

            std::string description = hits.get<std::string>("package.description");
            description = util::pretty::clip_string(description, 100);
            // If util::pretty::clip_string clips last \n, \n should be removed
            description.erase(std::remove(description.begin(), description.end(), '\n'), description.end());

            PLOG_INFO << fmt::format("{:<40}# {}", package, description);
        }
        return mitama::success();
    }

    [[nodiscard]] mitama::result<void, std::string>
    exec(Options&& opts) {
        return search(std::move(opts));
    }
} // end namespace

#endif // !POAC_CMD_SEARCH_HPP

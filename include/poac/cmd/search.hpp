#ifndef POAC_CMD_SEARCH_HPP
#define POAC_CMD_SEARCH_HPP

// std
#include <iostream>
#include <string>
#include <string_view>
#include <sstream>
#include <optional>

// external
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <fmt/core.h>
#include <fmt/ostream.h>

// internal
#include <poac/core/except.hpp>
#include <poac/io/term.hpp>
#include <poac/io/net.hpp>
#include <poac/util/pretty.hpp>
#include <poac/util/termcolor2/termcolor2.hpp>

namespace poac::cmd::search {
    struct Options {
        bool verbose;
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

    boost::property_tree::ptree
    get_search_api(const std::string& query) {
        std::string params;
        {
            boost::property_tree::ptree pt;
            pt.put("params", "query=" + query + "&hitsPerPage=20");
            std::stringstream ss;
            boost::property_tree::json_parser::write_json(ss, pt);
            params = ss.str();
        }

        std::stringstream ss;
        io::net::Headers headers;
        headers.emplace("X-Algolia-API-Key", ALGOLIA_SEARCH_ONLY_KEY);
        headers.emplace("X-Algolia-Application-Id", ALGOLIA_APPLICATION_ID);
        const io::net::requests req{ ALGOLIA_SEARCH_INDEX_API_HOST };
        const auto res = req.post(ALGOLIA_SEARCH_INDEX_API, params, headers);
        ss << res.data();

        boost::property_tree::ptree pt;
        boost::property_tree::json_parser::read_json(ss, pt);
        if (const auto nb_hits = pt.get_optional<int>("nbHits")) {
            if (nb_hits.value() <= 0) {
                throw core::except::error(
                        fmt::format("{} not found", query)
                );
            }
        }
        return pt;
    }

    [[nodiscard]] std::optional<core::except::Error>
    search(Options&& opts) {
        const auto pt = get_search_api(opts.package_name);
        if (opts.verbose) {
            std::stringstream ss;
            boost::property_tree::json_parser::write_json(ss, pt);
            std::cout << ss.str() << std::endl;
        }

        for (const boost::property_tree::ptree::value_type& child : pt.get_child("hits")) {
            const boost::property_tree::ptree& hits = child.second;

            std::string name = hits.get<std::string>("_highlightResult.package.name.value");
            replace(name, "<em>", termcolor2::green.to_string());
            replace(name, "</em>", termcolor2::reset.to_string());
            const std::string version = hits.get<std::string>("package.version");
            const auto package = fmt::format("{} = \"{}\"", name, version);

            std::string description = hits.get<std::string>("package.description");
            description = util::pretty::clip_string(description, 100);
            // If util::pretty::clip_string clips last \n, \n should add at last
            description.find('\n') == std::string::npos ? description += '\n' : "";

            fmt::print("{:<40}# {}", package, description);
        }
        return std::nullopt;
    }

    [[nodiscard]] std::optional<core::except::Error>
    exec(Options&& opts) {
        return search(std::move(opts));
    }
} // end namespace

#endif // !POAC_CMD_SEARCH_HPP

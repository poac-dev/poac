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
#include <mitama/result/result.hpp>

// internal
#include <poac/io/term.hpp>
#include <poac/io/net.hpp>
#include <poac/util/pretty.hpp>
#include <poac/util/termcolor2/termcolor2.hpp>
#include <poac/config.hpp>

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

    std::string
    request_body(const std::string& query) {
        boost::property_tree::ptree pt;
        pt.put("params", "query=" + query + "&hitsPerPage=20");
        std::stringstream body;
        boost::property_tree::json_parser::write_json(body, pt);
        return body.str();
    }

    [[nodiscard]] mitama::result<boost::property_tree::ptree, std::string>
    get_search_api(const std::string& query) {
        const io::net::requests request{ ALGOLIA_SEARCH_INDEX_API_HOST };
        io::net::Headers headers;
        headers.emplace("X-Algolia-API-Key", ALGOLIA_SEARCH_ONLY_KEY);
        headers.emplace("X-Algolia-Application-Id", ALGOLIA_APPLICATION_ID);
        const auto response = request.post(ALGOLIA_SEARCH_INDEX_API, request_body(query), headers);
        std::stringstream response_body;
        response_body << response.data();

        boost::property_tree::ptree pt;
        boost::property_tree::json_parser::read_json(response_body, pt);
        if (const auto nb_hits = pt.get_optional<int>("nbHits")) {
            if (nb_hits.value() <= 0) {
                return mitama::failure(
                    fmt::format("{} package not found", query)
                );
            }
        }
        return mitama::success(pt);
    }

    [[nodiscard]] mitama::result<void, std::string>
    search(Options&& opts) {
        const boost::property_tree::ptree pt = MITAMA_TRY(get_search_api(opts.package_name));
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
        return mitama::success();
    }

    [[nodiscard]] mitama::result<void, std::string>
    exec(Options&& opts) {
        return search(std::move(opts));
    }
} // end namespace

#endif // !POAC_CMD_SEARCH_HPP

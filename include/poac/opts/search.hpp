#ifndef POAC_OPTS_SEARCH_HPP
#define POAC_OPTS_SEARCH_HPP

#include <future>
#include <iostream>
#include <string>
#include <string_view>
#include <sstream>
#include <optional>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include <poac/core/except.hpp>
#include <poac/io/term.hpp>
#include <poac/io/net.hpp>
#include <poac/util/argparse.hpp>
#include <poac/util/pretty.hpp>
#include <poac/util/termcolor2/termcolor2.hpp>

namespace poac::opts::search {
    const clap::subcommand cli =
            clap::subcommand("search")
                .about("Search for packages in poac.pm")
                .arg(clap::opt("verbose", "Use verbose output").short_("v"))
                .arg(clap::arg("<pkg-name>"))
            ;

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

    void echo_title_line() {
        std::cout << termcolor2::underline;
        io::term::set_left(27);
        std::cout << "Package";
        io::term::set_left(50);
        std::cout << "| Description";
        io::term::set_left(10);
        std::cout << "| Version"
                  << "| C++ "
                  << termcolor2::reset
                  << std::endl;
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
                        core::except::msg::not_found(query));
            }
        }
        return pt;
    }

    [[nodiscard]] std::optional<core::except::Error>
    search(search::Options&& opts) {
        const auto pt = get_search_api(opts.package_name);
        if (opts.verbose) {
            std::stringstream ss;
            boost::property_tree::json_parser::write_json(ss, pt);
            std::cout << ss.str() << std::endl;
        }

        echo_title_line();
        for (const boost::property_tree::ptree::value_type& child : pt.get_child("hits")) {
            const boost::property_tree::ptree& hits = child.second;

            std::string owner = hits.get<std::string>("_highlightResult.owner.value");
            auto owner_count = replace(owner, "<em>", termcolor2::red.to_string()) * termcolor2::red.size();
            owner_count += replace(owner, "</em>", termcolor2::reset.to_string()) * termcolor2::reset.size();

            std::string repo = hits.get<std::string>("_highlightResult.repo.value");
            auto repo_count = replace(repo, "<em>", termcolor2::red.to_string()) * termcolor2::red.size();
            repo_count += replace(repo, "</em>", termcolor2::reset.to_string()) * termcolor2::reset.size();

            io::term::set_left(27 + owner_count + repo_count);
            std::cout << util::pretty::clip_string(owner + "/" + repo, 23 + owner_count + repo_count);

            io::term::set_left(50);
            std::cout << "| " + util::pretty::clip_string(hits.get<std::string>("description"), 45);

            io::term::set_left(10);
            std::cout << "|  " + hits.get<std::string>("version");

            const auto cpp_version = hits.get<std::string>("cpp_version"); // TODO: cpp-version
            std::cout << "| " << (cpp_version == "3" ? "03" : cpp_version)
                      << std::endl;
        }
        return std::nullopt;
    }

    [[nodiscard]] std::optional<core::except::Error>
    exec(std::future<std::optional<io::config::Config>>&&, std::vector<std::string>&& args) {
        if (args.size() != 1) {
            return core::except::Error::InvalidSecondArg::Search;
        }
        search::Options opts{};
        opts.verbose = util::argparse::use_rm(args, "-v", "--verbose");
        opts.package_name = args[0];
        return search::search(std::move(opts));
    }
} // end namespace
#endif // !POAC_OPTS_SEARCH_HPP

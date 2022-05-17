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
#include <mitama/anyhow/anyhow.hpp>
#include <mitama/thiserror/thiserror.hpp>
#include <spdlog/spdlog.h>
#include <structopt/app.hpp>

// internal
#include <poac/util/net.hpp>
#include <poac/util/pretty.hpp>
#include <poac/util/termcolor2/termcolor2.hpp>
#include <poac/util/verbosity.hpp>
#include <poac/config.hpp>

namespace poac::cmd::search {
    namespace anyhow = mitama::anyhow;
    namespace thiserror = mitama::thiserror;

    struct Options: structopt::sub_command {
        /// Package name to search
        std::string package_name;
    };

    class Error {
        template <thiserror::fixed_string S, class ...T>
        using error = thiserror::error<S, T...>;

    public:
        using NotFound =
            error<"No packages found for `{0}`", std::string>;
    };

    [[nodiscard]] anyhow::result<void>
    search(const Options& opts) {
        const boost::property_tree::ptree pt =
            MITAMA_TRY(
                util::net::api::search(opts.package_name, 20)
                    .map_err([](const std::string& e){ return anyhow::anyhow(e); })
            );
        if (util::verbosity::is_verbose()) {
            boost::property_tree::json_parser::write_json(std::cout, pt);
        }

        const auto children = pt.get_child("data");
        if (children.empty()) {
            return anyhow::failure<Error::NotFound>(opts.package_name);
        }
        for (const boost::property_tree::ptree::value_type& child : children) {
            const boost::property_tree::ptree& hits = child.second;
            const auto package = fmt::format(
                "{} = \"{}\"",
                hits.get<std::string>("name"),
                hits.get<std::string>("version")
            );

            std::string description = hits.get<std::string>("description");
            description = util::pretty::clip_string(description, 100);
            // If util::pretty::clip_string clips last \n, \n should be removed
            description.erase(std::remove(description.begin(), description.end(), '\n'), description.end());

            spdlog::info("{:<40}# {}", package, description);
        }
        return mitama::success();
    }

    [[nodiscard]] anyhow::result<void>
    exec(const Options& opts) {
        return search(opts);
    }
} // end namespace

STRUCTOPT(poac::cmd::search::Options, package_name);

#endif // !POAC_CMD_SEARCH_HPP

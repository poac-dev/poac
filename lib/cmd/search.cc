// std
#include <algorithm> // std::remove

// external
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <spdlog/spdlog.h> // NOLINT(build/include_order)

// internal
#include "poac/cmd/search.hpp"
#include "poac/util/net.hpp"
#include "poac/util/pretty.hpp"
#include "poac/util/verbosity.hpp"

namespace poac::cmd::search {

[[nodiscard]] Fn search(const Options& opts)->Result<void> {
  const boost::property_tree::ptree pt =
      Try(util::net::api::search(opts.package_name, 20).map_err(to_anyhow));
  if (util::verbosity::is_verbose()) {
    boost::property_tree::json_parser::write_json(std::cout, pt);
  }

  Let children = pt.get_child("data.results");
  if (children.empty()) {
    return Err<NotFound>(opts.package_name);
  }

  constexpr i32 MAX_COLUMNS = 10; // limit the number of columns
  i32 i = 1;
  for (const boost::property_tree::ptree::value_type& child : children) {
    if (i > MAX_COLUMNS) {
      break;
    }

    const boost::property_tree::ptree& hits = child.second;
    const String package = format(
        "{} = \"{}\"", hits.get<String>("name"), hits.get<String>("version")
    );

    LetMut description = hits.get<String>("description");
    description = util::pretty::clip_string(description, 100);
    // If util::pretty::clip_string clips last \n, \n should be removed
    description.erase(
        std::remove(description.begin(), description.end(), '\n'),
        description.end()
    );

    spdlog::info("{:<40}# {}", package, description);
    ++i;
  }
  return Ok();
}

} // namespace poac::cmd::search

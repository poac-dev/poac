#ifndef POAC_OPTS_GRAPH_HPP
#define POAC_OPTS_GRAPH_HPP

#include <iostream>
#include <fstream>
#include <algorithm>
#include <iterator>
#include <utility>
#include <vector>
#include <string>
#include <optional>

#include <boost/filesystem.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_utility.hpp>
#include <boost/graph/graphviz.hpp>
#include <boost/range/iterator_range_core.hpp>
#include <boost/range/adaptor/indexed.hpp>

#include <poac/core/except.hpp>
#include <poac/core/resolver/lock.hpp>
#include <poac/core/resolver/resolve.hpp>
#include <poac/io/yaml.hpp>
#include <poac/io/term.hpp>
#include <poac/util/argparse.hpp>
#include <poac/util/shell.hpp>
#include <poac/util/termcolor2.hpp>

namespace poac::opts::graph {
    constexpr auto summary = termcolor2::make_string("Create a dependency graph");
    constexpr auto options = termcolor2::make_string("[-o | --output]");

    struct Options {
        std::optional<boost::filesystem::path> output_file;
    };

    struct Vertex {
        std::string name;
        std::string version;
    };
    using Graph = boost::adjacency_list<boost::listS, boost::vecS, boost::directedS, Vertex>;

    core::resolver::resolve::Resolved
    create_resolved_deps(std::optional<io::yaml::Config>&& config) {
        namespace lock = core::resolver::lock;
        namespace resolver = core::resolver::resolve;
        namespace yaml = io::yaml;

        if (!config->dependencies) {
            throw core::except::error(
                    core::except::msg::key_does_not_exist("dependencies"));
        }
        const auto deps_node = config->dependencies.value();

        // create resolved deps
        resolver::Resolved resolved_deps{};
        if (const auto locked_deps = lock::load()) {
            resolved_deps = *locked_deps;
        } else { // poac.lock does not exist
            const resolver::Deps deps = install::resolve_packages(deps_node);
            resolved_deps = resolver::resolve(deps);
        }
        return resolved_deps;
    }

    std::pair<Graph, std::vector<std::string>>
    create_graph(std::optional<io::yaml::Config>&& config) {
        const auto resolved_deps = create_resolved_deps(std::move(config));
        Graph g;

        // Add vertex
        std::vector<Graph::vertex_descriptor> desc;
        for (const auto& dep : resolved_deps.activated | boost::adaptors::indexed()) {
            desc.push_back(boost::add_vertex(g));
            g[dep.index()].name = dep.value().name;
            g[dep.index()].version = dep.value().version;
        }
        // Add edge
        for (const auto& dep : resolved_deps.activated | boost::adaptors::indexed()) {
            if (!dep.value().deps.empty()) {
                for (const auto& d : dep.value().deps) {
                    const auto result = std::find(resolved_deps.activated.begin(), resolved_deps.activated.end(), d);
                    if (result != resolved_deps.activated.end()) {
                        const auto index = std::distance(resolved_deps.activated.begin(), result);
                        boost::add_edge(desc[dep.index()], desc[index], g);
                    }
                }
            }
        }

        std::vector<std::string> names;
        for (const auto& dep : resolved_deps.activated) {
            names.push_back(dep.name + ": " + dep.version);
        }
        return { g, names };
    }

    [[nodiscard]] std::optional<core::except::Error>
    dot_file_output(std::optional<io::yaml::Config>&& config, graph::Options&& opts) {
        const auto [g, names] = create_graph(std::move(config));
        std::ofstream file(opts.output_file->string());
        boost::write_graphviz(file, g, boost::make_label_writer(&names[0]));
        io::term::status_done();
        return std::nullopt;
    }

    [[nodiscard]] std::optional<core::except::Error>
    png_file_output(std::optional<io::yaml::Config>&& config, graph::Options&& opts) {
        if (util::_shell::has_command("dot")) {
            const auto [g, names] = create_graph(std::move(config));

            const std::string file_dot = opts.output_file->stem().string() + ".dot";
            std::ofstream file(file_dot);
            boost::write_graphviz(file, g, boost::make_label_writer(&names[0]));

            util::shell("dot -Tpng " + file_dot + " -o " + opts.output_file->string()).exec();
            boost::filesystem::remove(file_dot);
            io::term::status_done();
            return std::nullopt;
        } else {
            return core::except::Error::General{
                    "To output with .png you need to install the graphviz.\n"
                    "Or please consider outputting in .dot format."
            };
        }
    }

    [[nodiscard]] std::optional<core::except::Error>
    file_output(std::optional<io::yaml::Config>&& config, graph::Options&& opts) {
        if (opts.output_file->extension() == ".png") {
            return png_file_output(std::move(config), std::move(opts));
        } else if (opts.output_file->extension() == ".dot") {
            return dot_file_output(std::move(config), std::move(opts));
        } else {
            return core::except::Error::General{
                    "The extension of the output file must be .dot or .png."
            };
        }
    }

    [[nodiscard]] std::optional<core::except::Error>
    console_output(std::optional<io::yaml::Config>&& config) {
        const auto [g, names] = create_graph(std::move(config));
        (void)names; // error: unused variable
        boost::graph_traits<Graph>::edge_iterator itr, end;
        for (tie(itr, end) = edges(g); itr != end; ++itr) {
            std::cout << boost::get(&Vertex::name, g)[source(*itr, g)] << " -> "
                      << boost::get(&Vertex::name, g)[target(*itr, g)] << '\n';
        }
        return std::nullopt;
    }

    [[nodiscard]] std::optional<core::except::Error>
    graph(std::optional<io::yaml::Config>&& config, graph::Options&& opts) {
        if (opts.output_file) {
            return file_output(std::move(config), std::move(opts));
        } else {
            return console_output(std::move(config));
        }
        return std::nullopt;
    }

    [[nodiscard]] std::optional<core::except::Error>
    exec(std::optional<io::yaml::Config>&& config, std::vector<std::string>&& args) {
        graph::Options opts{};
        opts.output_file = util::argparse::use_get(args, "-o", "--output");
        return graph::graph(std::move(config), std::move(opts));
    }
} // end namespace
#endif // !POAC_OPTS_GRAPH_HPP

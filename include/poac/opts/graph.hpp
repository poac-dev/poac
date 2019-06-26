#ifndef POAC_OPTS_GRAPH_HPP
#define POAC_OPTS_GRAPH_HPP

#include <iostream>
#include <fstream>
#include <algorithm>
#include <iterator>
#include <utility>
#include <vector>
#include <string>
#include <cstdlib>

#include <boost/filesystem.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_utility.hpp>
#include <boost/graph/graphviz.hpp>
#include <boost/range/iterator_range_core.hpp>
#include <boost/range/adaptor/indexed.hpp>

#include "../core/except.hpp"
#include "../core/resolver/lock.hpp"
#include "../core/resolver/resolve.hpp"
#include "../io/yaml.hpp"
#include "../io/cli.hpp"
#include "../util/argparse.hpp"
#include "../util/shell.hpp"


// TODO: --input, -iで，入力する，poac.ymlファイルを指定. 指定しない場合はカレントディレクトリのを選択
// poac graph -i ./deps/boost/poac.yml -o hoge.png
// TODO: 標準出力にdotをだせるようにする．
// TODO: poac graph | dot -Gsplines=ortho -Earrowhead=open -Earrowsize=0.5 -Tpng -Ograph.png

// TODO: ついでにlockファイルも作成しておく -> -iでymlを指定指定している場合は，lockファイルを生成しない

namespace poac::opts {
    namespace _graph {
        struct Vertex {
            std::string name;
            std::string version;
        };
        using Graph = boost::adjacency_list<boost::listS, boost::vecS, boost::directedS, Vertex>;


        core::resolver::resolve::Resolved create_resolved_deps() {
            namespace lock = core::resolver::lock;
            namespace resolver = core::resolver::resolve;
            namespace except = core::except;
            namespace yaml = io::yaml;

            // FIXME: uninstall.hppに同じのがある
            auto node = yaml::load_config();
            std::map<std::string, YAML::Node> deps_node;
            if (const auto deps_map = yaml::get<std::map<std::string, YAML::Node>>(node["deps"])) {
                deps_node = *deps_map;
            }
            else {
                throw except::error(except::msg::could_not_read("deps in poac.yml"));
            }

            // create resolved deps
            resolver::Resolved resolved_deps{};
            if (const auto locked_deps = lock::load()) {
                resolved_deps = *locked_deps;
            }
            else { // poac.lock does not exist
                const resolver::Deps deps = _install::resolve_packages(deps_node);
                resolved_deps = resolver::resolve(deps);
            }
            return resolved_deps;
        }

        std::pair<Graph, std::vector<std::string>>
        create_graph() {
            const auto resolved_deps = create_resolved_deps();

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

        template<typename VS>
        int _main(VS&& argv) {
            namespace fs = boost::filesystem;
            namespace except = core::except;

            if (const auto output_op = util::argparse::use_get(argv, "-o", "--output")) {
                fs::path output = *output_op;
                if (output.extension() == ".png") {
                    if (util::_shell::has_command("dot")) {
                        const auto [g, names] = create_graph();

                        const std::string file_dot = output.stem().string() + ".dot";
                        std::ofstream file(file_dot);
                        boost::write_graphviz(file, g, boost::make_label_writer(&names[0]));

                        util::shell("dot -Tpng " + file_dot + " -o " + output.string()).exec();
                        fs::remove(file_dot);

                        io::cli::status_done();
                    }
                    else {
                        throw except::error(
                                "To output with .png you need to install the graphviz.\n"
                                "Or please consider outputting in .dot format.");
                    }
                }
                else if (output.extension() == ".dot") {
                    const auto [g, names] = create_graph();
                    std::ofstream file(output.string());
                    boost::write_graphviz(file, g, boost::make_label_writer(&names[0]));
                    io::cli::status_done();
                }
                else {
                    throw except::error(
                            "The extension of the output file must be .dot or .png.");
                }
            }
            else {
                const auto [g, names] = create_graph();
                (void)names; // error: unused variable
                boost::graph_traits<Graph>::edge_iterator itr, end;
                for(tie(itr, end) = edges(g); itr != end; ++itr) {
                    std::cout << boost::get(&Vertex::name, g)[source(*itr, g)] << " -> "
                              << boost::get(&Vertex::name, g)[target(*itr, g)] << '\n';
                }
            }
            return EXIT_SUCCESS;
        }
    }

    struct graph {
        static std::string summary() { return "Create a dependency graph"; }
        static std::string options() { return "[-o | --output]"; }
        template <typename VS>
        int operator()(VS&& argv) {
            return _graph::_main(std::forward<VS>(argv));
        }
    };
} // end namespace
#endif // !POAC_OPTS_GRAPH_HPP

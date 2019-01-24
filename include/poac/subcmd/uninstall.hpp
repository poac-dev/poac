#ifndef POAC_SUBCMD_UNINSTALL_HPP
#define POAC_SUBCMD_UNINSTALL_HPP

#include <iostream>
#include <vector>
#include <string>
#include <string_view>
#include <map>
#include <algorithm>

#include <boost/filesystem.hpp>

#include "./install.hpp"
#include "../core/exception.hpp"
#include "../core/resolver.hpp"
#include "../core/naming.hpp"
#include "../io/file/path.hpp"
#include "../io/file/yaml.hpp"
#include "../io/cli.hpp"
#include "../util/argparse.hpp"


namespace poac::subcmd {
    namespace _uninstall {
        template <typename VS, typename=std::enable_if_t<std::is_rvalue_reference_v<VS&&>>>
        void all(VS&& argv) {
            namespace fs = boost::filesystem;

            if (!util::argparse::use(argv, "-y", "--yes")) {
                std::cout << "Are you sure delete all packages? [Y/n] ";
                std::string yes_or_no;
                std::cin >> yes_or_no;
                std::transform(yes_or_no.begin(), yes_or_no.end(), yes_or_no.begin(), ::tolower);
                if (!(yes_or_no == "yes" || yes_or_no == "y")) {
                    std::cout << "canceled." << std::endl;
                    return;
                }
            }
            fs::remove_all(io::file::path::current_deps_dir);
        }


        template <typename SingleRangePass, typename T>
        void check_exist_name(const SingleRangePass& rng, const T& argv) {
            const auto first = std::begin(rng);
            const auto last = std::end(rng);
            for (const auto& v : argv) {
                const auto result = std::find_if(first, last, [&](auto x){ return v == x.first; });
                if (result == last) {
                    throw core::exception::error("There is no package named " + v + " in the dependency.");
                }
            }
        }

        core::resolver::Resolved
        lock_to_resolved(const std::map<std::string, YAML::Node>& locked_deps) {
            namespace yaml = io::file::yaml;
            namespace resolver = core::resolver;

            resolver::Resolved resolved_deps{};
            for (const auto& [name, next_node] : locked_deps) {
                const auto version = *yaml::get<std::string>(next_node, "version");
                const auto source = *yaml::get<std::string>(next_node, "source");
                // dependenciesも読む -> 順番に削除していく必要があるためと，対象でないパッケージが依存していることを防ぐため
                if (const auto deps_deps = yaml::get<std::map<std::string, YAML::Node>>(next_node, "dependencies")) {
                    std::vector<resolver::Package> deps;
                    for (const auto& [name2, next_node2] : *deps_deps) {
                        const auto version2 = *yaml::get<std::string>(next_node2, "version");
                        const auto source2 = *yaml::get<std::string>(next_node2, "source");
                        deps.push_back({ name2, version2, source2, {} });
                    }
                    resolved_deps.activated.push_back({ name, version, source, deps });
                }
                else {
                    resolved_deps.activated.push_back({ name, version, source, {} });
                }
                resolved_deps.backtracked[name] = { version, source };
            }
            return resolved_deps;
        }


        template <typename InputIterator, typename Backtracked>
        void create_uninstall_list(
                InputIterator first,
                InputIterator last,
                std::string_view target_name,
                Backtracked& uninstall_list)
        {
            namespace cli = io::cli;

            // 同じ名前で複数のバージョンは存在しないことが，
            // resolved_deps = lock_to_resolved(*locked_deps);
            // と，
            // const resolver::Deps deps = _install::resolve_packages(deps_node);
            // resolved_deps = resolver::resolve(deps);
            // の両方で保証されているため，同一の名前を検索するだけでパッケージを一意に特定できる．(versionの比較が不要)
            const auto target = std::find_if(first, last, [&](auto x){ return x.name == target_name; });
            if (target == last) { // 同じパッケージ名を二つ以上いれた
                return;
            }

            // 他に依存されているパッケージは削除しない
            if (!target->deps.empty()) {
                for (auto itr = first; itr != last; ++itr) {
                    // uninstall_listに入ってない
                    const auto found1 = std::find_if(uninstall_list.begin(), uninstall_list.end(),
                            [&](auto x){ return x.first == itr->name; });
                    // targetのdepsに入っていない
                    const auto found2 = std::find_if(target->deps.begin(), target->deps.end(),
                            [&](auto x) { return x.name == itr->name; });
                    if (found1 == uninstall_list.end() && found2 == target->deps.end()) {
                        for (const auto& deps : itr->deps) {
                            // 他のパッケージに依存されている
                            if (target->name == deps.name) {
                                const auto warn = deps.name + ": " + deps.version +
                                                  " can not be deleted because " +
                                                  itr->name + ": " + itr->version +
                                                  " depends on it";
                                cli::echo(cli::to_warning(warn));
                                return;
                            }
                        }
                    }
                }
            }

            // 循環していないかチェック
            const auto cycle_check = std::find_if(uninstall_list.begin(), uninstall_list.end(),
                    [&](auto x){ return x.first == target->name; });
            if (cycle_check == uninstall_list.end()) {
                // ここまでたどり着いたなら，他に依存されていないということなので，削除リストに加える
                uninstall_list[target->name] = { target->version, target->source };
                // さらにそれの依存も，削除リストに加える
                for (const auto& td : target->deps) {
                    create_uninstall_list(first, last, td.name, uninstall_list);
                }
            }
        }

        template <typename VS, typename=std::enable_if_t<std::is_rvalue_reference_v<VS&&>>>
        void individual(VS&& argv) {
            namespace fs = boost::filesystem;
            namespace yaml = io::file::yaml;
            namespace resolver = core::resolver;
            namespace cli = io::cli;
            namespace naming = core::naming;


            const auto deps_node = yaml::load_config("deps").at("deps");
            const auto deps_map = deps_node.as<std::map<std::string, YAML::Node>>();
            check_exist_name(deps_map, argv);

            // create resolved deps
            resolver::Resolved resolved_deps{};
            if (const auto locked_deps = _install::load_locked_deps(_install::get_yaml_timestamp())) {
                resolved_deps = lock_to_resolved(*locked_deps);
            }
            else { // poac.lock does not exist
                const resolver::Deps deps = _install::resolve_packages(deps_node);
                resolved_deps = resolver::resolve(deps);
            }

            // create uninstall list
            cli::echo();
            resolver::Backtracked uninstall_list{};
            const auto first = resolved_deps.activated.begin();
            const auto last = resolved_deps.activated.end();
            for (const auto& v : argv) {
                create_uninstall_list(first, last, v, uninstall_list);
            }

            // confirm
            if (!util::argparse::use_rm(argv, "-y", "--yes")) {
                for (const auto& [name, dep] : uninstall_list) {
                    std::cout << name << ": " << dep.version << std::endl;
                }
                cli::echo();
                std::cout << "Are you sure delete above packages? [Y/n] ";
                std::string yes_or_no;
                std::cin >> yes_or_no;
                std::transform(yes_or_no.begin(), yes_or_no.end(), yes_or_no.begin(), ::tolower);
                if (!(yes_or_no == "yes" || yes_or_no == "y")) {
                    std::cout << "canceled." << std::endl;
                    return;
                }
            }

            // Delete what was added to uninstall_list
            cli::echo();
            for (const auto& [name, dep] : uninstall_list) {
                const auto package_name = naming::to_current(dep.source, name, dep.version);
                const auto package_path = io::file::path::current_deps_dir / package_name;
                if (io::file::path::validate_dir(package_path)) {
                    fs::remove_all(package_path);
                    std::cout << name << " is deleted" << std::endl;
                }
                else {
                    std::cout << io::cli::to_red(name + " is not found") << std::endl;
                }
            }

            // TODO: lockファイルとymlファイルを書き換える

            cli::echo();
            cli::echo(cli::status_done());
        }

        template <typename VS, typename=std::enable_if_t<std::is_rvalue_reference_v<VS&&>>>
        void _main(VS&& argv) {
            namespace fs = boost::filesystem;
            namespace except = core::exception;
            namespace yaml = io::file::yaml;
            namespace resolver = core::resolver;
            namespace cli = io::cli;
            namespace naming = core::naming;

            if (util::argparse::use(argv, "-a", "--all")) {
                all(std::move(argv));
            }
            else {
                individual(std::move(argv));
            }
        }

        void check_arguments(const std::vector<std::string>& argv) {
            namespace except = core::exception;
            if (argv.empty()) {
                throw except::invalid_second_arg("uninstall");
            }
        }
    }

    struct uninstall {
        static const std::string summary() { return "Uninstall packages"; }
        static const std::string options() { return "[<pkg-names>, -a | --all, -y | --yes]"; }
        template <typename VS, typename=std::enable_if_t<std::is_rvalue_reference_v<VS&&>>>
        void operator()(VS&& argv) {
            _uninstall::check_arguments(argv);
            _uninstall::_main(std::move(argv));
        }
    };
} // end namespace
#endif // !POAC_SUBCMD_UNINSTALL_HPP

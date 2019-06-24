#ifndef POAC_OPTS_UNINSTALL_HPP
#define POAC_OPTS_UNINSTALL_HPP

#include <iostream>
#include <vector>
#include <string>
#include <string_view>
#include <map>
#include <algorithm>
#include <cstdlib>

#include <boost/filesystem.hpp>

#include "./install.hpp"
#include "../core/except.hpp"
#include "../core/name.hpp"
#include "../core/deper/resolver.hpp"
#include "../core/deper/lock.hpp"
#include "../io/path.hpp"
#include "../io/yaml.hpp"
#include "../io/cli.hpp"
#include "../util/argparse.hpp"
#include "../util/termcolor2.hpp"


namespace poac::opts {
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
            fs::remove_all(io::path::current_deps_dir);
        }


        template <typename SingleRangePass, typename T>
        void check_exist_name(const SingleRangePass& rng, const T& argv) {
            const auto first = std::begin(rng);
            const auto last = std::end(rng);
            for (const auto& v : argv) {
                const auto result = std::find_if(first, last, [&](auto x){ return v == x.first; });
                if (result == last) {
                    throw core::except::error("There is no package named ", v, " in the dependency.");
                }
            }
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
                                std::cout << cli::warning << warn << std::endl;
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
                uninstall_list[target->name] = { {target->version}, {target->source} };
                // さらにそれの依存も，削除リストに加える
                for (const auto& td : target->deps) {
                    create_uninstall_list(first, last, td.name, uninstall_list);
                }
            }
        }

        template <typename VS, typename=std::enable_if_t<std::is_rvalue_reference_v<VS&&>>>
        void individual(VS&& argv) {
            namespace fs = boost::filesystem;
            namespace yaml = io::yaml;
            namespace resolver = core::deper::resolver;
            namespace cli = io::cli;
            namespace name = core::name;
            namespace except = core::except;
            namespace lock = core::deper::lock;
            using termcolor2::color_literals::operator""_red;

            auto node = yaml::load_config();
            std::map<std::string, YAML::Node> deps_node;
            if (const auto deps_map = yaml::get<std::map<std::string, YAML::Node>>(node["deps"])) {
                deps_node = *deps_map;
                check_exist_name(deps_node, argv);
            }
            else {
                throw except::error(except::msg::could_not_read("deps in poac.yml"));
            }

            // create resolved deps
            const auto timestamp = yaml::get_timestamp();
            resolver::Resolved resolved_deps{};
            if (const auto locked_deps = lock::load(timestamp)) {
                resolved_deps = *locked_deps;
            }
            else { // poac.lock does not exist
                const resolver::Deps deps = _install::resolve_packages(deps_node);
                resolved_deps = resolver::resolve(deps);
            }

            // create uninstall list
            std::cout << std::endl;
            resolver::Backtracked uninstall_list{};
            const auto first = resolved_deps.activated.begin();
            const auto last = resolved_deps.activated.end();
            for (const auto& v : argv) {
                create_uninstall_list(first, last, v, uninstall_list);
            }

            // Omit package that does not already exist

            // confirm
            if (!util::argparse::use_rm(argv, "-y", "--yes")) {
                for (const auto& [name, dep] : uninstall_list) {
                    std::cout << name << ": " << dep.version << std::endl;
                }
                std::cout << std::endl;
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
            std::cout << std::endl;
            for (const auto& [name, dep] : uninstall_list) {
                const auto package_name = name::to_current(dep.source, name, dep.version);
                const auto package_path = io::path::current_deps_dir / package_name;
                if (io::path::validate_dir(package_path)) {
                    fs::remove_all(package_path);
                    std::cout << name << " is deleted" << std::endl;
                }
                else {
                    std::cout << name << " is not found"_red << std::endl;
                }
            }

            // lockファイルとymlファイルを書き換える
            // もし，uninstall_listと，lockファイルの中身が同一なら，完全にdepsがないということだから，poac.ymlのdepsキーとpoac.lockを消す
            if (resolved_deps.backtracked == uninstall_list) {
                node.remove("deps");
                if (std::ofstream ofs("poac.yml"); ofs) {
                    ofs << node;
                }
                else {
                    throw except::error(except::msg::could_not_load("poac.yml"));
                }
                fs::remove("poac.lock");
            }
            else {
                // uninstall_listに入っていないパッケージのみを書き込む
                for (const auto& [name, dep] : uninstall_list) {
                    // 先に，resolved_deps.activatedから，uninstall_listと同じものを削除する
                    const auto itr = std::find_if(resolved_deps.activated.begin(), resolved_deps.activated.end(),
                            [name=name](auto x){ return x.name == name; });
                    if (itr != resolved_deps.activated.end()) {
                        resolved_deps.activated.erase(itr);
                    }

                    // 同様に，deps_nodeからそのkeyを削除する
                    if (dep.source == "poac") {
                        node["deps"].remove(name);
                    }
                    else if (dep.source == "github") {
                        node["deps"].remove("github/" + name);
                    }
                }
                if (std::ofstream ofs("poac.yml"); ofs) {
                    ofs << node;
                }
                else {
                    throw except::error(except::msg::could_not_load("poac.yml"));
                }
                _install::create_lock_file(timestamp, resolved_deps.activated);
            }

            std::cout << std::endl;
            cli::status_done();
        }

        template <typename VS>
        int _main(VS&& argv) {
            if (util::argparse::use(argv, "-a", "--all")) {
                all(std::move(argv));
                return EXIT_SUCCESS;
            }
            else {
                individual(std::move(argv));
                return EXIT_SUCCESS;
            }
        }

        void check_arguments(const std::vector<std::string>& argv) {
            namespace except = core::except;
            if (argv.empty()) {
                throw except::invalid_second_arg("uninstall");
            }
        }
    }

    struct uninstall {
        static std::string summary() { return "Uninstall packages"; }
        static std::string options() { return "[<pkg-names>, -a | --all, -y | --yes]"; }
        template <typename VS>
        int operator()(VS&& argv) {
            _uninstall::check_arguments(argv);
            return _uninstall::_main(std::forward<VS>(argv));
        }
    };
} // end namespace
#endif // !POAC_OPTS_UNINSTALL_HPP

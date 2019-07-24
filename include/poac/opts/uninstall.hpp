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

#include <poac/opts/install.hpp>
#include <poac/core/except.hpp>
#include <poac/core/name.hpp>
#include <poac/core/resolver/resolve.hpp>
#include <poac/io/term.hpp>
#include <poac/io/path.hpp>
#include <poac/io/config.hpp>
#include <poac/util/argparse.hpp>
#include <poac/util/termcolor2.hpp>

namespace poac::opts::uninstall {
    constexpr auto summary = termcolor2::make_string("Uninstall packages");
    constexpr auto options = termcolor2::make_string("[<pkg-names> | -a, --all | -y, --yes]");

    struct Options {
        bool yes;
        bool all;
        std::vector<std::string> package_list;
    };

//    template <typename InputIterator, typename Backtracked>
//    void create_uninstall_list(
//            InputIterator first,
//            InputIterator last,
//            std::string_view target_name,
//            Backtracked& uninstall_list)
//    {
//        // 同じ名前で複数のバージョンは存在しないことが，
//        // resolved_deps = lock_to_resolved(*locked_deps);
//        // と，
//        // const resolver::Deps deps = _install::resolve_packages(deps_node);
//        // resolved_deps = resolver::resolve(deps);
//        // の両方で保証されているため，同一の名前を検索するだけでパッケージを一意に特定できる．(versionの比較が不要)
//        const auto target = std::find_if(first, last, [&](auto x){ return x.name == target_name; });
//        if (target == last) { // 同じパッケージ名を二つ以上いれた
//            return;
//        }
//
//        // 他に依存されているパッケージは削除しない
//        if (!target->second.dependencies.has_value()) {
//            for (auto itr = first; itr != last; ++itr) {
//                // uninstall_listに入ってない
//                const auto found1 = std::find_if(uninstall_list.begin(), uninstall_list.end(),
//                        [&](auto x){ return x.first == itr->first; });
//                // targetのdepsに入っていない
//                const auto found2 = std::find_if(target->second.dependencies->begin(), target->second.dependencies->end(),
//                        [&](auto x) { return x.name == itr->first; });
//                if (found1 == uninstall_list.end() && found2 == target->second.dependencies->end()) {
//                    for (const auto& [name, version] : itr) {
//                        // 他のパッケージに依存されている
//                        if (target->first == name) {
//                            const auto warn = name + ": " + version +
//                                              " can not be deleted because " +
//                                              itr->name + ": " + itr->version +
//                                              " depends on it";
//                            std::cout << io::term::warning << warn << std::endl;
//                            return;
//                        }
//                    }
//                }
//            }
//        }
//
//        // 循環していないかチェック
//        const auto cycle_check = std::find_if(uninstall_list.begin(), uninstall_list.end(),
//                [&](auto x){ return x.first == target->name; });
//        if (cycle_check == uninstall_list.end()) {
//            // ここまでたどり着いたなら，他に依存されていないということなので，削除リストに加える
//            uninstall_list[target->name] = { {target->version} };
//            // さらにそれの依存も，削除リストに加える
//            for (const auto& td : target->deps) {
//                create_uninstall_list(first, last, td.name, uninstall_list);
//            }
//        }
//    }

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

    [[nodiscard]] std::optional<core::except::Error>
    individual(std::optional<io::config::Config>&& config, uninstall::Options&& opts) {
        namespace fs = boost::filesystem;
        namespace resolve = core::resolver::resolve;
        using termcolor2::color_literals::operator""_red;

        if (config->dependencies) {
            check_exist_name(config->dependencies.value(), opts.package_list);
        } else {
            return core::except::Error::General{
                core::except::msg::could_not_read("deps in poac.yml")
            };
        }

        // create resolved deps
        const auto timestamp = io::config::get_timestamp();
        resolve::ResolvedDeps resolved_deps{};
//        if (const auto locked_deps = core::resolver::lock::load(timestamp)) {
//            resolved_deps = locked_deps.value();
//        } else { // poac.lock does not exist
            const resolve::NoDuplicateDeps deps = install::resolve_packages(config->dependencies.value());
            resolved_deps = resolve::resolve(deps);
//        }

        // create uninstall list
        std::cout << std::endl;
        resolve::NoDuplicateDeps uninstall_list{};
//        const auto first = resolved_deps.activated.begin();
//        const auto last = resolved_deps.activated.end();
//        for (const auto& v : opts.package_list) {
//            create_uninstall_list(first, last, v, uninstall_list); // FIXME
//        }

        // Omit a package that does not already exist

        // confirm
        if (!opts.yes) {
            for (const auto& [name, package] : uninstall_list) {
                std::cout << name << ": " << package.version << std::endl;
            }
            std::cout << std::endl;
            if (const auto error = io::term::yes_or_no("Are you sure delete above packages?")) {
                return error;
            }
        }

        // Delete what was added to uninstall_list
        std::cout << std::endl;
        for (const auto& dep : uninstall_list) {
            const auto package_name = core::name::to_current(dep.first);
            const auto package_path = io::path::current_deps_dir / package_name;
            if (io::path::validate_dir(package_path)) {
                fs::remove_all(package_path);
                std::cout << dep.first << " is deleted" << std::endl;
            }
            else {
                std::cout << dep.first << " is not found"_red << std::endl;
            }
        }

        // TODO: lockファイルとymlファイルを書き換える
        // もし，uninstall_listと，lockファイルの中身が同一なら，完全にdepsがないということだから，poac.ymlのdepsキーとpoac.lockを消す
//        if (resolved_deps.backtracked == uninstall_list) {
//            node.remove("deps");
//            if (std::ofstream ofs("poac.yml"); ofs) {
//                ofs << node;
//            }
//            else {
//                return core::except::Error::General{
//                    core::except::msg::could_not_load("poac.yml")
//                };
//            }
//            fs::remove("poac.lock");
//        }
//        else {
//            // uninstall_listに入っていないパッケージのみを書き込む
//            for (const auto& [name, dep] : uninstall_list) {
//                // 先に，resolved_deps.activatedから，uninstall_listと同じものを削除する
//                const auto itr = std::find_if(resolved_deps.activated.begin(), resolved_deps.activated.end(),
//                        [name=name](auto x){ return x.name == name; });
//                if (itr != resolved_deps.activated.end()) {
//                    resolved_deps.activated.erase(itr);
//                }
//
//                // 同様に，deps_nodeからそのkeyを削除する
//                if (dep.source == "poac") {
//                    node["deps"].remove(name);
//                }
//                else if (dep.source == "github") {
//                    node["deps"].remove("github/" + name);
//                }
//            }
//
//            if (std::ofstream ofs("poac.yml"); ofs) {
//                ofs << node;
//            } else {
//                return core::except::Error::General{
//                    core::except::msg::could_not_load("poac.yml")
//                };
//            }
//            install::create_lock_file(timestamp, resolved_deps.activated);
//        }
        std::cout << std::endl;
        io::term::status_done();
        return std::nullopt;
    }

    [[nodiscard]] std::optional<core::except::Error>
    all(uninstall::Options&& opts) {
        if (!opts.yes) {
            if (const auto error = io::term::yes_or_no("Are you sure uninstall all packages?")) {
                return error;
            }
        }
        boost::filesystem::remove_all(io::path::current_deps_dir);
        return std::nullopt;
    }

    [[nodiscard]] std::optional<core::except::Error>
    uninstall(std::optional<io::config::Config>&& config, uninstall::Options&& opts) {
        if (opts.all) {
            return all(std::move(opts));
        } else {
            return individual(std::move(config), std::move(opts));
        }
    }

    [[nodiscard]] std::optional<core::except::Error>
    exec(std::optional<io::config::Config>&& config, std::vector<std::string>&& args) {
        if (args.empty()) {
            return core::except::Error::InvalidSecondArg::Uninstall;
        }
        uninstall::Options opts{};
        opts.yes = util::argparse::use_rm(args, "-y", "--yes");
        opts.all = util::argparse::use_rm(args, "-a", "--all");
        args.shrink_to_fit();
        opts.package_list = args;
        return uninstall::uninstall(std::move(config), std::move(opts));
    }
} // end namespace
#endif // !POAC_OPTS_UNINSTALL_HPP

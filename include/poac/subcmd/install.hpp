// Beta: It correspond to installation only from GitHub
//       Do not resolve dependencies
#ifndef POAC_SUBCMD_INSTALL_HPP
#define POAC_SUBCMD_INSTALL_HPP

#include <iostream>
#include <algorithm>
#include <iterator>
#include <vector>
#include <string>
#include <sstream>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <tuple>
#include <map>
#include <functional>
#include <utility>
#include <tuple>
#include <fstream>
#include <regex>
#include <chrono>

#include <boost/filesystem.hpp>
#include <boost/optional.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/timer/timer.hpp>
#include <yaml-cpp/yaml.h>

#include "../io.hpp"
#include "../core/exception.hpp"
#include "../sources.hpp"
#include "../util.hpp"


// Version 1 will allow $ poac install <Nothing> only.
//   Parse poac.yml
// Version 2 will also allow $ poac install [<pkg-names>].
//   Parse arguments.

// TODO: --quite．CIや，Docker用
namespace poac::subcmd {
    namespace _install {
        struct Dependency {
            std::string url;
            std::string name;
            std::string version;
            std::string src;
            std::string cache_package_name;
            YAML::Node yaml;
            Dependency(
               std::string u_,
               std::string n_,
               std::string v_,
               std::string s_,
               std::string c_,
               YAML::Node  y_ )
            {
                url = u_;
                name = n_;
                version = v_;
                src = s_;
                cache_package_name = c_;
                yaml = y_;
            }
        };
        using Dependencies = std::vector<Dependency>;
        using Deps = Dependencies;

        // Copy
        // ./poac.yml         pkgname/poac.yml
        //  deps: pkgname ->
        bool configure(const std::string& pkgname, const YAML::Node& deps) {
            using io::file::path::poac_cache_dir;
            const auto from_path = poac_cache_dir / pkgname;
            std::ofstream ofs((from_path / "poac.yml").string());
            ofs << deps;
            return EXIT_SUCCESS;
        }

        // Copy package to ./deps
        bool copy_to_current(const std::string& pkgname, const std::string& pkgname2) {
            namespace path = io::file::path;
            const auto from_path = path::poac_cache_dir / pkgname;
            const auto to_path = path::current_deps_dir / pkgname2;
            return path::recursive_copy(from_path, to_path);
        }

        void echo_install_status(
                const bool res,
                const std::string& name,
                const std::string& version,
                const std::string& src) {
            const std::string status =
                    name + " " + version + " (from: " + src + ")";
            if (res) { // EXIT_FAILURE
                std::cout << io::cli::to_fetch_failed(status)
                          << std::endl;
            }
            else {
                std::cout << io::cli::to_fetched(status)
                          << std::endl;
            }
        }


        void fetch_packages(const Deps& deps) {
            namespace except = core::exception;
            namespace path = io::file::path;
            namespace pack = util::package;

            for (const auto& [url, name, version, src, cache_package_name, yaml] : deps) {
                if (src == "poac") {
                    namespace tb = io::file::tarball;

                    const auto pkg_dir = path::poac_cache_dir / cache_package_name;
                    const auto tar_dir = pkg_dir.string() + ".tar.gz";

                    bool res = io::network::get_file(url, tar_dir);
                    // If res is true, does not execute func. (short-circuit evaluation)
                    res = res || tb::extract_spec_rm_file(tar_dir, pkg_dir);
                    res = res || copy_to_current(cache_package_name, util::package::orgname_to_urlname(name));

                    echo_install_status(res, name, version, src);
                }
                else if (src == "github") {
                    const auto dest = path::poac_cache_dir /
                            pack::github_cache_package_name(name, version);

                    std::map<std::string, std::string> opts;
                    opts.insert(io::network::opt_depth(1));
                    opts.insert(io::network::opt_branch(version));
                    bool res = io::network::clone(url, dest, opts);
                    res = res || configure(cache_package_name, YAML::Clone(yaml));
                    res = res ||
                          copy_to_current(cache_package_name, pack::cache_to_current(cache_package_name));

                    echo_install_status(res, name, version, src);
                }
                else if (src == "cache&poac") {
                    const bool res = copy_to_current(cache_package_name, util::package::orgname_to_urlname(name));
                    echo_install_status(res, name, version, "poac");
                }
                else if (src == "cache&github") {
                    const bool res = copy_to_current(cache_package_name,
                                                     pack::cache_to_current(cache_package_name));
                    echo_install_status(res, name, version, "github");
                }
                else {
                    // If called this, this is a bug.
                    throw except::error("Unexcepted error");
                }
            }
        }


        std::map<std::string, std::string>
        res_dep(std::map<std::string, std::string>& receive_dep) {
            std::map<std::string, std::string> new_deps;

            for (const auto& [name, version] : receive_dep) {
                std::stringstream ss;
                ss << io::network::get("https://poac.pm/api/packages/" + name + "/" + version + "/deps");
                if (ss.str() != "null") {
                    boost::property_tree::ptree pt;
                    boost::property_tree::json_parser::read_json(ss, pt);
                    for (int i = 0;; ++i) {
                        if (const auto dep_name = pt.get_optional<std::string>(std::to_string(i) + ".name")) {
                            if (const auto dep_ver = pt.get_optional<std::string>(std::to_string(i) + ".version")) {
                                if (const auto ver = sources::poac::decide_version(*dep_name, *dep_ver)) {
                                    new_deps.emplace(*dep_name, *ver);
                                }
                            }
                        }
                        else {
                            break;
                        }
                    }
                }
            }

            if (new_deps.empty()) {
                return new_deps;
            }
            else {
                const auto depdepdep = res_dep(new_deps);
                new_deps.insert(depdepdep.begin(), depdepdep.end());
                return new_deps;
            }
        }
        // 形成されたDepsを使用する & Depsの依存も追加する
        void resolve_dependencies(Deps& deps) {
            std::map<std::string, std::string> new_deps;
            // srcがpoacの時のみ依存関係の解決を行う。
            for (auto& [url, name, version, src, cache_package_name, yaml] : deps) {
                if (src == "poac") {
                    if (const auto ver = sources::poac::decide_version(name, version)) {
                        new_deps.emplace(name, *ver);
                        version = *ver;
                        cache_package_name = util::package::to_cache_package_name("poac", util::package::orgname_to_urlname(name), *ver);
                    }
                }
            }
            const auto depdep = res_dep(new_deps);
            // Deps& depsと要素が被ってしまうため，deps_depはdepsへemplace_backしない
//            deps_dep.insert(depdep.begin(), depdep.end());

            for (const auto& [name, version] : depdep) {
                if (const auto ver = sources::poac::decide_version(name, version)) {
                    const std::string url = sources::poac::resolve(util::package::orgname_to_urlname(name), version);
                    const std::string cache_package_name = util::package::to_cache_package_name("poac", util::package::orgname_to_urlname(name), version);
                    const std::string current_package_name = util::package::to_current_package_name("poac", util::package::orgname_to_urlname(name), version);
                    if (sources::cache::resolve(cache_package_name)) {
                        deps.emplace_back("", name, version, "cache&poac", cache_package_name, YAML::Node{});
                    }
                    else if (const auto ver = sources::poac::decide_version(name, version)) {
                        const std::string url = sources::poac::resolve(util::package::orgname_to_urlname(name), *ver);
                        deps.emplace_back(url, name, *ver, "poac", cache_package_name, YAML::Node{});
                    }
                    else { // not found
                        throw core::exception::error("Not found " + name + " " + version);
                    }
                }
            }
        }

        // Depsの形成を行う
        void resolve_packages(Deps& deps, const YAML::Node& node) {
            namespace except = core::exception;

            // Even if a package of the same name is written, it is excluded.
            // However, it can not deal with duplication of other information (e.g. version etc.).
            for (const auto& [name, next_node] : node.as<std::map<std::string, YAML::Node>>()) {
                // hello_world: 0.2.1
                // itr->first: itr->second
                const std::string src = util::package::get_source(next_node);
                const std::string version = util::package::get_version(next_node, src);
                // TODO: srcをorg to name??
                const std::string cache_package_name = util::package::to_cache_package_name(util::package::orgname_to_urlname(src), name, version);
                const std::string current_package_name = util::package::to_current_package_name(util::package::orgname_to_urlname(src), name, version);

                if (sources::current::resolve(current_package_name)) {
                    continue;
                }
                else if (src == "poac") {
                    if (sources::cache::resolve(cache_package_name)) {
                        deps.emplace_back("", name, version, "cache&" + src, cache_package_name, YAML::Node{});
                    }
                    else if (const auto ver = sources::poac::decide_version(name, version)) {
                        const std::string url = sources::poac::resolve(util::package::orgname_to_urlname(name), *ver);
                        deps.emplace_back(url, name, *ver, src, cache_package_name, YAML::Node{});
                    }
                    else { // not found
                        throw except::error("Not found " + name + " " + version);
                    }
                }
                else if (src == "github") {
                    if (sources::cache::resolve(cache_package_name)) {
                        deps.emplace_back("", name, version, "cache&" + src, cache_package_name, YAML::Node{});
                    }
                    else {
                        const std::string url = sources::github::resolve(name);
                        deps.emplace_back(url, name, version, src, cache_package_name, YAML::Clone(next_node));
                        // not foundは，fetch時にしかチェックしない．その時，fetch_failedとして表示される．
                        // 理由は，GitHubではユーザーが存在を確認しやすい点と，
                        //  存在確認のAPIの処理が大幅な時間がかかってしまうため．
                    }
                }
                else { // unknown source
                    throw except::error("Unknown source");
                }
            }
        }

        void check_arguments(const std::vector<std::string>& argv) {
            namespace except = core::exception;
            if (!argv.empty()) throw except::invalid_second_arg("install");
        }


        template<typename VS, typename = std::enable_if_t<std::is_rvalue_reference_v<VS&&>>>
        void _main(VS&& argv) {
            namespace fs = boost::filesystem;
            namespace except = core::exception;
            namespace path = io::file::path;
            namespace yaml = io::file::yaml;
            namespace cli = io::cli;


            check_arguments(argv);
            fs::create_directories(path::poac_cache_dir);
            const auto node = yaml::load_setting_file("deps");


            Deps deps;

            // パッケージがキチンと存在するかの解決
            // と，どこに存在するのかの解決 (既にcurrent?, cache? poac? gitub?)
            std::cout << cli::to_status("Resolving packages...") << std::endl;
            resolve_packages(deps, node.at("deps"));
            if (deps.empty()) throw except::warn("Already up-to-date");


            // 依存関係の解決
            // (依存先のパッケージは，
            //  publish時にpoac.ymlに書いたdepsを保証するため，上の作業をもう一度する必要は無い)
            std::cout << cli::to_status("Resolving dependencies...") << std::endl;
            resolve_dependencies(deps);


            // if (verbose) {
//            for (const auto& [url, name, version, src, cache_package_name, yaml] : deps) {
//                std::cout << url << ", " << name << ", " << version << ", " << cache_package_name << std::endl;
//            }}

            // Download dependencies... (この時点で，パッケージの情報はURLと，パッケージ名，バージョン, ソース
            std::cout << cli::to_status("Fetching...") << std::endl << std::endl;
            fs::create_directories(path::current_deps_dir);
            fetch_packages(deps);
            std::cout << std::endl;

            std::cout << cli::to_status("Done.") << std::endl;
        }
    }

    struct install {
        static const std::string summary() { return "Beta: Install packages."; }
        static const std::string options() { return "<Nothing>"; }
        template<typename VS, typename = std::enable_if_t<std::is_rvalue_reference_v<VS&&>>>
        void operator()(VS&& argv) { _install::_main(std::move(argv)); }
    };
} // end namespace
#endif // !POAC_SUBCMD_INSTALL_HPP

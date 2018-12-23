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
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <yaml-cpp/yaml.h>

#include "../io.hpp"
#include "../core/exception.hpp"
#include "../core/resolver.hpp"
#include "../util.hpp"


// Version 1 will allow $ poac install <Nothing> only.
//   Parse poac.yml
// Version 2 will also allow $ poac install [<pkg-names>].
//   Parse arguments.
// TODO: --source (source file only (not pre-built))
namespace poac::subcmd {
    namespace _install {
        struct Dependency {
            std::string url;
            std::string name;
            std::string version;
            std::string src;
            std::string cache_name;
            std::string current_name;
            bool is_cached;
            YAML::Node yaml;
            Dependency(
               std::string u_,
               std::string n_,
               std::string v_,
               std::string s_,
               std::string ca_,
               std::string cu_,
               bool b,
               YAML::Node  y_ )
            {
                url = u_;
                name = n_;
                version = v_;
                src = s_;
                cache_name = ca_;
                current_name = cu_;
                is_cached = b;
                yaml = y_;
            }
        };
        using Dependencies = std::vector<Dependency>;
        using Deps = Dependencies;

        // Copy
        // ./poac.yml         pkgname/poac.yml
        //  deps: pkgname ->
        bool create_configure(const std::string& pkgname, const YAML::Node& deps) {
            using io::file::path::poac_cache_dir;
            const auto from_path = poac_cache_dir / pkgname;
            std::ofstream ofs((from_path / "poac.yml").string());
            ofs << deps;
            return EXIT_SUCCESS;
        }

        // Copy package to ./deps
        bool copy_to_current(const std::string& from, const std::string& to) {
            namespace path = io::file::path;
            const auto from_path = path::poac_cache_dir / from;
            const auto to_path = path::current_deps_dir / to;
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


        void fetch_packages(const Deps& deps, const bool quite) {
            namespace except = core::exception;
            namespace path = io::file::path;
            namespace tb = io::file::tarball;

            for (const auto& dep : deps) {
                if (dep.is_cached) {
                    const bool res = copy_to_current(dep.cache_name, dep.current_name);
                    if (!quite) {
                        echo_install_status(res, dep.name, dep.version, dep.src);
                    }
                }
                else if (dep.src == "poac") {
                    const auto pkg_dir = path::poac_cache_dir / dep.cache_name;
                    const auto tar_dir = pkg_dir.string() + ".tar.gz";

                    bool res = io::network::get_file(dep.url, tar_dir);
                    // If res is true, does not execute func. (short-circuit evaluation)
                    res = res || tb::extract_spec_rm(tar_dir, pkg_dir);
                    res = res || copy_to_current(dep.cache_name, dep.current_name);

                    if (!quite) {
                        echo_install_status(res, dep.name, dep.version, dep.src);
                    }
                }
                else if (dep.src == "github") {
                    const auto dest = path::poac_cache_dir / dep.cache_name;

                    std::map<std::string, std::string> opts;
                    opts.insert(io::network::opt_depth(1));
                    opts.insert(io::network::opt_branch(dep.version));
                    bool res = io::network::clone(dep.url, dest, opts);
                    res = res || create_configure(dep.cache_name, YAML::Clone(dep.yaml));
                    res = res || copy_to_current(dep.cache_name, dep.current_name);

                    if (!quite) {
                        echo_install_status(res, dep.name, dep.version, dep.src);
                    }
                }
                else {
                    // If called this, it is a bug.
                    throw except::error("Unexcepted error");
                }
            }
        }


        // TODO: 循環に対応
        std::map<std::string, std::string>
        res_dep(std::map<std::string, std::string>& receive_dep) {
            std::map<std::string, std::string> new_deps;

            for (const auto& [name, version] : receive_dep) {
                std::stringstream ss;
                ss << io::network::get(POAC_PACKAGES_API + name + "/" + version + "/deps");
                if (ss.str() != "null") {
                    boost::property_tree::ptree pt;
                    boost::property_tree::json_parser::read_json(ss, pt);

                    for (int i = 0;; ++i) {
                        const std::string si = std::to_string(i);
                        if (const auto dep_name = pt.get_optional<std::string>(si + ".name")) {
                            if (const auto dep_ver = pt.get_optional<std::string>(si + ".version")) {
                                if (const auto ver = core::resolver::poac::decide_version(*dep_name, *dep_ver)) {
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
            namespace except = core::exception;
            namespace naming = core::naming;

            std::map<std::string, std::string> new_deps;
            // srcがpoacの時のみ依存関係の解決を行う。
            for (auto& dep : deps) {
                if (dep.src == "poac") {
                    if (const auto ver = core::resolver::poac::decide_version(dep.name, dep.version)) {
                        new_deps.emplace(dep.name, *ver);
                        dep.version = *ver;
                        dep.cache_name = naming::to_cache("poac", dep.name, *ver);
                    }
                }
            }
            const auto depdep = res_dep(new_deps);
            // Deps& depsと要素が被ってしまうため，deps_depはdepsへemplace_backしない
//            deps_dep.insert(depdep.begin(), depdep.end());

            for (const auto& [name, version] : depdep) {
                if (const auto ver = core::resolver::poac::decide_version(name, version)) {
                    const std::string url = core::resolver::poac::archive_url(name, *ver);
                    const std::string cache_name = naming::to_cache("poac", name, *ver);
                    const std::string current_name = naming::to_current("poac", name, *ver);

                    if (core::resolver::cache::resolve(cache_name)) {
                        deps.emplace_back(
                                "",
                                name,
                                *ver,
                                "poac",
                                cache_name,
                                current_name,
                                true,
                                YAML::Node{}
                        );
                    }
                    else {
                        deps.emplace_back(
                                url,
                                name,
                                *ver,
                                "poac",
                                cache_name,
                                current_name,
                                false,
                                YAML::Node{}
                        );
                    }
                }
                else { // not found
                    throw except::error("Not found " + name + " " + version);
                }
            }
        }

        // Depsの形成を行う
        void resolve_packages(Deps& deps, const YAML::Node& node) {
            namespace except = core::exception;
            namespace naming = core::naming;

            // Even if a package of the same name is written, it is excluded.
            // However, it can not deal with duplication of other information (e.g. version etc.).
            for (const auto& [name, next_node] : node.as<std::map<std::string, YAML::Node>>()) {
                // hello_world: 0.2.1
                // itr->first: itr->second
                const auto [src, name2] = naming::get_source(name);
                const std::string version = naming::get_version(next_node, src);
                const std::string cache_name = naming::to_cache(src, name2, version);
                const std::string current_name = naming::to_current(src, name2, version);

                if (core::resolver::current::resolve(current_name)) {
                    continue;
                }
                else if (src == "poac") {
                    // decide_versionで同じものがcacheに存在しない時点で，
                    // 新しいバージョンが存在することになる．

                    // >=0.1.2 and <3.4.0 -> 2.5.0
                    if (const auto ver = core::resolver::poac::decide_version(name2, version)) {
                        const auto cache_name2 = naming::to_cache(src, name2, *ver);
                        if (core::resolver::cache::resolve(cache_name2)) {
                            deps.emplace_back(
                                    "",
                                    name2,
                                    *ver,
                                    src,
                                    cache_name2,
                                    current_name,
                                    true,
                                    YAML::Node{}
                            );
                        }
                        else {
                            const std::string url = core::resolver::poac::archive_url(current_name, *ver);
                            deps.emplace_back(
                                    url,
                                    name2,
                                    *ver,
                                    src,
                                    cache_name2,
                                    current_name,
                                    false,
                                    YAML::Node{}
                            );
                        }
                    }
                    else { // not found
                        throw except::error("Not found " + name2 + " " + version);
                    }
                }
                else if (src == "github") {
                    if (core::resolver::cache::resolve(cache_name)) {
                        deps.emplace_back(
                                "",
                                name2,
                                version,
                                src,
                                cache_name,
                                current_name,
                                true,
                                YAML::Node{}
                        );
                    }
                    else {
                        const std::string url = core::resolver::github::archive_url(name2);
                        deps.emplace_back(
                                url,
                                name2,
                                version,
                                src,
                                cache_name,
                                current_name,
                                false,
                                YAML::Clone(next_node)
                        );
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

        template<typename VS, typename = std::enable_if_t<std::is_rvalue_reference_v<VS&&>>>
        void _main(VS&& argv) {
            namespace fs = boost::filesystem;
            namespace except = core::exception;
            namespace path = io::file::path;
            namespace yaml = io::file::yaml;
            namespace cli = io::cli;


            core::resolver::poac::resolve();
            throw except::error("finshed");

            fs::create_directories(path::poac_cache_dir);
            const auto node = yaml::load_config("deps");
            const bool quite = util::argparse::use(argv, "-q", "--quite");


            Deps deps;

            // パッケージがキチンと存在するかの解決
            // と，どこに存在するのかの解決 (既にcurrent?, cache? poac? gitub?)
            if (!quite) {
                std::cout << cli::to_status("Resolving packages...") << std::endl;
            }
            resolve_packages(deps, node.at("deps"));
            if (deps.empty()) throw except::warn("Already up-to-date");


            // 依存関係の解決
            // (依存先のパッケージは，
            //  publish時にpoac.ymlに書いたdepsを保証するため，上の作業をもう一度する必要は無い)
            if (!quite) {
                std::cout << cli::to_status("Resolving dependencies...") << std::endl;
            }
            resolve_dependencies(deps);


            // When used at the same time, --quite is given priority.
            const bool verbose = !quite && util::argparse::use(argv, "-v", "--verbose");
            if (verbose) {
                for (const auto& d : deps) {
                    std::cout << "NAME: " << d.name << "\n"
                              << "  VERSION: " <<  d.version << "\n"
                              << "  URL: " << d.url << "\n"
                              << "  SOURCE: " << d.src << "\n"
                              << "  CACHE_NAME: " << d.cache_name << "\n"
                              << "  CURRENT_NAME: " << d.current_name << "\n"
                              << "  IS_CACHED: " << d.is_cached << "\n"
                              << std::endl;
                }
            }


            if (!quite) {
                std::cout << cli::to_status("Fetching...") << std::endl << std::endl;
            }
            fs::create_directories(path::current_deps_dir);
            fetch_packages(deps, quite);
            if (!quite) {
                std::cout << std::endl;
                std::cout << cli::to_status("Done.") << std::endl;
            }
        }

        void check_arguments(const std::vector<std::string>& argv) {
            namespace except = core::exception;
            if (argv.size() > 1) { // -v OR -q
                throw except::invalid_second_arg("install");
            }
        }
    }

    struct install {
        static const std::string summary() {
            return "Install packages";
        }
        static const std::string options() {
            return "[-v | --verbose, -q | --quite]";
        }
        template<typename VS, typename = std::enable_if_t<std::is_rvalue_reference_v<VS&&>>>
        void operator()(VS&& argv) {
            _install::check_arguments(argv);
            _install::_main(std::move(argv));
        }
    };
} // end namespace
#endif // !POAC_SUBCMD_INSTALL_HPP

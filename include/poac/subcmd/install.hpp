// Beta: It correspond to installation only from GitHub
//       Do not resolve dependencies
#ifndef POAC_SUBCMD_INSTALL_HPP
#define POAC_SUBCMD_INSTALL_HPP

#include <iostream>
#include <algorithm>
#include <iterator>
#include <vector>
#include <string>
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
//#include <csignal> trap "rm to_TFRecord.py" 0

#include <boost/filesystem.hpp>
#include <boost/optional.hpp>
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
namespace poac::subcmd { struct install {
    static const std::string summary() { return "Beta: Install packages."; }
    static const std::string options() { return "<Nothing>"; }

    template <typename VS, typename = std::enable_if_t<std::is_rvalue_reference_v<VS&&>>>
    void operator()(VS&& argv) { _main(std::move(argv)); }

    template <typename VS, typename = std::enable_if_t<std::is_rvalue_reference_v<VS&&>>>
    void _main(VS&& argv) {
        namespace fs = boost::filesystem;
        namespace except = core::exception;

        check_arguments(argv);
        fs::create_directories(io::file::path::poac_cache_dir);
        const auto node = io::file::yaml::load_setting_file("deps");


        std::vector<std::tuple<std::string, std::string, std::string, std::string, std::string, YAML::Node>> deps;

        // パッケージがキチンと存在するかの解決 (TODO: そのついでに，poac.ymlのfirestoreの内容を持ってくる)
        // と，どこに存在するのかの解決 (既にcurrent?, cache? poac? gitub?)
        std::cout << io::cli::to_status("Resolving packages...") << std::endl;
        resolve_packages(deps, node.at("deps"));
        if (deps.empty()) throw except::warn("Already up-to-date");


        // 依存関係の解決 (依存先のパッケージは，publish時にpoac.ymlに書いたdepsを保証するため，上の作業をもう一度する必要は無い)
        std::cout << io::cli::to_status("Resolving dependencies...") << std::endl;
        resolve_dependencies(deps);


        // ダウンロードする (この時点で，パッケージの情報はURLと，パッケージ名，バージョン, ソース
        std::cout << io::cli::to_status("Fetching...") << std::endl << std::endl;
        fs::create_directories(io::file::path::current_deps_dir);
        fetch_packages(deps);
        std::cout << std::endl;

        std::cout << io::cli::to_status("Done.") << std::endl;
    }


    template <typename Deps>
    void resolve_packages(Deps& deps, const YAML::Node& node) {
        namespace except = core::exception;
        // Even if a package of the same name is written, it is excluded.
        // However, it can not deal with duplication of other information (e.g. version etc.).
        for (const auto& [name, next_node] : node.as<std::map<std::string, YAML::Node>>()) {
            // hello_world: 0.2.1
            // itr->first: itr->second
            std::string src = util::package::get_source(next_node);
            const std::string version = util::package::get_version(next_node, src);
            const std::string cache_package_name = util::package::to_cache_package_name(src, name, version);
            const std::string current_package_name = util::package::to_current_package_name(src, name, version);

            if (sources::current::resolve(current_package_name)) {
                continue;
            }
            else if (src == "poac") {
                if (sources::cache::resolve(cache_package_name)) {
                    deps.emplace_back("", name, version, "cache&" + src, cache_package_name, YAML::Node{});
                }
                else if (sources::poac::installable(name, version)) {
                    const std::string url = sources::poac::resolve(name, version);
                    deps.emplace_back(url, name, version, src, cache_package_name, YAML::Node{});
                }
                else { // not found
                    // No matching version for hoge ~> 0.11.0 (from: mix.exs) in registry
                    // The latest version is: 0.10.0

                    // Failed to fetch record for 'hexpm/hoge' from registry (using cache)
                    // This could be because the package does not exist, it was spelled incorrectly or you don't have permissions to it
                    // ** (Mix) No package with name hoge (from: mix.exs) in registry
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

    template <typename Deps>
    void resolve_dependencies(Deps& deps) {
        (void)deps;
    }

    template <typename Deps>
    void fetch_packages(const Deps& deps) {
        namespace except = core::exception;

        for (const auto& [url, name, version, src, cache_package_name, yaml] : deps) {
            if (src == "poac") {
                namespace tb = io::file::tarball;

                const auto pkg_dir = io::file::path::poac_cache_dir / cache_package_name;
                const auto tar_dir = pkg_dir.string() + ".tar.gz";

                bool res = io::network::get_file(url, tar_dir);
                // If res is true, does not execute func. (short-circuit evaluation)
                res = res || tb::extract_spec_rm_file(tar_dir, pkg_dir);
                res = res || copy_to_current(cache_package_name, name);

                echo_install_status(res, name, version, src);
            }
            else if (src == "github") {
                const auto dest =
                        io::file::path::poac_cache_dir /
                        util::package::github_cache_package_name(name, version);

                std::map<std::string, std::string> opts;
                opts.insert(io::network::opt_depth(1));
                opts.insert(io::network::opt_branch(version));
                bool res = io::network::clone(url, dest, opts);
                res = res || configure(cache_package_name, YAML::Clone(yaml));
                res = res || copy_to_current(cache_package_name, util::package::cache_to_current(cache_package_name));

                echo_install_status(res, name, version, src);
            }
            else if (src == "cache&poac") {
                const bool res = copy_to_current(cache_package_name, name);
                echo_install_status(res, name, version, "poac");
            }
            else if (src == "cache&github") {
                const bool res = copy_to_current(cache_package_name, util::package::cache_to_current(cache_package_name));
                echo_install_status(res, name, version, "github");
            }
            else {
                // If called this, this is a bug.
                throw except::error("Unexcepted error");
            }
        }
    }

    void echo_install_status(
        const bool res,
        const std::string& name,
        const std::string& version,
        const std::string& src )
    {
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


    // Copy
    // ./poac.yml         pkgname/poac.yml
    //  deps: pkgname ->
    bool configure(const std::string& pkgname, const YAML::Node& deps) {
        const auto from_path =
                io::file::path::poac_cache_dir / pkgname;
        std::ofstream ofs((from_path / "poac.yml").string());
        ofs << deps;
        return EXIT_SUCCESS;
    }

    bool copy_to_current(const std::string& pkgname, const std::string& pkgname2) {
        const auto from_path =
                io::file::path::poac_cache_dir / pkgname;
        const auto to_path =
                io::file::path::current_deps_dir / pkgname2;
        // Copy package to ./deps
        return io::file::path::recursive_copy(from_path, to_path);
    }


    void check_arguments(const std::vector<std::string>& argv) {
        namespace except = core::exception;
        if (!argv.empty()) throw except::invalid_second_arg("install");
    }
};} // end namespace
#endif // !POAC_SUBCMD_INSTALL_HPP

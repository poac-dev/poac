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
#include <thread>
#include <future>
#include <functional>

#include <unistd.h>

#include <boost/filesystem.hpp>
#include <yaml-cpp/yaml.h>

#include "../io.hpp"
#include "../core/except.hpp"
#include "../core/yaml.hpp"
#include "../sources.hpp"


// Version 1 will allow $ poac install <Nothing> only.
//   Parse poac.yml
// Version 2 will also allow $ poac install [<pkg-names>].
//   Parse arguments.
namespace poac::subcmd { struct install {
    static const std::string summary() { return "Beta: Install packages."; }
    static const std::string options() { return "<Nothing>"; }

    template <typename VS>
    void operator()(VS&& vs) { _main(vs); }

    /*
     * ./poac.ymlからdepsの要素を取得
     * その後，current directoryに既に存在する場合(cacheには無い場合も含む)は，インストールタスクから消去します．
     * この時点で，何もインストールするものが無くなれば，Already up-to-dateと表示
     * 次にcacheの確認を行う．
     * cacheに存在し，currentにはない場合，コピーのみを行う．
     * cacheにも無い場合，githubなどからのインストールを行い，それを，まずcacheディレクトリに保存する
     * 解凍後，cacheからcurrentにコピーを行う．
     * 
     * GitHubの場合，poac.ymlが確認された場合のみ，依存関係の解決を行う．
     * 
     * TODO: Check if connecting network
     * TODO: download途中で，ctl Cされたファイルは消す
     * TODO: パッケージの解決順序
     * cache -> poac -> github -> conan -> buckaroo -> None
     * TODO: 各所で探索を行う
     * TODO: 見つかったところを表示する
     * ⠹  Installing... (found in github)
     * ✔  Installed! (found in cache)
     * ×  Not found
     * ×  Install failed
     */
    // 
    template <typename VS>
    void _main(VS&& vs) {
        namespace core = poac::core;
        namespace io   = poac::io;

        std::map<std::string, std::string> deps = check_requirements(vs);
        check_current(deps);
        if (deps.empty()) {
            std::cout << "Already up-to-date" << std::endl;
            std::exit(0);
        }

        const int deps_num = static_cast<int>(deps.size());
        std::vector<std::future<void>> async_funcs;
        std::vector<bool> check_list; // TODO: できればdepsやasync_funcsなどを消すことでこれと同様の役割を担ってほしい．


        std::cout << "Some new packages are needed.\n"
                  << "\n";

        auto s = std::chrono::system_clock::now();
        preinstall(deps, &async_funcs, &check_list);
        for (int i = 0; installing(deps_num, &i, async_funcs, check_list) != deps_num; ++i)
            usleep(100000);
        auto e = std::chrono::system_clock::now();

        std::cout << io::cli::clr_left
                  << io::cli::left(30);
        std::cout << "Elapsed time: "
                  << std::chrono::duration_cast<std::chrono::seconds>(e-s).count()
                  << "s";
        std::cout << std::endl;
        std::cout << io::cli::bold << " ==> Installation finished successfully!" << std::endl;
    }


    std::map<std::string, std::string> check_requirements(const std::vector<std::string>& vs) {
        if (!vs.empty())
            throw poac::core::invalid_second_argument("install");
        if (!poac::core::yaml::notfound_handle()) // auto generate poac.yml?
            throw poac::core::invalid_second_argument("install");

        const YAML::Node config = core::yaml::get_node("deps");
        const auto deps = config.as<std::map<std::string, std::string>>();
        if (deps.empty())
            throw poac::core::invalid_second_argument("install");

        return deps;
    }

    // TODO: できればstatic storageから排除する
    // username/repository -> repository
    static std::string get_name(const std::string& name) {
        if (name.find('/') != std::string::npos)
            return std::string(name, name.find('/')+1);
        else
            return name;
    }

    // TODO: できればstatic storageから排除する
    static std::string make_name(const std::string& name, const std::string& tag) {
        return (name + "-" + tag);
    }

    // TODO: できればstatic storageから排除する
    static std::string connect_path(const boost::filesystem::path& pre, const boost::filesystem::path& post) {
        return (pre / post).c_str();
    }

    bool validate_dir(const boost::filesystem::path& path) {
        namespace fs = boost::filesystem;
        return fs::exists(path) && fs::is_directory(path) && !fs::is_empty(path);
    }

    void check_current(std::map<std::string, std::string>& deps) {
        namespace fs = boost::filesystem;
        if (fs::path deps_dir(fs::path(".") / fs::path("deps")); !fs::exists(deps_dir)) {
            fs::create_directories(deps_dir);
        }
        else {
            for (auto itr = deps.begin(); itr != deps.end(); ) {
                if (validate_dir(connect_path(deps_dir, make_name(get_name(itr->first), itr->second))))
                    deps.erase(itr++);
                else
                    ++itr;
            }
        }
    }

    // TODO: io周りをもっとスマートにしたい
    void preinstall(
            std::map<std::string, std::string> deps,
            std::vector<std::future<void>>* async_funcs,
            std::vector<bool>* check_list
    ) {
        namespace fs = boost::filesystem;
        // check_cache
        fs::create_directories(io::file::POAC_CACHE_DIR);

        std::string status = "Installing..."; // TODO: Print "from github"
        int i = 0;
        for (const auto& [name, tag] : deps) {
            // check_cache
            const bool is_cache = validate_dir(connect_path(io::file::POAC_CACHE_DIR, make_name(get_name(name), tag)));

            std::cout << " ";
            if (is_cache)
                std::cout << "✔  " << io::cli::yellow << "Found in cache." << io::cli::reset;
            else
                std::cout << "   " << status;
            std::cout << "         "
                      << name
                      << ": "
                      << tag
                      << std::endl;

            if (!is_cache) {
                async_funcs->push_back(
                        std::async(std::launch::async, std::bind(_install, name, tag))
                );
            }
            else {
                // Copy package to ./deps
                // If it exists in cache and it is not in the current directory copy it to the current.
                const std::string folder = make_name(get_name(name), tag);
                io::file::copy_dir(connect_path(io::file::POAC_CACHE_DIR, folder), "./deps/" + folder);
            }
            check_list->push_back(is_cache);
            ++i;
        }
        std::cout << std::endl
                  << "0/" << deps.size() << " packages installed";
    }

    int installing(
            int deps_num,
            int* index_now,
            const std::vector<std::future<void>>& async_funcs,
            std::vector<bool>& check_list
    ) {
        if (*index_now >= static_cast<int>(io::cli::spinners.size()))
            *index_now = 0;

        // 0/num packages installed|
        // |0/num packages installed
        std::cout << io::cli::left(30)
                  << io::cli::up(1);

        std::cout << io::cli::up(deps_num);

        int j = 0;
        for (int i = 0; i < deps_num; ++i) {
            // true is finished
            if (!check_list[i]) {
                if (std::future_status::ready == async_funcs[j].wait_for(std::chrono::milliseconds(0))) {
                    std::cout << io::cli::right(2)
                              << "\b"
                              << "✔  "
                              << io::cli::green
                              << "Installed!"
                              << io::cli::reset
                              << "   "
                              << io::cli::left(17);
                    check_list[i] = true;
                    ++j;
                }
                else {
                    std::cout << io::cli::right(2)
                              << "\b"
                              << io::cli::spinners[*index_now]
                              << io::cli::left(2);
                }
            }
            std::cout << io::cli::down(1);
        }
        const int check_list_count = static_cast<int>(std::count_if(check_list.begin(), check_list.end(), [](bool b){return b;}));
        std::cout << std::endl
                  << check_list_count << "/" << deps_num << " packages installed";
        std::cout << std::flush;

        return check_list_count;
    }



    // TODO: これをもっと分離して，deferredにする
    // TODO: できればラムダ式にする
    static void _install(std::string& name, const std::string& tag) {
        namespace fs  = boost::filesystem;
        namespace src = poac::sources;
        namespace io  = poac::io;

        const std::string url  = src::github::resolve(name, tag);
        const std::string filename = io::file::POAC_CACHE_DIR.c_str() + tag + ".tar.gz";
        io::network::get_file(url, filename);

        // username/repository -> repository
        const std::string folder = get_name(name);
        // ~/.poac/cache/package.tar.gz -> ~/.poac/cache/username-repository-tag/...
        io::file::extract_tar_spec(filename, connect_path(io::file::POAC_CACHE_DIR, make_name(folder, tag)));
        fs::remove(filename);

        // Create directory ./deps
        fs::create_directories("./deps");
        // Copy package to ./deps
        io::file::copy_dir(connect_path(io::file::POAC_CACHE_DIR, make_name(folder, tag)), "./deps/" + make_name(folder, tag));
    }


    // template < typename ...func >
    // struct step_functions {
        
    // };
};} // end namespace
#endif // !POAC_SUBCMD_INSTALL_HPP

// Beta: It correspond to installation only from GitHub
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
     * 最初はdepsを取得します．
     * その後，current directoryに既に存在する場合(cacheには無い場合も含む)は，インストールタスクから消去します．
     * この時点で，何もインストールするものが無くなれば，Already up-to-dateと表示
     * 次にcacheの確認を行う．
     * cacheに存在し，currentにはない場合，コピーのみを行う．
     * cacheにも無い場合，githubなどからのインストールを行い，それを，まずcacheディレクトリに保存する
     * 解凍後，cacheからcurrentにコピーを行う．
     * 
     * TODO: Check if connecting network
     * TODO: download途中で，ctl Cされたファイルは消す
     * TODO: パッケージの解決順序
     * cache -> poac -> github -> conan -> buckaroo -> None
     * TODO: 各所で探索を行う
     * TODO: 見つかったところを表示する
     * ✔  Found in cache.
     * ⠹  Installing... (found in github)
     * ✔  Installed!
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

        const std::vector<bool> is_cache  = check_cache(deps);

        const int deps_num = static_cast<int>(deps.size());
        std::vector<std::future<void>> async_funcs;
        std::vector<bool> check_list;


        std::cout << "Some new packages are needed.\n"
                  << "\n";


        auto s = std::chrono::system_clock::now();

        int index_now = 0;
        start_install(deps, &async_funcs, &check_list, is_cache);
        while(true) {
            if (print_status(deps_num, &index_now, async_funcs, check_list) == deps_num) break;
            ++index_now;
            usleep(100000);
        }

        auto e = std::chrono::system_clock::now();

        std::cout << io::cli::clr_left
                  << io::cli::left(30);
        std::cout << "Elapsed time: "
                  << std::chrono::duration_cast<std::chrono::seconds>(e-s).count()
                  << "s";
        std::cout << std::endl;
        std::cout << io::cli::bold << " ==> Installation finished successfully!" << std::endl;
    }


    void check_current(std::map<std::string, std::string>& deps) {
        namespace fs = boost::filesystem;
        fs::path deps_dir(fs::path(".") / fs::path("deps"));

        if (!fs::exists(deps_dir)) {
            fs::create_directories(deps_dir);
        }
        else {
            for (auto itr = deps.begin(); itr != deps.end(); ) {
                // username/repository -> repository
                const std::string folder(itr->first, itr->first.find('/')+1);
                const std::string pkg = (deps_dir / fs::path(folder+"-"+itr->second)).c_str();
                if (fs::exists(pkg) && fs::is_directory(pkg) && !fs::is_empty(pkg)) {
                    deps.erase(itr++);
                }
                else {
                    ++itr;
                }
            }
        }
        return;
    }

    // TODO: これもthreadでやって欲しく無い？？けど，最初のcurrent検索は，だめっぽい．
    // TODO: thread数の制限掛けたい．
    std::vector<bool> check_cache(const std::map<std::string, std::string>& deps) {
        namespace fs = boost::filesystem;
        if (!fs::exists(io::file::POAC_CACHE_DIR)) {
            fs::create_directories(io::file::POAC_CACHE_DIR);
            return std::vector<bool>(deps.size(), false);
        }
        else {
            std::vector<bool> is_cache;
            for (const auto& [name, tag] : deps) {
                // username/repository -> repository
                const std::string folder(name, name.find('/')+1);
                const std::string pkg = (io::file::POAC_CACHE_DIR / fs::path(folder+"-"+tag)).c_str();
                is_cache.push_back(fs::exists(pkg) && fs::is_directory(pkg) && !fs::is_empty(pkg));
            }
            return is_cache;
        }
    }

    void start_install(
            std::map<std::string, std::string> deps,
            std::vector<std::future<void>>* async_funcs,
            std::vector<bool>* check_list,
            const std::vector<bool>& is_cache
    ) {
        std::string status = "Installing..."; // TODO: Print "from github"
        int i = 0;
        for (const auto& [key, value] : deps) {
            std::cout << " ";
            if (is_cache[i])
                std::cout << "✔  " << io::cli::yellow << "Found in cache." << io::cli::reset;
            else
                std::cout << "   " << status;
            std::cout << "         "
                      << key
                      << ": "
                      << value
                      << std::endl;
            // TODO: push backをしないでほしい．しかし，print_status関数で，deps_numの影響を受けるはず
            // TODO: それに加え，wait_forでも同様に影響を受ける. できるならば空で入れたい
            if (!is_cache[i]) {
                async_funcs->push_back(
                        std::async(std::launch::async, std::bind(_install, key, value))
                );
            }
            check_list->push_back(is_cache[i]);
            ++i;
        }
        std::cout << std::endl
                  << "0/" << deps.size() << " packages installed";
    }

    int print_status(
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

    static void _install(std::string& name, const std::string& tag) {
        namespace src = poac::sources;
        namespace fs  = boost::filesystem;

        const std::string url  = src::github::resolve(name, tag);
        const std::string root = io::file::expand_user("~/.poac");
        const std::string filename = root + "/cache/" + tag + ".tar.gz";
        io::network::file_get(url, filename);

        // username/repository -> repository
        const std::string folder(name, name.find('/')+1);
        // ~/.poac/cache/package.tar.gz -> ~/.poac/cache/username-repository-tag/...
        io::file::extract_tar_spec(filename, root + "/cache/"+folder+"-"+tag);
        fs::remove(filename);

        // Create directory ./deps
        fs::create_directories("./deps");
        // Copy package to ./deps
        io::file::copy_dir(root + "/cache/"+folder+"-"+tag, "./deps/" + folder+"-"+tag);
    }
};} // end namespace
#endif // !POAC_SUBCMD_INSTALL_HPP

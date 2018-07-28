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
#include <iomanip>
#include <utility>

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
     * TODO: Installability from source: GitHub (以下の条件が満たされない場合インストール不可である．)
     * TODO: 1. プロジェクトルートにpoac.ymlが存在する (poacによる依存関係解決アルゴリズムが使用可能)
     * TODO: 2-1. プロジェクトルートにCMakeLists.txtが存在する
     * TODO: 2-2. 2-1が満たされなかったとしても，includeディレクトリが存在する
     *
     * TODO: Check if connecting network
     * TODO: download途中で，ctl Cされたファイルは消す
     * TODO: Corresponds to redirect
     * TODO: パッケージの解決順序
     * cache -> poac -> github -> conan -> buckaroo -> None
     * TODO: 各所で探索を行う
     * TODO: 見つかったところを表示する
     */
    template <typename VS>
    void _main(VS&& vs) {
        namespace fs = boost::filesystem;
        namespace core = poac::core;
        namespace io   = poac::io;
        

        std::map<std::string, std::string> deps = check_requirements(vs);
        check_current(deps);
        if (deps.empty()) {
            std::cout << "Already up-to-date" << std::endl;
            std::exit(0);
        }

        const int deps_num = static_cast<int>(deps.size());
        // std::vector<std::future<void>> async_funcs;
        //                    task,              source
        std::vector<std::pair<std::future<void>, std::string>> async_funcs;
        // std::vector<bool> check_list;


        std::cout << "Some new packages are needed.\n"
                  << "\n";

        auto s = std::chrono::system_clock::now();

        fs::create_directories(io::file::POAC_CACHE_DIR);
        preinstall(deps, &async_funcs);
        for (int i = 0; install_now(&i, async_funcs) != deps_num; ++i)
            usleep(100000);
        
        auto e = std::chrono::system_clock::now();

        std::cout << io::cli::clr_line
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

    void set_left() {
        std::cout << std::setw(34) << std::left;
    }
    void installing(const int& index, const std::string& src) {
        std::cout << " " << io::cli::spinners[index] << "  ";
        set_left();
        std::cout << "Installing... (found in " + src + ")";
    }
    void installed(const std::string& src) {
        std::cout << " ✔  " << io::cli::green;
        set_left();
        std::cout << "Installed! (found in " + src + ")" << io::cli::reset;
    }
    void not_found() {
        std::cout << " ×  " << io::cli::red;
        set_left();
        std::cout << "Not found" << io::cli::reset;
    }
    void install_failed() {
        std::cout << " ×  " << io::cli::red;
        set_left();
        std::cout << "Install failed" << io::cli::reset;
    }

    void preinstall(
            std::map<std::string, std::string> deps,
            std::vector<std::pair<std::future<void>, std::string>>* async_funcs // 既にインストール完了などありえない．cacheにある場合でも，コピーするタスクがあるはずだ．
    ) {
        for (const auto& [name, tag] : deps) {
            // check_cache
            const bool is_cache = validate_dir(connect_path(io::file::POAC_CACHE_DIR, make_name(get_name(name), tag)));
            if (is_cache) {
                installed("cache");
                std::cout << name << ": " << tag << std::endl;

                async_funcs->push_back(
                    std::make_pair(
                        std::async(std::launch::async, std::bind(_copy, name, tag)),
                        "cache"
                    )
                );
            }
            else {
                installing(0, "github");
                std::cout << name << ": " << tag << std::endl;

                async_funcs->push_back(
                    std::make_pair(
                        std::async(std::launch::async, std::bind(_install, name, tag)),
                        "github"
                    )
                );
            }
        }
        std::cout << std::endl
                  << "0/" << deps.size() << " packages installed";
    }

    int install_now(
            int* index_now,
            const std::vector<std::pair<std::future<void>, std::string>>& async_funcs
    ) {
        if (*index_now >= static_cast<int>(io::cli::spinners.size()))
            *index_now = 0;

        // 0/num packages installed|
        // |0/num packages installed
        std::cout << io::cli::left(50)
                  << io::cli::up(1);

        std::cout << io::cli::up(async_funcs.size());

        int count = 0;
        for (const auto& fun : async_funcs) {
            // true is finished
            if (std::future_status::ready == fun.first.wait_for(std::chrono::milliseconds(0))) {
                std::cout << io::cli::right(1)
                            << "\b";
                installed(fun.second);
                std::cout << io::cli::left(50);
                ++count;
            }
            else {
                std::cout << io::cli::right(2)
                            << "\b"
                            << io::cli::spinners[*index_now]
                            << io::cli::left(50);
            }
            std::cout << io::cli::down(1);
        }
        std::cout << std::endl
                  << io::cli::right(1) << '\b'
                  << count;
        std::cout << std::flush;

        return count;
    }

    static void _copy(const std::string& name, const std::string& tag) {
        // Copy package to ./deps
        // If it exists in cache and it is not in the current directory copy it to the current.
        const std::string folder = make_name(get_name(name), tag);
        io::file::copy_dir(connect_path(io::file::POAC_CACHE_DIR, folder), "./deps/" + folder);
    }

    // TODO: これをもっと分離して，deferredにする
    // TODO: できればラムダ式にする
    static void _install(const std::string& name, const std::string& tag) {
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


     template < typename Funcs, Funcs ...fs >
     struct step_functions {
         const std::vector<Funcs> funcs;
         step_functions() : funcs({ fs... }) {}
     };
};} // end namespace
#endif // !POAC_SUBCMD_INSTALL_HPP

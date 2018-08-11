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
#include <boost/timer/timer.hpp>
#include <yaml-cpp/yaml.h>

#include "../io.hpp"
#include "../core/except.hpp"
#include "../sources.hpp"
#include "../util.hpp"


// Version 1 will allow $ poac install <Nothing> only.
//   Parse poac.yml
// Version 2 will also allow $ poac install [<pkg-names>].
//   Parse arguments.
namespace poac::subcmd { struct install {
    static const std::string summary() { return "Beta: Install packages."; }
    static const std::string options() { return "<Nothing>"; }

    template <typename VS>
    void operator()(VS&& vs) { _main(vs); }

    static void progress(const int& index, const std::string& status, const std::string& src) {
        std::cout << " " << io::cli::spinners[index] << "  ";
        io::cli::set_left(35);
        const std::string point = [&index](){
            if      (index <= 2) return ".  ";
            else if (index <= 5) return ".. ";
            else                 return "...";
        }();
        std::cout << status + point + " (found in " + src + ")";
    }
    static void installed(const std::string& src) {
        std::cout << io::cli::green << " ✔  ";
        io::cli::set_left(35);
        std::cout << "Installed! (found in " + src + ")" << io::cli::reset;
    }
    static void not_found() {
        std::cout << io::cli::red << " ×  ";
        io::cli::set_left(35);
        std::cout << "Not found" << io::cli::reset;
    }
    static void install_failed() {
        std::cout << io::cli::red << " ×  ";
        io::cli::set_left(35);
        std::cout << "Install failed" << io::cli::reset;
    }

    void rewrite(const std::function<void()>& f1, const std::function<void()>& f2) {
        std::cout << io::cli::right(1) << "\b";
        f1(); f2();
        std::cout << io::cli::left(100);
    }

    template <typename Async>
    int installing(int* index_now, const Async& async_funcs) {
        *index_now %= static_cast<int>(io::cli::spinners.size());

        // 0/num packages installed|
        // |0/num packages installed
        std::cout << io::cli::left(50) << io::cli::up(1);
        std::cout << io::cli::up(async_funcs.size());

        int count = 0;
        for (const auto& [func, info_func, src] : async_funcs) {
            // -1 is finished
            const int status = func.wait_for(std::chrono::milliseconds(0));
            if (status == -1) {
                if (src == "notfound")
                    rewrite(std::bind(&not_found), info_func);
                else
                    rewrite(std::bind(&installed, src), info_func);
                ++count;
            }
            else {
                const std::string now = [&status]() {
                    if (status == 0)      return "Downloading";
                    else if (status == 1) return "Building";
                    else if (status == 2) return "Copying";
                    else                  return "Error";
                }();
                rewrite(std::bind(&progress, *index_now, now, src), info_func);
            }
            std::cout << io::cli::down(1);
        }
        std::cout << std::endl
                  << io::cli::right(1)
                  << '\b'
                  << count;
        std::cout << std::flush;

        return count;
    }

    template <typename Async>
    void dependencies(Async* async_funcs) {
        namespace fs     = boost::filesystem;
        namespace except = core::except;
        namespace src    = sources;


        const YAML::Node deps = io::file::yaml::get_node("deps");
        int already_count = 0;

        // TODO: 同じdepsが書かれている場合の対策が施されていない．
        for (YAML::const_iterator itr = deps.begin(); itr != deps.end(); ++itr) {
            const std::string name = itr->first.as<std::string>();
            if (auto src = io::file::yaml::get<std::string>(itr->second, "src")) {
                if (auto func_pack = src::inference::resolve(itr->second, name, *src))
                    async_funcs->emplace_back(std::move(*func_pack));
                else
                    ++already_count;
            }
            else {
                const std::string version = itr->second.as<std::string>();

                if (auto func_pack = src::inference::resolve_poac(itr->second, name, version))
                    async_funcs->emplace_back(std::move(*func_pack));
                else
                    ++already_count;
            }
        }

        if (async_funcs->empty()) {
            if (already_count > 0)
                throw except::warn("Already up-to-date");
            else // When deps is not written in poac.yml
                throw except::invalid_second_arg("install");
        }

        // Start async functions...
        // TODO: hardware concurrency
        for (const auto& [func, _info, _src] : *async_funcs) {
            func.start();
            ((void)_info, (void)_src); // Avoid unused warning
        }
    }

    void check_requirements() {
        namespace fs     = boost::filesystem;
        namespace except = core::except;

        // Auto generate poac.yml on Version 2.
        if (!io::file::yaml::exists()) throw except::error("poac.yml is not found");
        fs::create_directories(io::file::path::poac_cache_dir);
    }

    void check_arguments(const std::vector<std::string>& vs) {
        namespace except = core::except;
        if (!vs.empty()) throw except::invalid_second_arg("install");
    }

    /*
     * ./poac.ymlからdepsの要素を取得
     * その後，current directoryに既に存在する場合(cacheには無い場合も含む)は，インストールタスクから消去します．
     * この時点で，何もインストールするものが無くなれば，Already up-to-dateと表示
     * 次にcacheの確認を行う．
     * cacheに存在し，currentにはない場合，コピーのみを行う．
     * cacheにも無い場合，githubなどからのインストールを行い，それを，まずcacheディレクトリに保存する
     * 解凍後，cacheからcurrentにコピーを行う．
     *
     * TODO: Check if connecting network
     * TODO: download途中で，ctl Cされたファイルは消す
     * TODO: Error handling. (tarball url not found.. etc)
     * TODO: カレントディレクトリには，バージョン番号は付かない
     */
    template <typename VS>
    void _main(VS&& vs) {
        namespace fs     = boost::filesystem;
        namespace except = core::except;

        // Start timer
        boost::timer::cpu_timer timer;


        check_arguments(vs);
        check_requirements();

        std::vector<std::tuple<util::step_functions, std::function<void()>, std::string>> async_funcs;
        dependencies(&async_funcs);

        const int deps_num = static_cast<int>(async_funcs.size());

        std::cout << "Some new packages are needed.\n\n";
        for (int i = 0; i < deps_num; ++i) std::cout << std::endl;
        std::cout << std::endl
                  << "0/" << deps_num << " packages installed";

        for (int i = 0; installing(&i, async_funcs) != deps_num; ++i)
            std::this_thread::sleep_for(std::chrono::microseconds(100000));


        std::cout << io::cli::clr_line
                  << io::cli::left(30)
                  << "Elapsed time: "
                  << util::pretty_time::to(timer.format(6, "%w"))
                  << std::endl
                  << io::cli::bold
                  << " ==> Installation finished successfully!"
                  << std::endl;
    }
};} // end namespace
#endif // !POAC_SUBCMD_INSTALL_HPP

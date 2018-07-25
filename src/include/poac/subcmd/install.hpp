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
    static const std::string summary() { return "Install packages."; }
    static const std::string options() { return "<Nothing>"; }

    template <typename VS>
    void operator()(VS&& vs) { _main(vs); }
    ///
    /// \tparam VS
    /// \param vs
    template <typename VS>
    void _main(VS&& vs) {
        namespace core = poac::core;
        namespace io   = poac::io;

        const auto deps = check_requirements(vs);
        const auto num = static_cast<int>(deps.size());
        std::vector<std::future<void>> async_funcs;
        std::vector<bool> check_list;


        std::cout << "Some new packages are needed. Here is the upgrade plan.\n"
                  << "\n";


        auto s = std::chrono::system_clock::now();

        int index_now = 0;
        start_install(deps, &async_funcs, &check_list);
        while(true) {
            if (print_status(&index_now, async_funcs, check_list) == num) break;
            ++index_now;
            usleep(100000);
        }

        auto e = std::chrono::system_clock::now();

        std::cout << io::cli::clr_left
                  << io::cli::left(30);
        std::cout << "Elapsed time: "
                  << std::chrono::duration_cast<std::chrono::seconds>(e-s).count()
                  << "[s]";
        std::cout << std::endl;
        std::cout << io::cli::bold << " ==> Installation finished successfully!" << std::endl;



        // TODO: Check if connecting network
        // TODO: If cache package exists, it use
        // TODO: cp to local

        // TODO: パッケージの解決順序
        //       cache -> poac -> github -> conan -> buckaroo -> None
        // TODO: 並列で探索を行う
        // ▶ Package: Finding pkg in cache... -> not found in cache
        // ▶ Package: Finding pkg in poac... -> not found in poac
        // ▶ Package: Could not found on GitHub. Please use username/pkgname style
        // ▶ Package: Finding pkg in conan... -> not found in conan
        // ▶ Package: Finding pkg in buckaroo... -> not found in buckaroo
        // ▶ Package: pkg is not found. -> Did you want package?

        // Install failed

        // cachesディレクトリの存在確認 -> あれば，全部で並列探索 -> なければ，cache以外で並列探索
    }


    int print_status(int* index_now, const std::vector<std::future<void>>& async_funcs, std::vector<bool>& check_list) {
        int deps_num = static_cast<int>(async_funcs.size());
        if (*index_now >= static_cast<int>(io::cli::spinners.size()))
            *index_now = 0;

        // 0/num packages installed|
        // |0/num packages installed
        std::cout << io::cli::left(30)
                  << io::cli::up(1);

        std::cout << io::cli::up(deps_num);
        for (int i = 0; i < deps_num; ++i) {
            // true is finished
            if (!check_list[i]) {
                if (std::future_status::ready == async_funcs[i].wait_for(std::chrono::milliseconds(0))) {
                    std::cout << io::cli::right(2)
                              << "\b"
                              << "✔  "
                              << io::cli::green
                              << "Installed!"
                              << io::cli::reset
                              << "   "
                              << io::cli::left(17);
                    check_list[i] = true;
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

    void start_install(std::map<std::string, std::string> deps, std::vector<std::future<void>>* async_funcs, std::vector<bool>* check_list) {
        std::string status = "Installing..."; // TODO: Print "from github"
        for (const auto& [key, value] : deps) {
            std::cout << "    "
                      << status
                      << "         "
                      << key
                      << ": "
                      << value
                      << std::endl;
            async_funcs->push_back(
                    std::async(std::launch::async, std::bind(_install, key, value))
            );
            check_list->push_back(false);
        }
        std::cout << std::endl
                  << "0/" << deps.size() << " packages installed";
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

    // TODO: mkdir -p ~/.poac/cache
    static void _install(const std::string& name, const std::string& tag) {
        namespace src = poac::sources;
        const std::string url = src::github::resolve(name, tag);
        io::network::file_get(url, io::file::expand_user("~/.poac/cache/" + tag + ".tar.gz"));
    }
};} // end namespace
#endif // !POAC_SUBCMD_INSTALL_HPP

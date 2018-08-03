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
#include <tuple>
#include <fstream>

#include <unistd.h>

#include <boost/filesystem.hpp>
#include <boost/timer/timer.hpp>
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
     * TODO: Check if connecting network
     * TODO: download途中で，ctl Cされたファイルは消す
     * TODO: Corresponds to redirect
     */
    template <typename VS>
    void _main(VS&& vs) {
        namespace fs     = boost::filesystem;
        namespace except = core::except;

        // Start timer
        boost::timer::cpu_timer timer;


        // TODO: 同じdepsを二つ以上書いた時の対処法
        std::map<std::string, std::string> deps = check_requirements(vs);
        check_current(deps);
        if (deps.empty()) throw except::warn("Already up-to-date");

        std::cout << "Some new packages are needed.\n"
                  << "\n";

        fs::create_directories(io::file::POAC_CACHE_DIR);


        const int deps_num(deps.size());

        std::vector<std::tuple<step_functions, std::function<void()>, std::string>> async_funcs;
        preinstall(deps, &async_funcs);
        for (int i = 0; install_now(&i, async_funcs) != deps_num; ++i)
            usleep(100000);


        std::cout << io::cli::clr_line
                  << io::cli::left(30);
        std::cout << timer.format(6, "Elapsed time: %ws");
        std::cout << std::endl;
        std::cout << io::cli::bold << " ==> Installation finished successfully!" << std::endl;
    }


    std::map<std::string, std::string> check_requirements(const std::vector<std::string>& vs) {
        namespace except = core::except;

        if (!vs.empty())
            throw except::invalid_second_arg("install");
        if (!core::yaml::notfound_handle()) // Auto generate poac.yml on Version 2.
            throw except::invalid_second_arg("install");

        const YAML::Node config = core::yaml::get_node("deps");
        const auto deps = config.as<std::map<std::string, std::string>>();
        if (deps.empty())
            throw except::invalid_second_arg("install");

        return deps;
    }

    // username/repository -> repository
    static std::string get_name(const std::string& name) {
        if (name.find('/') != std::string::npos)
            return std::string(name, name.find('/')+1);
        else
            return name;
    }

    static std::string make_name(const std::string& name, const std::string& tag) {
        return (name + "-" + tag);
    }

    void check_current(std::map<std::string, std::string>& deps) {
        namespace fs = boost::filesystem;
        if (fs::path deps_dir(fs::path(".") / fs::path("deps")); !fs::exists(deps_dir)) {
            fs::create_directories(deps_dir);
        }
        else {
            for (auto itr = deps.begin(); itr != deps.end(); ) {
                if (fs::exists(io::file::connect_path(deps_dir, make_name(get_name(itr->first), itr->second) + ".yml")))
                    deps.erase(itr++);
                else
                    ++itr;
            }
        }
    }

    static void set_left(const int&& n) {
        std::cout << std::setw(n) << std::left;
    }
    static void info(const std::string& name, const std::string& tag) {
        std::cout << name << ": " << tag;
    }
    static void progress(const int& index, const std::string& status, const std::string& src) {
        std::cout << " " << io::cli::spinners[index] << "  ";
        set_left(35);
        const std::string point = [&index](){
            if      (index <= 2) return ".  ";
            else if (index <= 5) return ".. ";
            else                 return "...";
        }();
        std::cout << status + point + " (found in " + src + ")";
    }
    static void installed(const std::string& src) {
        std::cout << " ✔  " << io::cli::green;
        set_left(35);
        std::cout << "Installed! (found in " + src + ")" << io::cli::reset;
    }
    static void not_found() {
        std::cout << " ×  " << io::cli::red;
        set_left(35);
        std::cout << "Not found" << io::cli::reset;
    }
    static void install_failed() {
        std::cout << " ×  " << io::cli::red;
        set_left(35);
        std::cout << "Install failed" << io::cli::reset;
    }

    void rewrite(const std::function<void()>& f1, const std::function<void()>& f2) {
        std::cout << io::cli::right(1) << "\b";
        f1(); f2();
        std::cout << io::cli::left(100);
    }


    template <typename Async>
    void preinstall(const std::map<std::string, std::string>& deps, Async* async_funcs) {
        namespace src = sources;

        // Search
        // cache -> poac -> github -> conan -> buckaroo -> notfound
        // TODO: 各所で探索を行う
        // TODO: どちらにせよ遅い．async_funcsに詰めた方が良い

        for (const auto& [name, tag] : deps) {
            if (src::cache::resolve(name, tag)) {
                async_funcs->emplace_back(
                    step_functions(
                            std::bind(&_copy, name, tag)
                    ),
                    std::bind(&info, name, tag),
                    "cache"
                );
            }
            else if (src::github::installable(name, tag)) {
                async_funcs->emplace_back(
                    step_functions(
                            std::bind(&_install, name, tag),
                            std::bind(&_build, name, tag),
                            std::bind(&_copy, name, tag)
                    ),
                    std::bind(&info, name, tag),
                    "github"
                );
            }
            else {
                async_funcs->emplace_back(
                    step_functions(
                            std::bind(&_placeholder)
                    ),
                    std::bind(&info, name, tag),
                    "notfound"
                );
            }
        }

        for (int i = 0; i < static_cast<int>(deps.size()); ++i)
            std::cout << std::endl;
        std::cout << std::endl
                  << "0/" << deps.size() << " packages installed";
    }

    template <typename Async>
    int install_now(int* index_now, const Async& async_funcs) {
        if (*index_now >= static_cast<int>(io::cli::spinners.size()))
            *index_now = 0;

        // 0/num packages installed|
        // |0/num packages installed
        std::cout << io::cli::left(50) << io::cli::up(1);
        std::cout << io::cli::up(async_funcs.size());

        int count = 0;
        for (const auto& fun : async_funcs) {
            // -1 is finished
            const int status = std::get<0>(fun).wait_for(std::chrono::milliseconds(0));
            if (status == -1) {
                if (std::get<2>(fun) == "notfound")
                    rewrite(std::bind(&not_found), std::get<1>(fun));
                else
                    rewrite(std::bind(&installed, std::get<2>(fun)), std::get<1>(fun));
                ++count;
            }
            else {
                // TODO: ダサい
                const std::string now = [&status]() {
                    // TODO: イメージ的には，status == 0 "Resolving"
                    // TODO: status == -1 "Not found" // ただし．これもprogressになってしまう
                    if (status == 0)
                        return "Downloading";
                    else if (status == 1)
                        return "Building";
                    else if (status == 2)
                        return "Copying";
                    else
                        return "Error";
                }();
                rewrite(std::bind(&progress, *index_now, now, std::get<2>(fun)), std::get<1>(fun));
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


    static void _install(const std::string& name, const std::string& tag) {
        namespace fs  = boost::filesystem;
        namespace src = sources;

        const std::string url  = src::github::resolve(name, tag);
        const std::string filename = io::file::POAC_CACHE_DIR.c_str() + tag + ".tar.gz";
        io::network::get_file(url, filename);

        // ~/.poac/cache/package.tar.gz -> ~/.poac/cache/username-repository-tag/...
        io::file::extract_tar_spec(filename, io::file::connect_path(io::file::POAC_CACHE_DIR, make_name(get_name(name), tag)));
        fs::remove(filename);
    }

    // TODO: poac.ymlが存在する場合．
    static void _build(const std::string& name, const std::string& tag) {
        namespace fs     = boost::filesystem;
        namespace except = core::except;

        std::string filepath = io::file::connect_path(io::file::POAC_CACHE_DIR, make_name(get_name(name), tag));

        // TODO: LICENSEなどが消えてしまう．
        if (fs::exists(io::file::connect_path(filepath, "CMakeLists.txt"))) {
            std::string command("cd " + filepath);
            command += " && mkdir build";
            command += " && cd build";
            // TODO: MACOSX_RPATHはダメ．
            command += " && MACOSX_RPATH=1 OPENSSL_ROOT_DIR=/usr/local/opt/openssl/ cmake .. 2>&1"; // TODO: 抽象化
            command += " && make -j4 2>&1"; // TODO: 抽象化
            command += " && DESTDIR=./ make install 2>&1";

            std::array<char, 128> buffer;
            std::string result;

            FILE* pipe = popen(command.c_str(), "r");
            if (!pipe) throw except::error("Couldn't start command.");
            while (std::fgets(buffer.data(), 128, pipe) != nullptr) {
                result += buffer.data();
            }

            if (int return_code = pclose(pipe); return_code == 0) {
                const std::string filepath_tmp = filepath + "_tmp";
                fs::rename(filepath, filepath_tmp);
                fs::create_directories(filepath);

                const fs::path build_after_dir(fs::path(filepath_tmp) / "build" / "usr" / "local");

                // Write to cache.yml and recurcive copy
                if (io::file::validate_dir(build_after_dir / "bin")) {
                    io::file::recursive_copy(build_after_dir / "bin", fs::path(filepath) / "bin");
                }
                if (io::file::validate_dir(build_after_dir / "include")) {
                    io::file::recursive_copy(build_after_dir / "include", fs::path(filepath) / "include");
                }
                if (io::file::validate_dir(build_after_dir / "lib")) {
                    io::file::recursive_copy(build_after_dir / "lib", fs::path(filepath) / "lib");
                }
                fs::remove_all(filepath_tmp);
            }
        }
        // Do not exists CMakeLists.txt, but ...
        else if (io::file::validate_dir(io::file::connect_path(filepath, "include"))) {
            const std::string filepath_tmp = filepath + "_tmp";

            fs::rename(filepath, filepath_tmp);
            fs::create_directories(filepath);
            io::file::recursive_copy(fs::path(filepath_tmp) / "include", fs::path(filepath) / "include");
            fs::remove_all(filepath_tmp);
        }
    }

    static void _copy(const std::string& name, const std::string& tag) {
        namespace fs = boost::filesystem;

        fs::create_directories("./deps");
        // Copy package to ./deps
        // If it exists in cache and it is not in the current directory copy it to the current.
        const std::string package = make_name(get_name(name), tag);
        io::file::recursive_copy(io::file::POAC_CACHE_DIR / package, fs::path(".") / "deps" / package);
    }

    static void _placeholder() {}


    struct priority_functions {};

    struct step_functions {
        const std::vector<std::function<void()>> funcs;
        const size_t size;
        std::future<void> mutable func_now;
        unsigned int mutable index = 0;

        template < typename ...Funcs >
        explicit step_functions(Funcs ...fs) : funcs({fs...}), size(sizeof...(Funcs)) { run(); }

        void run() const { func_now = std::async(std::launch::async, funcs[index]); }
        int next() const { ++index; run(); return index; }
        // All done: -1, Now: index
        template <class Rep, class Period>
        int wait_for(const std::chrono::duration<Rep, Period>& rel_time) const {
            if (std::future_status::ready == func_now.wait_for(rel_time)) {
                if (index < (size-1)) return next();
                else                  return -1;
            }
            else return index;
        }
    };
};} // end namespace
#endif // !POAC_SUBCMD_INSTALL_HPP

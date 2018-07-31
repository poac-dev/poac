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
     * TODO: Check if connecting network
     * TODO: download途中で，ctl Cされたファイルは消す
     * TODO: Corresponds to redirect
     * TODO: パッケージの解決順序
     * cache -> poac -> github -> conan -> buckaroo -> None
     * TODO: 各所で探索を行う
     */
    template <typename VS>
    void _main(VS&& vs) {
        namespace fs     = boost::filesystem;
        namespace core   = poac::core;
        namespace except = poac::core::except;
        namespace io     = poac::io;

        auto s = std::chrono::system_clock::now();

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
        namespace except = poac::core::except;

        if (!vs.empty())
            throw except::invalid_second_arg("install");
        if (!poac::core::yaml::notfound_handle()) // Auto generate poac.yml on Version 2.
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

    static void set_left() {
        std::cout << std::setw(35) << std::left;
    }
    static void info(const std::string& name, const std::string& tag) {
        std::cout << name << ": " << tag;
    }
    static void progress(const int& index, const std::string& status, const std::string& src) {
        std::cout << " " << io::cli::spinners[index] << "  ";
        set_left();
        const std::string point = [&index](){
            if      (index <= 2) return ".  ";
            else if (index <= 5) return ".. ";
            else                 return "...";
        }();
        std::cout << status + point + " (found in " + src + ")";
    }
    static void installed(const std::string& src) {
        std::cout << " ✔  " << io::cli::green;
        set_left();
        std::cout << "Installed! (found in " + src + ")" << io::cli::reset;
    }
    static void not_found() {
        std::cout << " ×  " << io::cli::red;
        set_left();
        std::cout << "Not found" << io::cli::reset;
    }
    static void install_failed() {
        std::cout << " ×  " << io::cli::red;
        set_left();
        std::cout << "Install failed" << io::cli::reset;
    }

    void rewrite(const std::function<void()>& f1, const std::function<void()>& f2) {
        std::cout << io::cli::right(1) << "\b";
        f1(); f2();
        std::cout << io::cli::left(100);
    }


    template <typename Async>
    void preinstall(const std::map<std::string, std::string>& deps, Async* async_funcs) {
        namespace src = poac::sources;

        for (const auto& [name, tag] : deps) {
            if (validate_dir(connect_path(io::file::POAC_CACHE_DIR, make_name(get_name(name), tag)))) {
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

        for (int i = 0; i < static_cast<int>(deps.size()); ++i) {
            std::cout << std::endl;
        }
        std::cout << std::endl
                  << "0/" << deps.size() << " packages installed";
    }

    template <typename Async>
    int install_now(int* index_now, const Async& async_funcs) {
        if (*index_now >= static_cast<int>(io::cli::spinners.size()))
            *index_now = 0;

        // 0/num packages installed|
        // |0/num packages installed
        std::cout << io::cli::left(50)
                  << io::cli::up(1);

        std::cout << io::cli::up(async_funcs.size());

        int count = 0;
        for (const auto& fun : async_funcs) {
            // -1 is finished
            const int status = std::get<0>(fun).wait_for(std::chrono::milliseconds(0));
            if (status == -1) {
                if (std::get<2>(fun) == "notfound") {
                    rewrite(std::bind(&not_found), std::get<1>(fun));
                }
                else {
                    rewrite(std::bind(&installed, std::get<2>(fun)), std::get<1>(fun));
                }
                ++count;
            }
            else {
                const std::string now = [&status]() {
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
        namespace src = poac::sources;
        namespace io  = poac::io;

        const std::string url  = src::github::resolve(name, tag);
        const std::string filename = io::file::POAC_CACHE_DIR.c_str() + tag + ".tar.gz";
        io::network::get_file(url, filename);

        // ~/.poac/cache/package.tar.gz -> ~/.poac/cache/username-repository-tag/...
        io::file::extract_tar_spec(filename, connect_path(io::file::POAC_CACHE_DIR, make_name(get_name(name), tag)));
        fs::remove(filename);
    }

    static void _build(const std::string& name, const std::string& tag) {
        namespace fs     = boost::filesystem;
        namespace except = poac::core::except;

        std::string filename = connect_path(io::file::POAC_CACHE_DIR, make_name(get_name(name), tag));
        if (fs::exists(connect_path(filename, "CMakeLists.txt"))) {
            std::string command("cd " + filename + " && mkdir build && cd build && cmake .. 2>&1 && make 2>&1");

            std::array<char, 128> buffer;
            std::string result;

            FILE* pipe = popen(command.c_str(), "r");
            // operator bool
            if (!pipe) throw except::error("Couldn't start command.");
            while (std::fgets(buffer.data(), 128, pipe) != nullptr) {
                result += buffer.data();
            }
            [[maybe_unused]] int return_code = pclose(pipe);
        }
    }

    static void _copy(const std::string& name, const std::string& tag) {
        namespace fs = boost::filesystem;

        fs::create_directories("./deps");
        // Copy package to ./deps
        // If it exists in cache and it is not in the current directory copy it to the current.
        const std::string folder = make_name(get_name(name), tag);
        io::file::copy_dir(connect_path(io::file::POAC_CACHE_DIR, folder), "./deps/" + folder);
    }

    static void _placeholder() {}


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

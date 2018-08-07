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
#include <utility>
#include <tuple>
#include <fstream>
#include <regex>
#include <chrono>

#include <boost/filesystem.hpp>
#include <boost/timer/timer.hpp>
#include <yaml-cpp/yaml.h>

#include "../io.hpp"
#include "../core/except.hpp"
#include "../io/file.hpp"
#include "../sources.hpp"
#include "../util/package.hpp"


struct step_functions {
    const std::vector<std::function<void()>> funcs;
    const size_t size;
    std::future<void> mutable func_now;
    unsigned int mutable index = 0;

    template < typename ...Funcs >
    explicit step_functions(Funcs ...fs) : funcs({fs...}), size(sizeof...(Funcs)) {}

    void start() const { if (index == 0) run(); }
    void run()   const { func_now = std::async(std::launch::async, funcs[index]); }
    int  next()  const { ++index; run(); return index; }
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
// Version 1 will allow $ poac install <Nothing> only.
//   Parse poac.yml
// Version 2 will also allow $ poac install [<pkg-names>].
//   Parse arguments.
namespace poac::subcmd { struct install {
    static const std::string summary() { return "Beta: Install packages."; }
    static const std::string options() { return "<Nothing>"; }

    template <typename VS>
    void operator()(VS&& vs) { _main(vs); }

    static void info(const std::string& name, const std::string& tag) {
        std::cout << name << ": " << tag;
    }
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
    int installing(int *index_now, const Async &async_funcs) {
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
                const std::string now = [&status]() {
                    if (status == 0)      return "Downloading";
                    else if (status == 1) return "Building";
                    else if (status == 2) return "Copying";
                    else                  return "Error";
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


    static void _download(const std::string& url, const std::string& pkgname) {
        namespace fs  = boost::filesystem;
        namespace src = sources;

        const std::string pkg_dir = (io::file::path::poac_cache_dir / pkgname).string();
        const std::string tarname = pkg_dir + ".tar.gz";
        io::network::get_file(url, tarname);

        // ~/.poac/cache/package.tar.gz -> ~/.poac/cache/username-repository-tag/...
        io::file::tarball::extract_spec(tarname, pkg_dir);
        fs::remove(tarname);
    }

    static void _build(const std::string& pkgname) {
        namespace fs     = boost::filesystem;
        namespace except = core::except;

        const fs::path filepath = io::file::path::poac_cache_dir / pkgname;

        // TODO: LICENSEなどが消えてしまう．
        if (fs::exists(filepath / "CMakeLists.txt")) {
            std::string command("cd " + filepath.string());
            command += " && mkdir build";
            command += " && cd build";
            // TODO: MACOSX_RPATHはダメ． // TODO: cmake install_dir
            command += " && MACOSX_RPATH=1 OPENSSL_ROOT_DIR=/usr/local/opt/openssl/ cmake .. 2>&1"; // TODO: 抽象化
            command += " && make -j4 2>&1"; // TODO: 抽象化 // hardware_concurrency
            command += " && DESTDIR=./ make install 2>&1";

            std::array<char, 128> buffer;
            std::string result;

            FILE* pipe = popen(command.c_str(), "r");
            if (!pipe) throw except::error("Couldn't start command.");
            while (std::fgets(buffer.data(), 128, pipe) != nullptr)
                result += buffer.data();

            if (int return_code = pclose(pipe); return_code == 0) {
                const std::string filepath_tmp = filepath.string() + "_tmp";
                fs::rename(filepath, filepath_tmp);
                fs::create_directories(filepath);

                const fs::path build_after_dir(fs::path(filepath_tmp) / "build" / "usr" / "local");

                // Write to cache.yml and recurcive copy
                if (io::file::path::validate_dir(build_after_dir / "bin"))
                    io::file::path::recursive_copy(build_after_dir / "bin", fs::path(filepath) / "bin");
                if (io::file::path::validate_dir(build_after_dir / "include"))
                    io::file::path::recursive_copy(build_after_dir / "include", fs::path(filepath) / "include");
                if (io::file::path::validate_dir(build_after_dir / "lib"))
                    io::file::path::recursive_copy(build_after_dir / "lib", fs::path(filepath) / "lib");
                fs::remove_all(filepath_tmp);
            }
        }
        // Do not exists CMakeLists.txt, but ...
        else if (io::file::path::validate_dir(io::file::path::connect_path(filepath, "include"))) {
            const std::string filepath_tmp = filepath.string() + "_tmp";

            fs::rename(filepath, filepath_tmp);
            fs::create_directories(filepath);
            io::file::path::recursive_copy(fs::path(filepath_tmp) / "include", fs::path(filepath) / "include");
            fs::remove_all(filepath_tmp);
        }
        // TODO: manualに対応する
        else {
            const std::string filepath_tmp = filepath.string() + "_tmp";
            fs::rename(filepath, filepath_tmp);

            std::string command("cd " + filepath_tmp);
            command += " && ./bootstrap.sh";
            command += " && ./b2 install -j2 --prefix=" + filepath.string();

            std::array<char, 128> buffer;
            std::string result;

            FILE* pipe = popen(command.c_str(), "r");
            if (!pipe) throw except::error("Couldn't start command.");
            while (std::fgets(buffer.data(), 128, pipe) != nullptr)
                result += buffer.data();

            fs::remove_all(filepath_tmp);
        }
    }

    static void _copy(const std::string& pkgname) {
        namespace fs = boost::filesystem;

        fs::create_directories(io::file::path::current_deps_dir);
        // Copy package to ./deps
        // If it exists in cache and it is not in the current directory copy it to the current.
        io::file::path::recursive_copy(io::file::path::poac_cache_dir / pkgname, io::file::path::current_deps_dir / pkgname);
    }

    static void _placeholder() {}


    template <typename Async>
    void search_github(Async* async_funcs, const std::string& name, const std::string& version, const std::string& pkgname) {
        namespace src    = sources;

        if (src::cache::resolve(pkgname)) {
            async_funcs->emplace_back(
                    step_functions(
                            std::bind(&_copy, pkgname)
                    ),
                    std::bind(&info, name, version),
                    "cache"
            );
        }
        else if (src::github::installable(name, version)) {
            async_funcs->emplace_back(
                    step_functions(
                            std::bind(&_download, src::github::resolve(name, version), pkgname),
                            std::bind(&_build, pkgname),
                            std::bind(&_copy, pkgname)
                    ),
                    std::bind(&info, name, version),
                    "github"
            );
        }
        else {
            async_funcs->emplace_back(
                    step_functions(
                            std::bind(&_placeholder)
                    ),
                    std::bind(&info, name, version),
                    "notfound"
            );
        }
    }

    template <typename Async>
    void search_poac(Async* async_funcs, const std::string& name, const std::string& version, const std::string& pkgname) {
        namespace src    = sources;

        if (src::cache::resolve(pkgname)) {
            async_funcs->emplace_back(
                    step_functions(
                            std::bind(&_copy, pkgname)
                    ),
                    std::bind(&info, name, version),
                    "cache"
            );
        }
        else {
            async_funcs->emplace_back(
                    step_functions(
                            std::bind(&_placeholder)
                    ),
                    std::bind(&info, name, version),
                    "notfound"
            );
        }
    }

    template <typename Async>
    void search_tarball(Async* async_funcs, const std::string& url, const std::string& name, const std::string& version, const std::string& pkgname) {
        namespace src    = sources;

        if (src::cache::resolve(pkgname)) {
            async_funcs->emplace_back(
                    step_functions(
                            std::bind(&_copy, pkgname)
                    ),
                    std::bind(&info, name, version),
                    "cache"
            );
        }
        else {
            async_funcs->emplace_back(
                    step_functions(
                            std::bind(&_download, url, pkgname),
                            std::bind(&_build, pkgname),
                            std::bind(&_copy, pkgname)
                    ),
                    std::bind(&info, name, version),
                    "tarball"
            );
        }
    }

    template <typename Async>
    void dependencies(Async* async_funcs) {
        namespace fs     = boost::filesystem;
        namespace except = core::except;
        namespace src    = sources;

        // TODO: depsが無いとthrowされるはず. deps:だけ書かれていたら？
        const YAML::Node deps = io::file::yaml::get_node("deps");
        int already_count = 0;

        // TODO: 同じdepsが書かれている場合の対策が施されていない．
        for (YAML::const_iterator itr = deps.begin(); itr != deps.end(); ++itr) {
            const std::string name = itr->first.as<std::string>();
            bool exists_src_key = true;

            try { itr->second["src"]; }
            catch (...) { exists_src_key = false; }

            if (exists_src_key) {
                if (itr->second["src"].as<std::string>() == "github") {
                    const std::string version = itr->second["tag"].as<std::string>();
                    const std::string pkgname = util::package::github_conv_pkgname(name, version);

                    if (io::file::path::validate_dir(io::file::path::current_deps_dir / pkgname))
                        ++already_count;
                    else
                        search_github(async_funcs, name, version, pkgname);
                }
                else if (itr->second["src"].as<std::string>() == "tarball") {
                    const std::string version = "nothing";
                    const std::string pkgname = util::package::basename(name);

                    if (io::file::path::validate_dir(io::file::path::current_deps_dir / pkgname))
                        ++already_count;
                    else
                        search_tarball(async_funcs, itr->second["url"].as<std::string>(), name, version, pkgname);
                }
                else {
                    throw except::error("poac.yml error\nWhat source is " + itr->second["src"].as<std::string>() + "?");
                }
            }
            else {
                const std::string version = itr->second.as<std::string>();
                const std::string pkgname = util::package::github_conv_pkgname(name, version);

                if (io::file::path::validate_dir(io::file::path::current_deps_dir / pkgname))
                    ++already_count;
                else
                    search_poac(async_funcs, name, version, pkgname);
            }
        }

        if (async_funcs->empty()) {
            if (already_count > 0)
                throw except::warn("Already up-to-date");
            else
                throw except::invalid_second_arg("install");
        }

        // Start async functions...
        for (const auto& [func, _info, _src] : *async_funcs) {
            func.start();
            ((void)_info, (void)_src); // Avoid unused warning
        }
    }

    void check_requirements() {
        namespace fs     = boost::filesystem;
        namespace except = core::except;

        // Auto generate poac.yml on Version 2.
        if (!io::file::yaml::notfound_handle()) throw except::invalid_second_arg("install");
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
     */
    template <typename VS>
    void _main(VS&& vs) {
        namespace fs     = boost::filesystem;
        namespace except = core::except;

        // Start timer
        boost::timer::cpu_timer timer;


        check_arguments(vs);
        check_requirements();

        std::vector<std::tuple<step_functions, std::function<void()>, std::string>> async_funcs;
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
                  << timer.format(6, "Elapsed time: %ws")
                  << std::endl
                  << io::cli::bold
                  << " ==> Installation finished successfully!"
                  << std::endl;
    }
};} // end namespace
#endif // !POAC_SUBCMD_INSTALL_HPP

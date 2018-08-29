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

// TODO: 妙なprogressを出さないoptionが欲しい．CIや，Docker用
// TODO: --simple-progressみたいな -> elm-package installのoutputに寄せる
// TODO: --quiteも必要．
namespace poac::subcmd { struct install {
    static const std::string summary() { return "Beta: Install packages."; }
    static const std::string options() { return "<Nothing>"; }

        template <typename VS, typename = std::enable_if_t<std::is_rvalue_reference_v<VS&&>>>
    void operator()(VS&& vs) { _main(vs); }

    static void info(const std::string& name, const std::string& version) {
        std::cout << name << ": " << version;
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
    int installing(int* index_now, const Async& async_funcs) {
        *index_now %= static_cast<int>(io::cli::spinners.size());

        // 0/num packages installed|
        // |0/num packages installed
        std::cout << io::cli::left(50) << io::cli::up(1);
        std::cout << io::cli::up(async_funcs.size());

        int count = 0;
        for (const auto& [func, info_func, src] : async_funcs) {
            const std::string status = func.wait_for(std::chrono::milliseconds(0));
            if (status == "Notfound") {
                rewrite(std::bind(&not_found), info_func);
                ++count;
            }
            else if (status == "Done") {
                rewrite(std::bind(&installed, src), info_func);
                ++count;
            }
            else if (status == "Error") {
                rewrite(std::bind(&install_failed), info_func);
                ++count;
            }
            else {
                rewrite(std::bind(&progress, *index_now, status, src), info_func);
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


    // TODO: LICENSEなどが消えてしまう
    static void _cmake_build(
            const std::string& pkgname,
            const std::map<std::string, std::string>& cmake_envs)
    {
        namespace fs     = boost::filesystem;
        namespace except = core::exception;

        const fs::path filepath = io::file::path::poac_cache_dir / pkgname;

        util::command cmd("cd " + filepath.string());
        cmd &= "mkdir build";
        cmd &= "cd build";
        util::command cmake_cmd("cmake ..");
        for (const auto& [key, val] : cmake_envs)
            cmake_cmd.env(key, val);
        cmd &= cmake_cmd.std_err();
        cmd &= util::command("make -j4").std_err();
        cmd &= util::command("make install").env("DESTDIR", "./").std_err();

        if (auto result = cmd.run()) {
            const std::string filepath_tmp = filepath.string() + "_tmp";
            fs::rename(filepath, filepath_tmp);
            fs::create_directories(filepath);

            const fs::path build_after_dir(fs::path(filepath_tmp) / "build" / "usr" / "local");

            // Write to cache.yml and recurcive copy
            for (const auto& s : std::vector<std::string>({ "bin", "include", "lib" }))
                if (io::file::path::validate_dir(build_after_dir / s))
                    io::file::path::recursive_copy(build_after_dir / s, fs::path(filepath) / s);
            fs::remove_all(filepath_tmp);
        }
        else {
            /* error */
            // datetime-error.log
            // return EXIT_FAILURE; ???
            // 下のmanual_buildでも同じことをする羽目になるので，return EXIT_...だけして，あとは，step_funcsで処理すべき
            // もしくは，
        }
    }
    // TODO: きちんとカレントディレクトリにコピーされなくても，installed!と表示されてしまう．
    // TODO: 上のは，copyの問題？？？
    static void _manual_build(const std::string& pkgname, const util::command& cmd) {
        namespace fs     = boost::filesystem;
        namespace except = core::exception;

        const fs::path filepath = io::file::path::poac_cache_dir / pkgname;

        const std::string filepath_tmp = filepath.string() + "_tmp";
        fs::rename(filepath, filepath_tmp);

        if (auto result = cmd.run()) {
            // TODO: boost build is return 1 always
//        fs::remove_all(filepath_tmp);
        }
        else { /* error */ }
        fs::remove_all(filepath_tmp);
    }
    // Copy include directory only
    [[maybe_unused]] static void _header_only(const std::string& pkgname) {
        namespace fs = boost::filesystem;
        const fs::path filepath = io::file::path::poac_cache_dir / pkgname;

        const std::string filepath_tmp = filepath.string() + "_tmp";

        fs::rename(filepath, filepath_tmp);
        fs::create_directories(filepath);
        io::file::path::recursive_copy(fs::path(filepath_tmp) / "include", fs::path(filepath) / "include");
        fs::remove_all(filepath_tmp);
    }
    static void _copy(const std::string& pkgname) {
        namespace fs = boost::filesystem;

        fs::create_directories(io::file::path::current_deps_dir);
        // Copy package to ./deps
        // If it exists in cache and it is not in the current directory copy it to the current.
        io::file::path::recursive_copy(io::file::path::poac_cache_dir / pkgname, io::file::path::current_deps_dir / util::package::cache_to_current(pkgname));
    }
    static void _placeholder() {}


    // build system(cmake | manual | none)
    boost::optional<std::string> resolve_build_system(const YAML::Node& node) {
        if (const auto build_system = io::file::yaml::get<std::string>(node, "build"))
            return build_system;
        else if (const auto build_system2 = io::file::yaml::get2<std::string>(node, "build", "system"))
            return build_system2;
        else
            return boost::none;
    }

    std::string resolve_source(const std::string& pkgname, const std::string& source) {
        namespace src = sources;
        if (src::current::resolve(pkgname))
            return "current";
        else if (src::cache::resolve(pkgname))
            return "cache";
        else
            return source;
    }

    boost::optional<util::step_funcs_with_status> resolve(
            const YAML::Node& node,
            const std::string& name,
            const std::string& version,
            const std::string& pkgname,
            const std::string& source)
    {
        namespace fs  = boost::filesystem;
        namespace src = sources;

        const std::string url = src::github::resolve(name, version);

        if (source == "current") {
            return boost::none;
        }
        else if (source == "cache") {
            return util::step_funcs_with_status(std::make_tuple("Copying", std::bind(&_copy, pkgname)));
        }
        // TODO: manualの場合，installableではチェックできない．
//    else if ((source == "github") ? github::installable(name, version) : true) {
        else if (source != "poac") {
            std::vector<std::tuple<std::string, std::function<void()>>> func_pack;

            if (source == "github") {
                const std::string url = src::github::resolve(name);
                const fs::path dest = io::file::path::poac_cache_dir / util::package::github_conv_pkgname(name, version);

                std::map<std::string, std::string> opts;
                opts.insert(io::network::opt_depth(1));
                opts.insert(io::network::opt_branch(version));

                func_pack.emplace_back("Cloning", std::bind(&io::network::clone, url, dest, opts));
            }
            else {
                namespace tb = io::file::tarball;

                const fs::path pkg_dir = io::file::path::poac_cache_dir / pkgname;
                const fs::path tarname = pkg_dir.string() + ".tar.gz";

                func_pack.emplace_back("Downloading", std::bind(&io::network::get_file, url, tarname));
                func_pack.emplace_back("Extracting", std::bind(&tb::extract_spec_rm_file, tarname, pkg_dir));
            }

            if (const auto build_system = resolve_build_system(node)) {
                if (*build_system == "cmake") {
                    if (const auto cmake_envs = io::file::yaml::get2<std::map<std::string, std::string>>(node, "build", "environment"))
                        func_pack.emplace_back("Building", std::bind(&_cmake_build, pkgname, *cmake_envs));
                    else
                        func_pack.emplace_back("Building", std::bind(&_cmake_build, pkgname, std::map<std::string, std::string>()));
                    func_pack.emplace_back("Copying", std::bind(&_copy, pkgname));
                    return util::step_funcs_with_status(std::move(func_pack));
                }
                else if (*build_system == "manual") {
                    if (const auto steps = io::file::yaml::get2<std::vector<std::string>>(node, "build", "steps")) {
                        func_pack.emplace_back("Building", std::bind(&_manual_build, pkgname, util::command(*steps)));
                        func_pack.emplace_back("Copying", std::bind(&_copy, pkgname));
                        return util::step_funcs_with_status(std::move(func_pack));
                    }
                }
            }
        }
        // TODO: すぐにDoneになってしまう．
        return util::step_funcs_with_status(std::make_tuple("Notfound", std::bind(&_placeholder)));
    }

    template <typename Async>
    void dependencies(Async* async_funcs) {
        namespace fs     = boost::filesystem;
        namespace except = core::exception;
        namespace src    = sources;


        const YAML::Node deps = io::file::yaml::get_node("deps");
        int already_count = 0;

        // TODO: 同じdepsが書かれている場合の対策が施されていない．
        for (YAML::const_iterator itr = deps.begin(); itr != deps.end(); ++itr) {
            // hello_world: 0.2.1
            // itr->first: itr->second
            const std::string name = itr->first.as<std::string>();
            std::string src     = util::package::get_source(itr->second);
            const std::string version = util::package::get_version(itr->second, src);
            const std::string pkgname = util::package::github_conv_pkgname(name, version);

            src = resolve_source(pkgname, src);
            if (auto func_pack = resolve(itr->second, name, version, pkgname, src))
                async_funcs->emplace_back(std::move(*func_pack), std::bind(&info, name, version), src);
            else
                ++already_count;
        }

        if (async_funcs->empty()) {
            if (already_count > 0)
                throw except::warn("Already up-to-date");
            else // TODO: When deps is not written in poac.yml
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
        namespace except = core::exception;

        // Auto generate poac.yml on Version 2.
        if (!io::file::yaml::exists()) throw except::error("poac.yml is not found");
        fs::create_directories(io::file::path::poac_cache_dir);
    }

    void check_arguments(const std::vector<std::string>& argv) {
        namespace except = core::exception;
        if (!argv.empty()) throw except::invalid_second_arg("install");
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
    void _main(VS&& argv) {
        namespace fs     = boost::filesystem;
        namespace except = core::exception;

        // Start timer
        boost::timer::cpu_timer timer; // TODO: 全てのコマンドにおいて計測したい


        check_arguments(argv);
        check_requirements();

        std::vector<
                std::tuple<
                        util::step_funcs_with_status,
                        std::function<void()>,
                        std::string
                        >> async_funcs;
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

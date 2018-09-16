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
    void operator()(VS&& argv) { _main(std::move(argv)); }


    template <typename Async>
    int installing(int* index_now, Async& async_funcs) {
        // 0/num packages installed|
        // |0/num packages installed
        std::cout << io::cli::left(50) << io::cli::up(1);
        std::cout << io::cli::up(async_funcs.size());

        int count = 0;
        for (auto& [func, info] : async_funcs) {
            const std::string status = func.wait_for(std::chrono::milliseconds(0));

            std::cout << io::cli::right(1) << "\b";
            if (func.is_done()) {
                io::cli::set_left(50); // 最終的に，左側が50文字になれば良い．その余り分を空白で埋めれば良い．
                std::cout << status;
                ++count;
            }
            else {
                io::cli::set_left(41);
                std::cout << " " + io::cli::at_spinner(*index_now) +
                             "  " + status;
            }
            std::cout << info
                      << io::cli::left(100)
                      << io::cli::down(1);
        }
        std::cout << std::endl
                  << io::cli::right(1)
                  << '\b'
                  << count
                  << std::flush;
        return count;
    }


    // TODO: LICENSEなどが消えてしまう
    static bool _cmake_build(
            const std::string& pkgname,
            const std::map<std::string, std::string>& cmake_envs )
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
        cmd &= cmake_cmd.stderr_to_stdout();
        cmd &= util::command("make -j4").stderr_to_stdout();
        cmd &= util::command("make install").env("DESTDIR", "./").stderr_to_stdout();

        if (auto result = cmd.exec()) {
            const std::string filepath_tmp = filepath.string() + "_tmp";
            fs::rename(filepath, filepath_tmp);
            fs::create_directories(filepath);

            const fs::path build_after_dir(fs::path(filepath_tmp) / "build" / "usr" / "local");

            // Write to cache.yml and recurcive copy
            for (const auto& s : std::vector<std::string>({ "bin", "include", "lib" }))
                if (io::file::path::validate_dir(build_after_dir / s))
                    io::file::path::recursive_copy(build_after_dir / s, fs::path(filepath) / s);
            fs::remove_all(filepath_tmp);

            return EXIT_SUCCESS;
        }
        else {
            /* error */
            // datetime-error.log
            return EXIT_FAILURE;
        }
    }
    static bool _manual_build(const std::string& pkgname, util::command& cmd) {
        namespace fs     = boost::filesystem;
        namespace except = core::exception;

        const fs::path filepath = io::file::path::poac_cache_dir / pkgname;

        const std::string filepath_tmp = filepath.string() + "_tmp";
        fs::rename(filepath, filepath_tmp);

        cmd.stderr_to_stdout();
        if (auto result = cmd.exec()) {
            // TODO: boost build is return 1 always
//        fs::remove_all(filepath_tmp);
        }
        else { /* error */ }
        fs::remove_all(filepath_tmp);
        return EXIT_SUCCESS;
    }
    // Copy include directory only
//    [[maybe_unused]] static void _header_only(const std::string& pkgname) {
//        namespace fs = boost::filesystem;
//        const fs::path filepath = io::file::path::poac_cache_dir / pkgname;
//
//        const std::string filepath_tmp = filepath.string() + "_tmp";
//
//        fs::rename(filepath, filepath_tmp);
//        fs::create_directories(filepath);
//        io::file::path::recursive_copy(fs::path(filepath_tmp) / "include", fs::path(filepath) / "include");
//        fs::remove_all(filepath_tmp);
//    }
    static bool _copy(const std::string& pkgname) {
        // Copy package to ./deps
        // If it exists in cache and it is not in the current directory copy it to the current.
        return io::file::path::recursive_copy(
                io::file::path::poac_cache_dir / pkgname,
                io::file::path::current_deps_dir / util::package::cache_to_current(pkgname)
        );
    }
    static bool _placeholder() { return EXIT_SUCCESS; }


    // build system(cmake | manual | none)
    boost::optional<std::string> resolve_build_system(const YAML::Node& node) {
        if (const auto build_system = io::file::yaml::get<std::string>(node))
            return *build_system;
        else if (const auto build_system2 = io::file::yaml::get1<std::string>(node, "system"))
            return *build_system2;
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
            util::step_funcs_with_status step_funcs;
            step_funcs.funcs.emplace_back("Copying (from " + source + ")", std::bind(&_copy, pkgname));
            step_funcs.error_msg = io::cli::to_red(" ×  Install failed");
            step_funcs.finish_msg = io::cli::to_green(" ✔  Installed! (from " + source + ")");
            return step_funcs;
        }
        else if (source != "poac") {
            util::step_funcs_with_status step_funcs;

            if (source == "github") {
                const std::string url = src::github::resolve(name);
                const fs::path dest = io::file::path::poac_cache_dir / util::package::github_conv_pkgname(name, version);

                std::map<std::string, std::string> opts;
                opts.insert(io::network::opt_depth(1));
                opts.insert(io::network::opt_branch(version));

                step_funcs.funcs.emplace_back(
                        "Cloning (from " + source + ")",
                        std::bind(&io::network::clone, url, dest, opts)
                );
            }
            else {
                namespace tb = io::file::tarball;

                const fs::path pkg_dir = io::file::path::poac_cache_dir / pkgname;
                const fs::path tarname = pkg_dir.string() + ".tar.gz";

                step_funcs.funcs.emplace_back(
                        "Downloading (from " + source + ")",
                        std::bind(&io::network::get_file, url, tarname)
                );
                step_funcs.funcs.emplace_back(
                        "Extracting (from " + source + ")",
                        std::bind(&tb::extract_spec_rm_file, tarname, pkg_dir)
                );
            }

            // TODO: もっと美しく
            if (const auto build_config = io::file::yaml::get_by_depth(node, "build")) {
                if (const auto build_system = resolve_build_system(node)) {
                    if (*build_system == "cmake") {
                        if (const auto cmake_envs = io::file::yaml::get1<std::map<std::string, std::string>>(node, "environment"))
                            step_funcs.funcs.emplace_back(
                                    "Building (from " + source + ")",
                                    std::bind(&_cmake_build, pkgname, *cmake_envs)
                            );
                        else
                            step_funcs.funcs.emplace_back(
                                    "Building (from " + source + ")",
                                    std::bind(&_cmake_build, pkgname, std::map<std::string, std::string>())
                            );
                        step_funcs.funcs.emplace_back(
                                "Copying (from " + source + ")",
                                std::bind(&_copy, pkgname)
                        );

                        step_funcs.error_msg = io::cli::to_red(" ×  Install failed");
                        step_funcs.finish_msg = io::cli::to_green(" ✔  Installed! (from " + source + ")");
                        return step_funcs;
                    }
                    else if (*build_system == "manual") {
                        if (const auto steps = io::file::yaml::get1<std::vector<std::string>>(node, "steps")) {
                            step_funcs.funcs.emplace_back(
                                    "Building (from " + source + ")",
                                    std::bind(&_manual_build, pkgname, util::command(*steps))
                            );
                            step_funcs.funcs.emplace_back(
                                    "Copying (from " + source + ")",
                                    std::bind(&_copy, pkgname)
                            );

                            step_funcs.error_msg = io::cli::to_red(" ×  Install failed");
                            step_funcs.finish_msg = io::cli::to_green(" ✔  Installed! (from " + source + ")");
                            return step_funcs;
                        }
                    }
                }
            }
        }
        util::step_funcs_with_status step_funcs;
        step_funcs.funcs.emplace_back("Notfound", std::bind(&_placeholder));
        step_funcs.finish_msg = io::cli::to_red(" ×  Not found");
        return step_funcs;
    }

    template <typename Async>
    void dependencies(Async* async_funcs, const YAML::Node& node) {
        namespace except = core::exception;

        int already_count = 0;
        // Even if a package of the same name is written, it is excluded.
        // However, it can not deal with duplication of other information (e.g. version etc.).
        for (const auto& [name, next_node] : node.as<std::map<std::string, YAML::Node>>()) {
            // hello_world: 0.2.1
            // itr->first: itr->second
            std::string src = util::package::get_source(next_node);
            const std::string version = util::package::get_version(next_node, src);
            const std::string pkgname = util::package::github_conv_pkgname(name, version);

            src = resolve_source(pkgname, src);
            if (auto func_pack = resolve(next_node, name, version, pkgname, src))
                async_funcs->emplace_back(std::move(*func_pack), name + ": " + version);
            else
                ++already_count;
        }

        if (async_funcs->empty()) {
            if (already_count > 0)
                throw except::warn("Already up-to-date");
            else
                throw except::invalid_second_arg("install");
        }
    }

    template <typename Async>
    void start_funcs(Async& async_funcs) {
        namespace fs = boost::filesystem;
        fs::create_directories(io::file::path::current_deps_dir);
        // TODO: hardware concurrency
        for (auto& [func, _info] : async_funcs) {
            func.start();
            (void)_info; // Avoid unused warning
        }
    }

    void check_arguments(const std::vector<std::string>& argv) {
        namespace except = core::exception;
        if (!argv.empty()) throw except::invalid_second_arg("install");
    }

    template <typename VS, typename = std::enable_if_t<std::is_rvalue_reference_v<VS&&>>>
    void _main(VS&& argv) {
        namespace fs = boost::filesystem;

        // Start timer
        // TODO: 全てのコマンドにおいて計測したい (もう一段階抽象化後)
        boost::timer::cpu_timer timer;


        check_arguments(argv);
        fs::create_directories(io::file::path::poac_cache_dir);
        const auto node = io::file::yaml::load_setting_file("deps");

        std::vector<
            std::tuple<
                util::step_funcs_with_status,
                std::string
            >
        > async_funcs;
        dependencies(&async_funcs, node.at("deps"));
        start_funcs(async_funcs);

        const int deps_num = static_cast<int>(async_funcs.size());

        std::cout << "Some new packages are needed.\n\n";
        for (int i = 0; i < deps_num; ++i)
            std::cout << std::endl;
        std::cout << std::endl
                  << "0/" << deps_num << " packages installed";


        for (int i = 0; installing(&i, async_funcs) != deps_num; ++i)
            std::this_thread::sleep_for(std::chrono::milliseconds(100));


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

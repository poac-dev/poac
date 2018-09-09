#ifndef POAC_SUBCMD_BUILD_HPP
#define POAC_SUBCMD_BUILD_HPP

#include <iostream>
#include <string>
#include <map>
#include <regex>
#include <stdexcept>

#include <boost/filesystem.hpp>
#include <boost/range/iterator_range.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>

#include "../core/exception.hpp"
#include "../io/file.hpp"
#include "../io/cli.hpp"
#include "../util/buildsystem.hpp"
#include "../util/package.hpp"


// TODO: --backend cmake, ninja, ...
// TODO: ld: symbol(s) not found for architecture x86_64
// TODO: clang: error: linker command failed with exit code 1 (use -v to see invocation)
// TODO: こういったエラーの時は，depsにファイルがあるのか，確認 message を output する．
namespace poac::subcmd { struct build {
    static const std::string summary() { return "Beta: Compile all sources that depend on this project."; }
    static const std::string options() { return "[-v | --verbose]"; } // TODO: --no-cache --release


    template <typename VS, typename = std::enable_if_t<std::is_rvalue_reference_v<VS&&>>>
    void operator()(VS&& argv) { _main(std::move(argv)); }
    template <typename VS, typename = std::enable_if_t<std::is_rvalue_reference_v<VS&&>>>
    void _main(VS&& argv) {
        namespace fs     = boost::filesystem;
        namespace except = core::exception;

        check_arguments(argv);
        check_requirements();
        // TODO: node必要？？
        const auto node = io::file::yaml::load_setting_file("build");

        const auto first = argv.begin(), last = argv.end();
        const bool verbose = (std::find(first, last, "-v") != last || std::find(first, last, "--verbose") != last);

        util::buildsystem bs;

        // TODO: poac.yml compiler: -> error version outdated

        // TODO: runの時に，build: bin:true なければ，かつ，./main.cppが無ければ，runはエラーになる．
        // TODO: もしくは，main.cppじゃなくても，バイナリを生成するソースをpoac.ymlから指定できるようにしておく．
        // TODO: curl-configのように．
        if (const auto bin = io::file::yaml::get_by_depth(node.at("build"), "bin")) {
            if ((*bin).as<bool>()) {
                fs::create_directories(io::file::path::current_build_bin_dir);
                fs::create_directories(io::file::path::current_build_cache_obj_dir);
                fs::create_directories(io::file::path::current_build_cache_hash_dir);

                // TODO: このinfoもbuildsystemに管理させる
                // TODO: -> すると，エラーの時だけ，boost::noneを返して，それ以外は，CompiledとWarningを選択できる
                if (const auto bin_path = bs.build_bin(true, verbose)) {
                    std::cout << io::cli::green << "Compiled: " << io::cli::reset
                              << "Output to `" + fs::relative(*bin_path).string() + "`"
                              << std::endl;
                }
                else { /* error */
                    // TODO: compileに失敗時も表示されてしまう．
                    // TODO: コンパイル失敗後(linkも含む)に，もう一度する時に，hashが残って，
                    // TODO: コンパイルできないので，いずれかの失敗があれば，hashは削除すべき
                    // TODO: bin_pathを取得できない！！！
                    std::cout << io::cli::yellow << "Warning: " << io::cli::reset
                              << "There is no change. Binary exists in `" + fs::relative("").string() + "`."
                              << std::endl;
                }
            }
        }
        // TODO: runの時はいらない
        if (const auto lib = io::file::yaml::get_by_depth(node.at("build"), "lib")) {
            if ((*lib).as<bool>()) {
                if (const auto stlib_path = bs.build_stlink_lib(verbose)) {
                    std::cout << io::cli::green << "Generated: " << io::cli::reset
                              << "Output to `" +
                                 fs::relative(*stlib_path).string() +
                                 ".a" + "`"
                              << std::endl;
                }
                else { /* error */
                    // TODO: stlib_pathを取得できない！！！
                    std::cout << io::cli::yellow << "Warning: " << io::cli::reset
                              << "There is no change. Static library exists in `" +
                                 fs::relative("").string() +
                                 ".a" + "`."
                              << std::endl;
                }
                if (const auto dylink_path = bs.build_dylink_lib(verbose)) {
                    std::cout << io::cli::green << "Generated: " << io::cli::reset
                              << "Output to `" +
                                 fs::relative(*dylink_path).string() + ".dylib" + "`"
                              << std::endl;
                }
                else { /* error */
                    std::cout << io::cli::yellow << "Warning: " << io::cli::reset
                              << "There is no change. Dynamic library exists in `" +
                                 fs::relative("").string() + ".dylib" + "`."
                              << std::endl;
                }
            }
        }
    }


    void check_requirements() {
        namespace fs     = boost::filesystem;
        namespace except = core::exception;
    }

    void check_arguments(const std::vector<std::string>& argv) {
        namespace except = core::exception;

        if (argv.size() > 1)
            throw except::invalid_second_arg("build");
    }
};} // end namespace
#endif // !POAC_SUBCMD_BUILD_HPP

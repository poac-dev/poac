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
#include "../core/naming.hpp"
#include "../util/argparse.hpp"


namespace poac::subcmd { struct build {
    static const std::string summary() { return "Beta: Compile all sources that depend on this project."; }
    static const std::string options() { return "[-v | --verbose]"; } // TODO: --release, --no-cache


    template <typename VS, typename = std::enable_if_t<std::is_rvalue_reference_v<VS&&>>>
    void operator()(VS&& argv) { _main(std::move(argv)); }
    template <typename VS, typename = std::enable_if_t<std::is_rvalue_reference_v<VS&&>>>
    void _main(VS&& argv) {
        namespace fs = boost::filesystem;
        namespace except = core::exception;
        namespace naming = core::naming;

        check_arguments(argv);
        check_requirements();
        const auto node = io::file::yaml::load_setting_file("build", "name");
        const bool verbose = util::argparse::use(argv, "-v", "--verbose");

        if (const auto deps_node = io::file::yaml::load_setting_file_opt("deps")) {
            for (const auto& [name, next_node] : (*deps_node).at("deps").as<std::map<std::string, YAML::Node>>()) {
                // TODO: ./deps/name/poac.ymlに，buildの項がなければ，header only library
                // install時にpoac.ymlは必ず作成されるため，存在する前提で扱う
                const auto [src, name2] = naming::get_source(name);
                const std::string version = naming::get_version(next_node, src);
                const std::string current_package_name = naming::to_current(src, name, version);
                const auto deps_path = fs::current_path() / "deps" / current_package_name;

                bool do_build = true;
                if (const auto deps_yml_file = io::file::yaml::exists_setting_file(deps_path)) {
                    try {
                        const auto test1 = *io::file::yaml::load(*deps_yml_file);
                        const auto test2 = test1["build"];
                        (void)test2;
                    }
                    catch (...) {
                        do_build = false;
                    }
                }

                if (do_build && fs::exists(deps_path)) {
                    std::string comd = "cd " + deps_path.string() + " && poac build";
                    for (const auto& s : argv)
                        comd += " " + s;
                    std::system(comd.c_str());
                    // TODO: できればコマンドじゃないほうが良い. linkせずに使われてたらエラーになってしまう．
                    // TODO: また，Generated: Output to `_build/lib/shell-cpp.a`
                    // TODO: と，表示されるが，カレントではなく，./deps/pkg/_build に作成されてしまうため，
                    // TODO: 直すべき
                }
                else if (do_build) {
                    throw except::error(name + " is not installed.\n"
                                        "Please build after running `poac install`");
                }
            }
        }

        util::buildsystem bs;
        if (const auto bin = io::file::yaml::get_by_width(node.at("build"), "bin")) {
            if (const auto op_bin = io::file::yaml::get<bool>((*bin).at("bin"))) {
                if (*op_bin) {
                    if (build_bin(bs, true, verbose)) {} // TODO: ディレクトリで指定できるようにする？？？
                    else { /* compile or link error */
                        // 一度コンパイルに成功した後にpoac runを実行し，コンパイルに失敗しても実行されるエラーの回避

//                        std::cout << "hogegegegegeg" << std::endl;

                        const std::string project_name = node.at("name").as<std::string>();
                        const fs::path executable_path = fs::relative(io::file::path::current_build_bin_dir / project_name);
                        try { fs::remove(executable_path); }
                        catch (...) {} // ファイルが存在しない場合を握りつぶす
                    }
                }
            }
        }
        if (const auto lib = io::file::yaml::get_by_width(node.at("build"), "lib")) {
            if (const auto op_lib = io::file::yaml::get<bool>((*lib).at("lib"))) {
                if (*op_lib) {
                    if (build_link_libs(bs, verbose)) {}
                    else { /* compile or gen error */ }
                }
            }
        }
    }

    boost::optional<std::string> build_bin(
        util::buildsystem& bs,
        const bool usemain=false,
        const bool verbose=false )
    {
        namespace fs = boost::filesystem;

        bs.configure_compile(usemain, verbose);
        // Since the obj file already exists and has not been changed as a result
        //  of verification of the hash file, return only the list of existing obj_files
        //  and do not compile.
        // There is no necessity of linking that there is no change completely.
        if (bs.compile_conf.source_files.empty()) { // No need for compile and link
            const std::string bin_path =
                    (io::file::path::current_build_bin_dir / bs.project_name).string();
            // Dealing with an error which is said to have cache even though it is not going well.
            if (fs::exists(bin_path)) {
                std::cout << io::cli::yellow << "Warning: " << io::cli::reset
                          << "There is no change. Binary exists in `" +
                             fs::relative(bin_path).string() + "`."
                          << std::endl;
            }
            return bin_path;
        }
        else {
            if (const auto obj_files_path = bs._compile()) {
                bs.configure_link(*obj_files_path, verbose);
                if (const auto bin_path = bs._link()) {
                    std::cout << io::cli::green << "Compiled: " << io::cli::reset
                              << "Output to `" +
                                 fs::relative(*bin_path).string() +
                                 "`"
                              << std::endl;
                    return bin_path;
                }
                else { // Link failure
                    // TODO: 全部削除すると，testのcacheも消えてしまう．
                    fs::remove_all(io::file::path::current_build_cache_dir);
                    return boost::none;
                }
            }
            else { // Compile failure
                return boost::none;
            }
        }
    }

    boost::optional<std::string> build_link_libs(
        util::buildsystem& bs,
        const bool verbose = false )
    {
        namespace fs = boost::filesystem;

        bs.configure_compile(false, verbose);
        if (bs.compile_conf.source_files.empty()) { // No need for compile and link
            const std::string lib_path =
                    (io::file::path::current_build_lib_dir / bs.project_name).string();
            if (fs::exists(lib_path)) {
                std::cout << io::cli::yellow << "Warning: " << io::cli::reset
                          << "There is no change. Static link library exists in `" +
                             fs::relative(lib_path).string() +
                             ".a" + "`."
                          << std::endl;
                std::cout << io::cli::yellow << "Warning: " << io::cli::reset
                          << "There is no change. Dynamic link library exists in `" +
                             fs::relative(lib_path).string() +
                             ".dylib" + "`."
                          << std::endl;
            }
            return lib_path;
        }
        if (const auto obj_files_path = bs._compile()) {
            bs.configure_static_lib(*obj_files_path, verbose);
            if (const auto stlib_path = bs._gen_static_lib()) {
                std::cout << io::cli::green << "Generated: " << io::cli::reset
                          << "Output to `" +
                             fs::relative(*stlib_path).string() +
                             "`"
                          << std::endl;
//                return lib_path;
            }
            else { // Static link library generation failed
                // TODO: 全部削除すると，testのcacheも消えてしまう．
                fs::remove_all(io::file::path::current_build_cache_dir);
//                return boost::none;
            }

            bs.configure_dynamic_lib(*obj_files_path, verbose);
            if (const auto dylib_path = bs._gen_dynamic_lib()) {
                std::cout << io::cli::green << "Generated: " << io::cli::reset
                          << "Output to `" +
                             fs::relative(*dylib_path).string() +
                             "`"
                          << std::endl;
//                return lib_path;
            }
            else { // Dynamic link library generation failed
                // TODO: 全部削除すると，testのcacheも消えてしまう．
                fs::remove_all(io::file::path::current_build_cache_dir);
//                return boost::none;
            }

            // TODO:
            return boost::none;
        }
        else { // Compile failure
            return boost::none;
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

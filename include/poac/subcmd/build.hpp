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


// TODO: --release, --no-cache
namespace poac::subcmd {
    namespace _build {
        boost::optional<std::string>
        build_bin(util::buildsystem& bs, const bool verbose)
        {
            namespace fs = boost::filesystem;

            bs.configure_compile(true, verbose);
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

        boost::optional<std::string>
        build_link_libs(util::buildsystem& bs, const bool verbose)
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


        void build_deps() {
            namespace fs = boost::filesystem;
            namespace except = core::exception;
            namespace naming = core::naming;

            // TODO: lockファイル実装後，順序を決定
            if (const auto deps_node = io::file::yaml::load_config_opt("deps")) {
                for (const auto& [name, next_node] : (*deps_node).at("deps").as<std::map<std::string, YAML::Node>>()) {
                    // TODO: ./deps/name/poac.ymlに，buildの項がなければ，header only library
                    // install時にpoac.ymlは必ず作成されるため，存在する前提で扱う
                    const auto[src, name2] = naming::get_source(name);
                    const std::string version = naming::get_version(next_node, src);
                    const std::string current_package_name = naming::to_current(src, name2, version);
                    const auto deps_path = fs::current_path() / "deps" / current_package_name;

                    bool do_build = true;
                    if (const auto deps_yml_file = io::file::yaml::exists_config(deps_path)) {
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
//                        for (const auto& s : argv)
//                            comd += " " + s;
                        std::system(comd.c_str());
                        // TODO: できればコマンドじゃないほうが良い. linkせずに使われてたらエラーになってしまう．
                        // TODO: また，Generated: Output to `_build/lib/shell-cpp.a`
                        // TODO: と，表示されるが，カレントではなく，./deps/pkg/_build に作成されてしまうため，
                        // TODO: Generated: Output to... の出力の方を直すべき
                    }
                    else if (do_build) {
                        throw except::error(name + " is not installed.\n"
                                                   "Please build after running `poac install`");
                    }
                }
            }
        }


        template<typename VS, typename = std::enable_if_t<std::is_rvalue_reference_v<VS&&>>>
        void _main(VS&& argv) {
            namespace fs = boost::filesystem;
            namespace except = core::exception;
            namespace naming = core::naming;

            const auto node = io::file::yaml::load_config();
            const bool verbose = util::argparse::use(argv, "-v", "--verbose");

//            build_deps();

            util::buildsystem bs;
            if (io::file::yaml::get(node, "build", "lib")) {
                if (!build_link_libs(bs, verbose)) {
                    // compile or gen error
                }
            }
            if (io::file::yaml::get(node, "build", "bin")) {
                // TODO: ディレクトリで指定できるようにする？？？
                if (!build_bin(bs, verbose)) {
                    // compile or link error

                    // 一度コンパイルに成功した後にpoac runを実行し，コンパイルに失敗しても実行されるエラーの回避
                    if (const auto project_name = io::file::yaml::get<std::string>(node, "name")) {
                        const auto binary_name = io::file::path::current_build_bin_dir / *project_name;
                        const fs::path executable_path = fs::relative(binary_name);
                        try { fs::remove(executable_path); }
                        catch (...) {} // ファイルが存在しない場合を握りつぶす
                    }
                }
            }
        }

        void check_arguments(const std::vector<std::string>& argv) {
            namespace except = core::exception;
            if (argv.size() > 1) {
                throw except::invalid_second_arg("build");
            }
        }
    }

    struct build {
        static const std::string summary() {
            return "Compile all sources that depend on this project.";
        }
        static const std::string options() {
            return "[-v | --verbose]";
        }
        template<typename VS, typename = std::enable_if_t<std::is_rvalue_reference_v<VS&&>>>
        void operator()(VS&& argv) {
            _build::check_arguments(argv);
            _build::_main(std::move(argv));
        }
    };
} // end namespace
#endif // !POAC_SUBCMD_BUILD_HPP

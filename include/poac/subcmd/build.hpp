#ifndef POAC_SUBCMD_BUILD_HPP
#define POAC_SUBCMD_BUILD_HPP

#include <iostream>
#include <string>
#include <map>
#include <optional>

#include <boost/filesystem.hpp>

#include "../core/exception.hpp"
#include "../io/file.hpp"
#include "../io/cli.hpp"
#include "../util/stroite.hpp"
#include "../core/naming.hpp"
#include "../util/argparse.hpp"


// TODO: --release, --no-cache, --example,
// TODO: --check-std(標準では標準ライブラリをチェックしないため，標準ライブラリを書き換えてもリビルドしない)
namespace poac::subcmd {
    namespace _build {
        std::optional<std::string>
        build_bin(stroite::builder& bs, const bool verbose)
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
                        boost::system::error_code error;
                        fs::remove_all(io::file::path::current_build_cache_dir, error);
                        return std::nullopt;
                    }
                }
                else { // Compile failure
                    return std::nullopt;
                }
            }
        }

        std::optional<std::string>
        build_link_libs(stroite::builder& bs, const bool verbose)
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
                }
                else { // Static link library generation failed
                    // TODO: 全部削除すると，testのcacheも消えてしまう．
                    boost::system::error_code error;
                    fs::remove_all(io::file::path::current_build_cache_dir, error);
                }

                bs.configure_dynamic_lib(*obj_files_path, verbose);
                if (const auto dylib_path = bs._gen_dynamic_lib()) {
                    std::cout << io::cli::green << "Generated: " << io::cli::reset
                              << "Output to `" +
                                 fs::relative(*dylib_path).string() +
                                 "`"
                              << std::endl;
                }
                else { // Dynamic link library generation failed
                    // TODO: 全部削除すると，testのcacheも消えてしまう．
                    boost::system::error_code error;
                    fs::remove_all(io::file::path::current_build_cache_dir, error);
                }
                return std::nullopt;
            }
            else { // Compile failure
                return std::nullopt;
            }
        }


        void build_deps(const YAML::Node& node, const bool verbose) {
            namespace fs = boost::filesystem;
            namespace exception = core::exception;
            namespace naming = core::naming;
            namespace yaml = io::file::yaml;

            // TODO: lockファイル実装後，ビルド順序を決定 // TODO: 本来は，poac.ymlでなくpoac.lockをループすれば良い
            if (const auto deps_node = io::file::yaml::get<std::map<std::string, YAML::Node>>(node, "deps")) {
                for (const auto& [name, next_node] : *deps_node) {
                    const auto[src, name2] = naming::get_source(name);
                    const std::string version = naming::get_version(next_node, src);
                    const std::string current_package_name = naming::to_current(src, name2, version);
                    const auto deps_path = fs::current_path() / "deps" / current_package_name;

                    if (fs::exists(deps_path)) {
                        // install時にpoac.ymlは必ず作成されるため，存在する前提で扱う
                        const auto deps_config_node = yaml::load_config_by_dir(deps_path);

                        // depsのビルド時はbinaryは不要．必要になる可能性があるのはlibraryのみ
                        if (io::file::yaml::get(deps_config_node, "build", "lib")) {
                            stroite::builder bs(deps_path);

                            bs.configure_compile(false, verbose);
                            if (!bs.compile_conf.source_files.empty()) {
                                std::cout << io::cli::to_status(name) << std::endl;

                                if (const auto obj_files_path = bs._compile()) {
                                    bs.configure_static_lib(*obj_files_path, verbose);
                                    if (const auto stlib_path = bs._gen_static_lib()) {
                                        std::cout << io::cli::green << "Generated: " << io::cli::reset
                                                  << "Output to `" +
                                                     fs::relative(*stlib_path).string() +
                                                     "`"
                                                  << std::endl;
                                    }
                                    else { // Static link library generation failed
//                                        fs::remove_all(io::file::path::current_build_cache_dir);
                                    }

                                    bs.configure_dynamic_lib(*obj_files_path, verbose);
                                    if (const auto dylib_path = bs._gen_dynamic_lib()) {
                                        std::cout << io::cli::green << "Generated: " << io::cli::reset
                                                  << "Output to `" +
                                                     fs::relative(*dylib_path).string() +
                                                     "`"
                                                  << std::endl;
                                    }
                                    else { // Dynamic link library generation failed
//                                        fs::remove_all(io::file::path::current_build_cache_dir);
                                    }
                                }
                                else { // Compile failure
                                    throw exception::error("\nCompile error.");
                                }
                                std::cout << std::endl;
                            }
                        }
                    }
                    else {
                        throw exception::error(
                                name + " is not installed.\n"
                                "Please build after running `poac install`");
                    }
                }
            }
        }


        template<typename VS, typename = std::enable_if_t<std::is_rvalue_reference_v<VS&&>>>
        void _main(VS&& argv) {
            namespace fs = boost::filesystem;
            namespace exception = core::exception;
            namespace naming = core::naming;
            namespace yaml = io::file::yaml;

            const auto node = yaml::load_config();
            const bool verbose = util::argparse::use(argv, "-v", "--verbose");
            const auto project_name = yaml::get_with_throw<std::string>(node, "name");

            build_deps(node, verbose);
            stroite::builder bs;
            std::cout << io::cli::to_status(project_name) << std::endl;
            if (yaml::get(node, "build", "lib")) {
                if (!build_link_libs(bs, verbose)) {
                    // compile or gen error
                }
            }
            if (yaml::get(node, "build", "bin")) { // TODO: もし上でlibをビルドしたのなら，それを利用してバイナリをビルドする！
                // TODO: ディレクトリで指定できるようにする？？？
                if (!build_bin(bs, verbose)) {
                    // compile or link error

                    // 一度コンパイルに成功した後にpoac runを実行し，コンパイルに失敗しても実行されるエラーの回避
                    const auto binary_name = io::file::path::current_build_bin_dir / project_name;
                    const fs::path executable_path = fs::relative(binary_name);
                    boost::system::error_code error;
                    fs::remove(executable_path, error);
                }
            }
        }

        void check_arguments(const std::vector<std::string>& argv) {
            namespace exception = core::exception;
            if (argv.size() > 1) {
                throw exception::invalid_second_arg("build");
            }
        }
    }

    struct build {
        static const std::string summary() {
            return "Compile all sources that depend on this project";
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

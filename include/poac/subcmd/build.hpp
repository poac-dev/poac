#ifndef POAC_SUBCMD_BUILD_HPP
#define POAC_SUBCMD_BUILD_HPP

#include <iostream>
#include <string>
#include <map>
#include <optional>
#include <cstdlib>

#include <boost/filesystem.hpp>
#include <yaml-cpp/yaml.h>

#include "../core/exception.hpp"
#include "../core/lock.hpp"
#include "../io/file.hpp"
#include "../io/cli.hpp"
#include "../util/stroite.hpp"
#include "../core/naming.hpp"
#include "../util/argparse.hpp"


// TODO: --release, --no-cache, --example, --backend cmake
// TODO: --check-std(標準では標準ライブラリをチェックしないため，標準ライブラリを書き換えてもリビルドしない)
namespace poac::subcmd {
    namespace _build {
        std::optional<std::string>
        handle_message(const std::string& method, const std::optional<std::string>& output) {
            namespace fs = boost::filesystem;
            if (output) {
                std::cout << io::cli::to_green(method + ": ")
                          << "Output to `"
                          << fs::relative(*output).string()
                          << "`"
                          << std::endl;
                return output;
            }
            else { // Static link library generation failed // Dynamic link library generation failed
                // TODO: 全部削除すると，testのcacheも消えてしまう．// .dylibだけ消せば？？？ __APPLE__で分岐必要
                boost::system::error_code error;
                fs::remove_all(io::file::path::current_build_cache_dir, error);
                return std::nullopt;
            }
        }

        auto handle_compile_message(const std::optional<std::string>& output) {
            return handle_message("Compiled", output);
        }
        auto handle_generate_message(const std::optional<std::string>& output) {
            return handle_message("Generated", output);
        }

        auto handle_link(
                stroite::builder& bs,
                const std::vector<std::string>& obj_files_path,
                const bool verbose)
        {
            bs.configure_link(obj_files_path, verbose);
            return handle_compile_message(bs._link());
        }
        std::optional<std::string>
        handle_compile(
                stroite::builder& bs,
                const bool verbose)
        {
            if (const auto obj_files_path = bs._compile()) {
                return handle_link(bs, *obj_files_path, verbose);
            }
            else { // Compile failure
                return std::nullopt;
            }
        }
        auto handle_generate_static_lib(
                stroite::builder& bs,
                const std::vector<std::string>& obj_files_path,
                const bool verbose)
        {
            bs.configure_static_lib(obj_files_path, verbose);
            return handle_generate_message(bs._gen_static_lib());
        }
        auto handle_generate_dynamic_lib(
                stroite::builder& bs,
                const std::vector<std::string>& obj_files_path,
                const bool verbose)
        {
            bs.configure_dynamic_lib(obj_files_path, verbose);
            return handle_generate_message(bs._gen_dynamic_lib());
        }

        void handle_generate_lib(
                stroite::builder& bs,
                const std::vector<std::string>& obj_files_path,
                const bool verbose)
        {
            handle_generate_static_lib(bs, obj_files_path, verbose);
            handle_generate_dynamic_lib(bs, obj_files_path, verbose);
        }

        void handle_exist_message(
                const std::string& file_path,
                const std::string& extension,
                const std::string& type)
        {
            namespace fs = boost::filesystem;
            // Dealing with an error which is said to have cache even though it is not going well.
            if (fs::exists(file_path + extension)) {
                std::cout << io::cli::to_yellow("Warning: ")
                          << "There is no change. " << type << " exists in `"
                          << fs::relative(file_path).string() << extension << "`."
                          << std::endl;
            }
        }
        void is_exist_static_lib(const std::string& lib_path) {
            handle_exist_message(lib_path, ".a", "Static link library");
        }
        void is_exist_dynamic_lib(const std::string& lib_path) {
#ifdef __APPLE__
            const std::string extension = ".dylib";
#elif defined(_WIN32)
            const std::string extension = ".dll";
#else
            const std::string extension = ".so";
#endif
            handle_exist_message(lib_path, extension, "Dynamic link library");
        }
        std::string is_exist_lib(const std::string& project_name) {
            namespace path = io::file::path;
            const auto lib_path = (path::current_build_lib_dir / project_name).string();
            is_exist_static_lib(lib_path);
            is_exist_dynamic_lib(lib_path);
            return lib_path;
        }


        std::optional<std::string>
        build_bin(stroite::builder& bs, const bool verbose)
        {
            bs.configure_compile(true, verbose);
            // Since the obj file already exists and has not been changed as a result
            //  of verification of the hash file, return only the list of existing obj_files
            //  and do not compile.
            // There is no necessity of linking that there is no change completely.
            if (bs.compile_conf.source_files.empty()) { // No need for compile and link
#ifdef _WIN32
                const std::string extension = ".exe";
#else
                const std::string extension = "";
#endif
                const std::string bin_path =
                        (io::file::path::current_build_bin_dir / bs.project_name).string();
                handle_exist_message(bin_path, extension, "Binary");
                return bin_path;
            }
            else {
                return handle_compile(bs, verbose);
            }
        }

        std::optional<std::string>
        build_link_libs(stroite::builder& bs, const bool verbose)
        {
            bs.configure_compile(false, verbose);
            if (bs.compile_conf.source_files.empty()) { // No need for compile and link
                return is_exist_lib(bs.project_name);
            }
            if (const auto obj_files_path = bs._compile()) {
                handle_generate_lib(bs, *obj_files_path, verbose);
                return std::nullopt;
            }
            else { // Compile failure
                return std::nullopt;
            }
        }


        bool compile_deps(
                const YAML::Node& node,
                const std::string& name,
                const boost::filesystem::path& deps_path,
                const bool verbose)
        {
            namespace exception = core::exception;

            // depsのビルド時はbinaryは不要．必要になる可能性があるのはlibraryのみ
            if (io::file::yaml::get(node, "build", "lib")) {
                stroite::builder bs(deps_path);

                bs.configure_compile(false, verbose);
                if (!bs.compile_conf.source_files.empty()) {
                    std::cout << io::cli::to_status(name) << std::endl;

                    if (const auto obj_files_path = bs._compile()) {
                        handle_generate_lib(bs, *obj_files_path, verbose);
                    }
                    else { // Compile failure
                        throw exception::error("\nCompile error.");
                    }
                    std::cout << std::endl;
                }

                return true;
            }
            return false;
        }

        void build_deps(const YAML::Node& node, const bool verbose) {
            namespace fs = boost::filesystem;
            namespace exception = core::exception;
            namespace lock = core::lock;
            namespace naming = core::naming;
            namespace yaml = io::file::yaml;


            if (!yaml::get(node, "deps")) {
                return; // depsが存在しない
            }
            // TODO: ビルド順序
            if (const auto locked_deps = lock::load_ignore_timestamp()) {
                for (const auto& [name, dep] : (*locked_deps).backtracked) {
                    const std::string current_package_name = naming::to_current(dep.source, name, dep.version);
                    const auto deps_path = fs::current_path() / "deps" / current_package_name;

                    if (fs::exists(deps_path)) {
                        // IF dep.source == "github"
                        // ./deps/pack/poac.yml は存在しないと見做す (TODO: poac projectなのにgithubをsourceとしている場合がある)
                        // 現状は，./poac.ymlから，buildキーを読み込む -> 無いなら header-onlyと見做す．

                        // IF dep.source == "poac"
                        // プロジェクトルートの方に，buildキーがあるならそちらを
                        //  -> 無いなら，提供者->deps_pathの方を選ぶ
                        //  -> 無いなら，header-onlyと見做す．
                        if (!compile_deps(node, name, deps_path, verbose) && dep.source == "poac") {
                            const auto deps_config_node = yaml::load_config_by_dir(deps_path);
                            compile_deps(deps_config_node, name, deps_path, verbose);
                        }
                    }
                    else {
                        throw exception::error(
                                name + " is not installed.\n"
                                       "Please build after running `poac install`");
                    }
                }
            }
            else {
                throw exception::error(
                        "Could not load poac.lock.\n"
                        "Please build after running `poac install`");
            }
        }

        template<typename VS, typename = std::enable_if_t<std::is_rvalue_reference_v<VS&&>>>
        int _main(VS&& argv) {
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
            if (yaml::get(node, "build", "bin")) { // TODO: もし上でlibをビルドしたのなら，それを利用してバイナリをビルドする
                // TODO: ディレクトリで指定できるように
                if (!build_bin(bs, verbose)) {
                    // compile or link error

                    // 一度コンパイルに成功した後にpoac runを実行し，コンパイルに失敗しても実行されるエラーの回避
                    const auto binary_name = io::file::path::current_build_bin_dir / project_name;
                    const fs::path executable_path = fs::relative(binary_name);
                    boost::system::error_code error;
                    fs::remove(executable_path, error);
                }
            }

            return EXIT_SUCCESS;
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
        int operator()(VS&& argv) {
            _build::check_arguments(argv);
            return _build::_main(std::move(argv));
        }
    };
} // end namespace
#endif // !POAC_SUBCMD_BUILD_HPP

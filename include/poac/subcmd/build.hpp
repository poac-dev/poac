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
#include "../core/stroite/utils/absorper.hpp"
#include "../core/deper/lock.hpp"
#include "../io/file.hpp"
#include "../io/cli.hpp"
#include "../core/stroite.hpp"
#include "../core/naming.hpp"
#include "../util/argparse.hpp"


// TODO: --release, --no-cache (build systemにpoacを使用する時のみ), --example, --backend poac or cmake
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
                core::stroite::builder& bs,
                const std::vector<std::string>& obj_files_path,
                const std::vector<std::string>& library_path, // depsのlibや，自分自身のlib
                const bool verbose)
        {
            bs.configure_link(obj_files_path, verbose);
            for (const auto& l : library_path) {
                bs.link_conf.library_path.push_back(l);
            }
            return handle_compile_message(bs._link());
        }
        std::optional<std::string>
        handle_compile(
                core::stroite::builder& bs,
                const std::vector<std::string>& library_path,
                const bool verbose)
        {
            if (const auto obj_files_path = bs._compile()) {
                return handle_link(bs, *obj_files_path, library_path, verbose);
            }
            else { // Compile failure
                return std::nullopt;
            }
        }
        auto handle_generate_static_lib(
                core::stroite::builder& bs,
                const std::vector<std::string>& obj_files_path,
                const bool verbose)
        {
            bs.configure_static_lib(obj_files_path, verbose);
            return handle_generate_message(bs._gen_static_lib());
        }
        auto handle_generate_dynamic_lib(
                core::stroite::builder& bs,
                const std::vector<std::string>& obj_files_path,
                const bool verbose)
        {
            bs.configure_dynamic_lib(obj_files_path, verbose);
            return handle_generate_message(bs._gen_dynamic_lib());
        }

        void handle_generate_lib(
                core::stroite::builder& bs,
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
            const std::string extension = core::stroite::utils::absorper::dynamic_lib_extension;
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
        build_bin(
                core::stroite::builder& bs,
                const std::vector<std::string>& library_path,
                const bool verbose)
        {
            bs.configure_compile(true, verbose);
            // Since the obj file already exists and has not been changed as a result
            //  of verification of the hash file, return only the list of existing obj_files
            //  and do not compile.
            // There is no necessity of linking that there is no change completely.
            if (bs.compile_conf.source_files.empty()) { // No need for compile and link
                const std::string extension = core::stroite::utils::absorper::binary_extension;
                const std::string bin_path =
                        (io::file::path::current_build_bin_dir / bs.project_name).string();
                handle_exist_message(bin_path, extension, "Binary");
                return bin_path;
            }
            else {
                return handle_compile(bs, library_path, verbose);
            }
        }

        std::optional<std::string>
        build_link_libs(core::stroite::builder& bs, const bool verbose)
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
            namespace stroite = core::stroite;

            if (const auto system = stroite::core::builder::detect_build_system(node)) {
                if (*system == "poac") {
                    // depsのビルド時はbinaryは不要．必要になる可能性があるのはlibraryのみ
                    if (io::file::yaml::get(node, "build", "lib")) {
                        stroite::builder bs(deps_path);

                        bs.configure_compile(false, verbose);
                        if (!bs.compile_conf.source_files.empty()) {
                            io::cli::echo(io::cli::to_status(name));

                            if (const auto obj_files_path = bs._compile()) {
                                handle_generate_lib(bs, *obj_files_path, verbose);
                            }
                            else { // Compile failure
                                throw exception::error("\nCompile error.");
                            }
                            io::cli::echo();
                        }
                        return true;
                    }
                }
                else if (*system == "cmake") {
                    stroite::cmake bs(deps_path);
                    io::cli::echo(io::cli::to_status(name));
                    bs.build();
                    io::cli::echo();
                }
            }
            return false;
        }

        std::optional<std::vector<std::string>>
        build_deps(const YAML::Node& node, const bool verbose) {
            namespace fs = boost::filesystem;
            namespace exception = core::exception;
            namespace stroite = core::stroite;
            namespace lock = core::deper::lock;
            namespace naming = core::naming;
            namespace yaml = io::file::yaml;


            std::vector<std::string> library_path;
            if (const auto deps_node = yaml::get<std::map<std::string, YAML::Node>>(node, "deps")) {
                // TODO: ビルド順序
                if (const auto locked_deps = lock::load_ignore_timestamp()) {
                    for (const auto& [name, dep] : (*locked_deps).backtracked) {
                        const std::string current_package_name = naming::to_current(dep.source, name, dep.version);
                        const auto deps_path = fs::current_path() / "deps" / current_package_name;

                        if (fs::exists(deps_path)) {
                            bool exist_build_key = true;
                            try { // 依存の依存は，poac.ymlに書かれていないため，buildキーは存在しない
                                (*deps_node).at(name);
                            }
                            catch(std::out_of_range&) {
                                exist_build_key = false;
                            }

                            if (exist_build_key) {
                                if (const auto system = stroite::core::builder::detect_build_system((*deps_node).at(name))) {
                                    const bool result = compile_deps((*deps_node).at(name), name, deps_path, verbose);
                                    if (result) {
                                        library_path.push_back((io::file::path::current_build_lib_dir / current_package_name).string() + ".a"); // TODO: 可変にしたい
                                    }
                                }
                                else if (const auto deps_config_node = yaml::load_config_by_dir(deps_path)) {
                                    const bool result = compile_deps(*deps_config_node, name, deps_path, verbose);
                                    if (result) {
                                        library_path.push_back((io::file::path::current_build_lib_dir / current_package_name).string() + ".a"); // TODO: 可変にしたい
                                    }
                                }
                                // header-only
                            }
                            else {
                                if (const auto deps_config_node = yaml::load_config_by_dir(deps_path)) {
                                    const bool result = compile_deps(*deps_config_node, name, deps_path, verbose);
                                    if (result) {
                                        library_path.push_back((io::file::path::current_build_lib_dir / current_package_name).string() + ".a"); // TODO: 可変にしたい
                                    }
                                }
                                // header-only
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
            if (library_path.empty()) {
                return std::nullopt;
            }
            else {
                return library_path;
            }
        }

        template<typename VS, typename=std::enable_if_t<std::is_rvalue_reference_v<VS&&>>>
        int _main(VS&& argv) {
            namespace fs = boost::filesystem;
            namespace exception = core::exception;
            namespace stroite = core::stroite;
            namespace naming = core::naming;
            namespace yaml = io::file::yaml;

            const auto node = yaml::load_config();
            const bool verbose = util::argparse::use(argv, "-v", "--verbose");
            const auto project_name = yaml::get_with_throw<std::string>(node, "name");

            if (const auto system = stroite::core::builder::detect_build_system(node)) {
                const auto built_deps = build_deps(node, verbose); // TODO: ここでビルドしたlibを，リンクしたい
                const bool is_built_deps = static_cast<bool>(built_deps);

                if (*system == "poac") {
                    stroite::builder bs;

                    if (is_built_deps) {
                        io::cli::echo(io::cli::to_status(project_name));
                    }

//                    bool built_lib = false;
                    if (yaml::get(node, "build", "lib")) {
//                        built_lib = true;
                        if (!build_link_libs(bs, verbose)) {
                            // compile or gen error
                        }
                    }
                    if (yaml::get(node, "build", "bin")) { // TODO: もし上でlibをビルドしたのなら，それを利用してバイナリをビルドする -> まだ -> cpp_shell_cmdでテスト
                        // TODO: ディレクトリで指定できるように
                        if (!build_bin(bs, *built_deps, verbose)) {
                            // compile or link error

                            // 一度コンパイルに成功した後にpoac runを実行し，コンパイルに失敗しても実行されるエラーの回避
                            const auto binary_name = io::file::path::current_build_bin_dir / project_name;
                            const fs::path executable_path = fs::relative(binary_name);
                            boost::system::error_code error;
                            fs::remove(executable_path, error);
                        }
                    }
                }
                else if (*system == "cmake") {
                    stroite::cmake bs;
                    if (is_built_deps) {
                        io::cli::echo(io::cli::to_status(project_name));
                    }
                    bs.build(); // only build
                    io::cli::echo(io::cli::to_green("Compiled: "), project_name);
                }
            }
            else { // error
                throw exception::error(
                        "Required key `build` does not exist in poac.yml.\n"
                        "Please refer to https://docs.poac.io");
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
        template<typename VS, typename=std::enable_if_t<std::is_rvalue_reference_v<VS&&>>>
        int operator()(VS&& argv) {
            _build::check_arguments(argv);
            return _build::_main(std::move(argv));
        }
    };
} // end namespace
#endif // !POAC_SUBCMD_BUILD_HPP

#ifndef POAC_OPTS_BUILD_HPP
#define POAC_OPTS_BUILD_HPP

#include <iostream>
#include <string>
#include <map>
#include <optional>
#include <cstdlib>

#include <boost/filesystem.hpp>
#include <yaml-cpp/yaml.h>

#include "../core/except.hpp"
#include "../core/stroite/utils/absorb.hpp"
#include "../core/stroite/utils/detect.hpp"
#include "../core/resolver/lock.hpp"
#include "../io.hpp"
#include "../core/stroite.hpp"
#include "../core/name.hpp"
#include "../util/argparse.hpp"
#include "../util/termcolor2.hpp"


namespace poac::opts {
    namespace _build {



        // TODO: この辺りの，ハンドラを，いい感じに，builder.hppに持っていく．


        std::optional<std::string>
        handle_message(const std::string& method, const std::optional<std::string>& output) {
            namespace fs = boost::filesystem;
            if (output) {
                std::cout << termcolor2::green<> << method << ": " << termcolor2::reset<>
                          << "Output to `"
                          << fs::relative(*output).string()
                          << "`"
                          << std::endl;
                return output;
            }
            else { // Static link library generation failed // Dynamic link library generation failed
                // TODO: 全部削除すると，testのcacheも消えてしまう．// .dylibだけ消せば？？？ __APPLE__で分岐必要
                boost::system::error_code error;
                fs::remove_all(io::path::current_build_cache_dir, error);
                return std::nullopt;
            }
        }

        std::optional<std::string>
        handle_compile_message(const std::optional<std::string>& output) {
            return handle_message("Compiled", output);
        }
        std::optional<std::string>
        handle_generate_message(const std::optional<std::string>& output) {
            return handle_message("Generated", output);
        }

        std::optional<std::string>
        handle_link(
                core::stroite::core::builder& bs,
                const std::vector<std::string>& obj_files_path,
                const std::vector<std::string>& library_path) // depsのlibや，自分自身のlib
        {
            bs.configure_link(obj_files_path);
            for (const auto& l : library_path) {
                bs.link_conf.library_path.push_back(l);
            }
            return handle_compile_message(bs.link());
        }
        std::optional<std::string>
        handle_compile(
                core::stroite::core::builder& bs,
                const std::vector<std::string>& library_path)
        {
            if (const auto obj_files_path = bs.compile()) {
                return handle_link(bs, *obj_files_path, library_path);
            }
            else { // Compile failure
                return std::nullopt;
            }
        }
        std::optional<std::string>
        handle_generate_static_lib(
                core::stroite::core::builder& bs,
                const std::vector<std::string>& obj_files_path)
        {
            bs.configure_static_lib(obj_files_path);
            return handle_generate_message(bs.gen_static_lib());
        }
        std::optional<std::string>
        handle_generate_dynamic_lib(
                core::stroite::core::builder& bs,
                const std::vector<std::string>& obj_files_path)
        {
            bs.configure_dynamic_lib(obj_files_path);
            return handle_generate_message(bs.gen_dynamic_lib());
        }

        void handle_generate_lib(
                core::stroite::core::builder& bs,
                const std::vector<std::string>& obj_files_path)
        {
            handle_generate_static_lib(bs, obj_files_path);
            handle_generate_dynamic_lib(bs, obj_files_path);
        }

        void handle_exist_message(
                const std::string& file_path,
                const std::string& extension,
                const std::string& type)
        {
            namespace fs = boost::filesystem;
            using termcolor2::color_literals::operator""_yellow;

            // Dealing with an error which is said to have cache even though it is not going well.
            if (fs::exists(file_path + extension)) {
                std::cout << "Warning: "_yellow
                          << "There is no change. " << type << " exists in `"
                          << fs::relative(file_path).string() << extension << "`."
                          << std::endl;
            }
        }
        void is_exist_static_lib(const std::string& lib_path) {
            handle_exist_message(lib_path, ".a", "Static link library");
        }
        void is_exist_dynamic_lib(const std::string& lib_path) {
            const std::string extension = core::stroite::utils::absorb::dynamic_lib_extension;
            handle_exist_message(lib_path, extension, "Dynamic link library");
        }
        std::string is_exist_lib(const std::string& project_name) {
            const auto lib_path = (io::path::current_build_lib_dir / project_name).string();
            is_exist_static_lib(lib_path);
            is_exist_dynamic_lib(lib_path);
            return lib_path;
        }


        std::optional<std::string> // TODO: このあたり，builder.hppへ移動できる -> bs.build()のみで，binのビルドとかlibとかを意識せずに使いたい
        build_bin(
                core::stroite::core::builder& bs,
                const std::vector<std::string>& library_path)
        {
            bs.configure_compile(true);
            // Since the obj file already exists and has not been changed as a result
            //  of verification of the hash file, return only the list of existing obj_files
            //  and do not compile.
            // There is no necessity of linking that there is no change completely.
            if (bs.compile_conf.source_files.empty()) { // No need for compile and link
                const std::string extension = core::stroite::utils::absorb::binary_extension;
                const std::string bin_path =
                        (io::path::current_build_bin_dir / bs.project_name).string();
                handle_exist_message(bin_path, extension, "Binary");
                return bin_path;
            }
            else {
                return handle_compile(bs, library_path);
            }
        }

        std::optional<std::string>
        build_link_libs(
                core::stroite::core::builder& bs,
                std::vector<std::string>& deps_obj_files_path)
        {
            bs.configure_compile(false);
            if (bs.compile_conf.source_files.empty()) { // No need for compile and link
                return is_exist_lib(bs.project_name);
            }
            if (auto obj_files_path = bs.compile()) {
                for (const auto o : *obj_files_path) {
                    deps_obj_files_path.push_back(o);
                }
                handle_generate_lib(bs, deps_obj_files_path);
                return std::nullopt;
            }
            else { // Compile failure
                return std::nullopt;
            }
        }


        std::optional<std::vector<std::string>>
        compile_deps(
                const YAML::Node& node,
                const std::string& name,
                const boost::filesystem::path& deps_path,
                const bool verbose)
        {
            namespace except = core::except;
            namespace stroite = core::stroite;

            if (const auto system = stroite::utils::detect::build_system(node)) {
                if (*system == "poac") {
                    // depsのビルド時はbinaryは不要．必要になる可能性があるのはlibraryのみ
                    if (io::yaml::get(node, "build", "lib")) {
                        stroite::core::builder bs(verbose, deps_path);

                        bs.configure_compile(false);
                        if (!bs.compile_conf.source_files.empty()) {
                            std::cout << io::cli::status << name << std::endl;

                            if (const auto obj_files_path = bs.compile()) {
                                handle_generate_lib(bs, *obj_files_path);
                                std::cout << std::endl;
                                return *obj_files_path;
                            }
                            else { // Compile failure
                                throw except::error("\nCompile error.");
                            }
                        }
                    }
                }
                else if (*system == "cmake") {
                    stroite::chain::cmake bs(deps_path);
                    std::cout << io::cli::status << name << std::endl;
                    bs.build();
                    std::cout << std::endl;
                    return {};
                }
            }
            return std::nullopt; // 結局compileしていない
        }

        // TODO: depends.hppで処理させる？ -> であれば，ビルド自体はすべきでない？
        std::optional<std::vector<std::string>>
        build_deps(const YAML::Node& node, std::vector<std::string>& obj_files_path, const bool verbose) {
            namespace fs = boost::filesystem;
            namespace except = core::except;
            namespace stroite = core::stroite;
            namespace lock = core::deper::lock;
            namespace name = core::name;
            namespace yaml = io::yaml;


            std::vector<std::string> library_path;
            if (const auto deps_node = yaml::get<std::map<std::string, YAML::Node>>(node, "deps")) {
                // TODO: ビルド順序
                if (const auto locked_deps = lock::load_ignore_timestamp()) {
                    for (const auto& [name, dep] : (*locked_deps).backtracked) {
                        const std::string current_package_name = name::to_current(dep.source, name, dep.version);
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
                                if (const auto system = stroite::utils::detect::build_system((*deps_node).at(name))) {
                                    if (const auto obj_files_path_opt = compile_deps((*deps_node).at(name), name, deps_path, verbose)) {
                                        for (const auto& o : *obj_files_path_opt) {
                                            obj_files_path.push_back(o);
                                        }
                                        library_path.push_back((io::path::current_build_lib_dir / current_package_name).string() + ".a"); // TODO: 可変にしたい / dylibかa
                                    }
                                }
                                else if (const auto deps_config_node = yaml::load_config_by_dir(deps_path)) {
                                    if (const auto obj_files_path_opt = compile_deps(*deps_config_node, name, deps_path, verbose)) {
                                        for (const auto& o : *obj_files_path_opt) {
                                            obj_files_path.push_back(o);
                                        }
                                        library_path.push_back((io::path::current_build_lib_dir / current_package_name).string() + ".a"); // TODO: 可変にしたい
                                    }
                                }
                                // header-only
                            }
                            else {
                                if (const auto deps_config_node = yaml::load_config_by_dir(deps_path)) {
                                    if (const auto obj_files_path_opt = compile_deps(*deps_config_node, name, deps_path, verbose)) {
                                        for (const auto& o : *obj_files_path_opt) {
                                            obj_files_path.push_back(o);
                                        }
                                        library_path.push_back((io::path::current_build_lib_dir / current_package_name).string() + ".a"); // TODO: 可変にしたい
                                    }
                                }
                                // header-only
                            }
                        }
                        else {
                            throw except::error(
                                    name, " is not installed.\n"
                                    "Please build after running `poac install`");
                        }
                    }
                }
                else {
                    throw except::error(
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

        template<typename VS>
        int _main(VS&& argv) {
            namespace fs = boost::filesystem;
            namespace except = core::except;
            namespace stroite = core::stroite;
            namespace name = core::name;
            namespace yaml = io::yaml;
            using termcolor2::color_literals::operator""_green;


            // {
            //   stroite::core::Builder bs(fs::current_path());
            //   bs.build(verbose);
            // }

            // for (l : lock_file) {
            //   stroite::core::Builder bs(fs::current_path());
            //   bs.build(verbose); -> Unneed build detected OR Compiled
            // }

            const auto node = yaml::load_config();
            const bool verbose = util::argparse::use(argv, "-v", "--verbose");
            const auto project_name = yaml::get_with_throw<std::string>(node, "name");

            if (const auto system = stroite::utils::detect::build_system(node)) {
                std::vector<std::string> deps_obj_files_path;
                const auto built_deps = build_deps(node, deps_obj_files_path, verbose);
                const bool is_built_deps = static_cast<bool>(built_deps);

                if (*system == "poac") {
                    stroite::core::builder bs(verbose);

                    if (is_built_deps) {
                        std::cout << io::cli::status << project_name << std::endl;
                    }

//                    bool built_lib = false;
                    if (yaml::get(node, "build", "lib")) {
//                        built_lib = true;
                        if (!build_link_libs(bs, deps_obj_files_path)) {
                            // compile or gen error
                            return EXIT_FAILURE;
                        }
                    }
                    if (yaml::get(node, "build", "bin")) { // TODO: もし上でlibをビルドしたのなら，それを利用してバイナリをビルドする -> まだ -> cpp_shell_cmdでテスト
                        // TODO: ディレクトリで指定できるように
                        if (!build_bin(bs, *built_deps)) {
                            // 一度コンパイルに成功した後にpoac runを実行し，コンパイルに失敗しても実行されるエラーの回避
                            const auto binary_name = io::path::current_build_bin_dir / project_name;
                            const fs::path executable_path = fs::relative(binary_name);
                            boost::system::error_code error;
                            fs::remove(executable_path, error);

                            // compile or link error
                            return EXIT_FAILURE;
                        }
                    }
                }
                else if (*system == "cmake") {
                    stroite::chain::cmake bs;
                    if (is_built_deps) {
                        std::cout << io::cli::status << project_name << std::endl;
                    }
                    bs.build(); // only build
                    std::cout << "Compiled: "_green << project_name << std::endl;
                }
            }
            else { // error
                throw except::error(
                        "Required key `build` does not exist in poac.yml.\n"
                        "Please refer to https://doc.poac.pm");
            }
            return EXIT_SUCCESS;
        }

        void check_arguments(const std::vector<std::string>& argv) {
            namespace except = core::except;
            if (argv.size() > 1) {
                throw except::invalid_second_arg("build");
            }
        }
    }

    struct build {
        static std::string summary() {
            return "Compile all sources that depend on this project";
        }
        static std::string options() {
            return "[-v | --verbose]";
        }
        template<typename VS>
        int operator()(VS&& argv) {
            _build::check_arguments(argv);
            return _build::_main(std::forward<VS>(argv));
        }
    };
} // end namespace
#endif // !POAC_OPTS_BUILD_HPP

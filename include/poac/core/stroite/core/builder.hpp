#ifndef POAC_CORE_STROITE_CORE_BUILDER_HPP
#define POAC_CORE_STROITE_CORE_BUILDER_HPP

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <iterator>
#include <functional>
#include <list>
#include <algorithm>
#include <map>
#include <optional>

#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>

#include "./cache.hpp"
#include "./compiler.hpp"
#include "./depends.hpp"
#include "./search.hpp"
#include "../utils.hpp"
#include "../../exception.hpp"
#include "../../naming.hpp"
#include "../../deper/lock.hpp"
#include "../../deper/semver.hpp"
#include "../../../io/file/path.hpp"
#include "../../../io/cli.hpp"
#include "../../../io/file/yaml.hpp"


namespace poac::core::stroite::core {
    struct builder {
        utils::options::compile compile_conf;
        utils::options::link link_conf;
        utils::options::static_lib static_lib_conf;
        utils::options::dynamic_lib dynamic_lib_conf;

        std::string compiler;
        std::string project_name;
        boost::filesystem::path base_dir;

        std::map<std::string, YAML::Node> node;
        std::map<std::string, std::map<std::string, std::string>> depends_ts;
        std::optional<std::map<std::string, YAML::Node>> deps_node;

        bool verbose;


        auto make_include_search_path() {
            namespace fs = boost::filesystem;
            namespace lock = deper::lock;
            namespace yaml = io::file::yaml;
            namespace path = io::file::path;

            std::vector<std::string> include_search_path;
            if (deps_node) { // TODO: subcmd/build.hppで，存在確認が取れている
                if (const auto locked_deps = lock::load_ignore_timestamp()) {
                    for (const auto& [name, dep] : locked_deps->backtracked) {
                        const std::string current_package_name = naming::to_current(dep.source, name, dep.version);
                        const fs::path include_dir = path::current_deps_dir / current_package_name / "include";

                        if (path::validate_dir(include_dir)) {
                            include_search_path.push_back(include_dir.string());
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
            return include_search_path;
        }

        auto make_macro_defns() {
            namespace fs = boost::filesystem;
            namespace yaml = io::file::yaml;
            using utils::config::make_macro_defn;

            std::vector<std::string> macro_defns;
            // poac automatically define the absolute path of the project's root directory.
            macro_defns.emplace_back(make_macro_defn("POAC_PROJECT_ROOT", fs::current_path().string()));
            const auto version = deper::semver::Version(yaml::get_with_throw<std::string>(node.at("version")));
            macro_defns.emplace_back(make_macro_defn("POAC_VERSION", version.get_full()));
            macro_defns.emplace_back(make_macro_defn("POAC_MAJOR_VERSION", version.major));
            macro_defns.emplace_back(make_macro_defn("POAC_MINOR_VERSION", version.minor));
            macro_defns.emplace_back(make_macro_defn("POAC_PATCH_VERSION", version.patch));
            return macro_defns;
        }

        std::vector<std::string>
        make_compile_other_args() {
            namespace yaml = io::file::yaml;
            if (const auto compile_args = yaml::get<std::vector<std::string>>(node.at("build"), "compile_args")) {
                return *compile_args;
            }
            else {
                return {};
            }
        }


        std::vector<std::string>
        hash_source_files(
            std::vector<std::string>&& source_files,
            const bool usemain )
        {
            namespace fs = boost::filesystem;

            if (usemain) {
                if (!fs::exists("main.cpp")) {
                    throw exception::error("main.cpp does not exists");
                }
                else {
                    source_files.push_back("main.cpp");
                }
            }
            return core::cache::check_src_cpp(compile_conf, depends_ts, source_files, verbose);
        }

        void configure_compile(const bool usemain)
        {
            namespace yaml = io::file::yaml;
            namespace path = io::file::path;

            compile_conf.system = compiler;
            compile_conf.version_prefix = utils::config::default_version_prefix();
            compile_conf.cpp_version = yaml::get_with_throw<unsigned int>(node.at("cpp_version"));
            compile_conf.include_search_path = make_include_search_path();
            compile_conf.other_args = make_compile_other_args();
            compile_conf.source_files = hash_source_files(search::cpp(base_dir), usemain);
            compile_conf.macro_defns = make_macro_defns();
            compile_conf.base_dir = base_dir;
            compile_conf.output_root = path::current_build_cache_obj_dir;
        }
        std::optional<std::vector<std::string>>
        _compile() {
            namespace fs = boost::filesystem;
            namespace io = io::file;

            for (const auto& s : compile_conf.source_files) {
                // sourceファイルを一つづつコンパイルする．
                compile_conf.source_file = s;
                if (const auto ret = core::compiler::compile(compile_conf, verbose)) {
                    // Since compile succeeded, save hash
                    std::ofstream ofs;
                    for (const auto& [hash_name, data] : depends_ts) { // TODO: ここまで持ち回るから落ちる？
                        std::string output_string;
                        for (const auto& [file_name, hash] : data) {
                            output_string += file_name + ": " + hash + "\n";
                        }
                        fs::create_directories(fs::path(hash_name).parent_path());
                        io::path::write_to_file(ofs, hash_name, output_string);
                    }

                }
                else {
                    return std::nullopt;
                }
            }

            // Because it is excluded for the convenience of cache,
            //  ignore the return value of compiler.compile.
            std::vector<std::string> obj_files;
            for (const auto& s : compile_conf.source_files) {
                obj_files.push_back(
                        (compile_conf.output_root / fs::relative(s))
                                .replace_extension("o")
                                .string()
                );
            }
            return obj_files;
        }

        std::vector<std::string>
        make_link_other_args() {
            namespace yaml = io::file::yaml;
            if (const auto link_args = yaml::get<std::vector<std::string>>(node.at("build"), "link_args")) {
                return *link_args;
            }
            else {
                return {};
            }
        }
        auto make_link() {
            namespace fs = boost::filesystem;
            namespace yaml = io::file::yaml;

            std::vector<std::string> library_search_path;
            std::vector<std::string> static_link_libs;
            std::vector<std::string> library_path;

            if (deps_node) {
                for (const auto& [name, next_node] : *deps_node) {
                    const auto[src, name2] = naming::get_source(name);
                    const std::string version = naming::get_version(next_node, src);

                    // FIXME: srcではなく，build systemを読む．
                    if (src != "poac") {
                        const std::string pkgname = naming::to_cache(src, name2, version);
                        const fs::path pkgpath = io::file::path::current_deps_dir / pkgname;

                        // TODO: できればlockファイルに書かれたパッケージの./depsディレクトリのpoac.ymlを読むのが好ましい
                        if (const fs::path lib_dir = pkgpath / "lib"; fs::exists(lib_dir)) {
                            library_search_path.push_back(lib_dir.string());

                            if (const auto link = yaml::get<std::vector<std::string>>(next_node, "link", "include")) {
                                for (const auto& l : *link) {
                                    static_link_libs.push_back(l);
                                }
                            }
                            else {
                                static_link_libs.push_back(pkgname);
                            }
                        }
                    }
                }
            }
            return std::make_tuple(library_search_path, static_link_libs, library_path);
        }
        void configure_link(const std::vector<std::string>& obj_files_path)
        {
            link_conf.system = compiler;
            link_conf.project_name = project_name;
            link_conf.output_root = io::file::path::current_build_bin_dir;
            link_conf.obj_files_path = obj_files_path;
            const auto links = make_link();
            link_conf.library_search_path = std::get<0>(links);
            link_conf.static_link_libs = std::get<1>(links);
            link_conf.library_path = std::get<2>(links);
            link_conf.other_args = make_link_other_args();
        }
        auto _link()
        {
            return core::compiler::link(link_conf, verbose);
        }

        void configure_static_lib(const std::vector<std::string>& obj_files_path)
        {
            namespace io = io::file;
            static_lib_conf.project_name = project_name;
            static_lib_conf.output_root = io::path::current_build_lib_dir;
            static_lib_conf.obj_files_path = obj_files_path;
        }
        auto _gen_static_lib()
        {
            return core::compiler::gen_static_lib(static_lib_conf, verbose);
        }

        void configure_dynamic_lib(const std::vector<std::string>& obj_files_path)
        {
            namespace io = io::file;
            dynamic_lib_conf.system = compiler;
            dynamic_lib_conf.project_name = project_name;
            // outputを一箇所か分散か選べるように．boost::hoghoeみたいに，enumのオプションを渡すとOK
            // 一箇所ってのは，./ poac build -> ./_buildだけど，depsも./_buildに配置されるやつ
            dynamic_lib_conf.output_root = io::path::current_build_lib_dir;
            dynamic_lib_conf.obj_files_path = obj_files_path;
        }
        auto _gen_dynamic_lib()
        {
            return core::compiler::gen_dynamic_lib(dynamic_lib_conf, verbose);
        }

        // TODO: poac.ymlのhashもcheckしてほしい
        // TODO: 自らのinclude，dirも，(存在するなら！) includeパスに渡してほしい．そうすると，poacでinclude<poac/poac.hpp>できる
        explicit builder(const bool verbose, const boost::filesystem::path& base_path=boost::filesystem::current_path())
        {
            namespace yaml = io::file::yaml;

            const auto config_file = yaml::load_config_by_dir_with_throw(base_path);
            node = yaml::get_by_width(config_file, "name", "version", "cpp_version", "build");
            deps_node = yaml::get<std::map<std::string, YAML::Node>>(config_file, "deps");
            project_name = naming::slash_to_hyphen(node.at("name").as<std::string>());
            compiler = utils::config::auto_select_compiler();
            base_dir = base_path;
            this->verbose = verbose;
        }
    };
} // end namespace
#endif // POAC_CORE_STROITE_CORE_BUILDER_HPP

#ifndef POAC_CORE_STROITE_CORE_BUILDER_HPP
#define POAC_CORE_STROITE_CORE_BUILDER_HPP

#include <cstdlib>
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
#include "../field/standard.hpp"
#include "../utils.hpp"
#include "../../except.hpp"
#include "../../name.hpp"
#include "../../deper/lock.hpp"
#include "../../deper/semver.hpp"
#include "../../../io/path.hpp"
#include "../../../io/cli.hpp"
#include "../../../io/yaml.hpp"


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

        bool verbose; // TODO: 一旦依存関係を集めて，一気に変換だとメモリが保たない．なので，インクリメンタルに変換するのが良い．


        std::vector<std::string>
        hash_source_files(
            std::vector<std::string>&& source_files,
            const bool usemain )
        {
            namespace fs = boost::filesystem;

            if (usemain) {
                if (!fs::exists("main.cpp")) {
                    throw except::error(except::msg::does_not_exist("main.cpp"));
                }
                else {
                    source_files.push_back("main.cpp");
                }
            }
            return core::cache::check_src_cpp(compile_conf, depends_ts, source_files, verbose);
        }

        void make_include_search_path() { // TODO: hashチェック時の大量の文字列配列が恐らくキツイ．
            namespace fs = boost::filesystem;
            namespace lock = deper::lock;
            namespace yaml = io::yaml;
            namespace path = io::path;

            if (const auto locked_deps = lock::load_ignore_timestamp()) {
                for (const auto& [name, dep] : locked_deps->backtracked) {
                    const std::string current_package_name = name::to_current(dep.source, name, dep.version);
                    const fs::path include_dir = path::current_deps_dir / current_package_name / "include";

                    if (path::validate_dir(include_dir)) {
                        compile_conf.include_search_path.push_back(include_dir.string());
                    }
                    else {
                        throw except::error(
                                name + " is not installed.\n"
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
        void configure_compile(const bool usemain)
        {
            namespace yaml = io::yaml;
            namespace path = io::path;

            compile_conf.system = compiler;

            const auto cpp_version = yaml::get_with_throw<std::uint8_t>(node.at("cpp_version"));
            const std::string cn = field::standard::command_to_name(compiler);
            compile_conf.std_version = field::standard::convert(cpp_version, cn, yaml::get(node.at("build"), "gnu"));

//            compile_conf.include_search_path = utils::options::make_include_search_path(exist_deps_key);
            compile_conf.other_args = utils::options::make_compile_other_args(node);
            compile_conf.source_files = hash_source_files(search::cpp(base_dir), usemain);
            compile_conf.macro_defns = utils::options::make_macro_defns(node);
            compile_conf.base_dir = base_dir;
            compile_conf.output_root = path::current_build_cache_obj_dir;
        }
        std::optional<std::vector<std::string>>
        compile() {
            namespace fs = boost::filesystem;

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

        void make_link(const std::map<std::string, YAML::Node>& deps_node) {
            namespace fs = boost::filesystem;
            namespace yaml = io::yaml;

            for (const auto& [raw_name, next_node] : deps_node) {
                const auto [src, name] = name::get_source(raw_name);
                const std::string version = name::get_version(next_node, src);

                // FIXME: srcではなく，build systemを読む．
                if (src != "poac") {
                    const std::string caching_name = name::to_cache(src, name, version); // TODO: これ，なんで，cacheなのに，
                    const fs::path pkgpath = io::path::current_deps_dir / caching_name; // TODO: depsを読んでるん？？？

                    // TODO: できればlockファイルに書かれたパッケージの./depsディレクトリのpoac.ymlを読むのが好ましい
                    if (const fs::path lib_dir = pkgpath / "lib"; fs::exists(lib_dir)) {
                        link_conf.library_search_path.push_back(lib_dir.string());

                        if (const auto link = yaml::get<std::vector<std::string>>(next_node, "link", "include")) {
                            for (const auto& l : *link) {
                                link_conf.static_link_libs.push_back(l);
                            }
                        }
                        else {
                            link_conf.static_link_libs.push_back(caching_name);
                        }
                    }
                }
            }
        }
        void configure_link(const std::vector<std::string>& obj_files_path) // TODO: obj_files_path以外は，インスタンス時に作れるからメモリの無駄遣いにならない．
        {
            namespace yaml = io::yaml;

            link_conf.obj_files_path = obj_files_path;

            link_conf.system = compiler;
            link_conf.project_name = project_name;
            link_conf.output_root = io::path::current_build_bin_dir;
//            make_link();
//            link_conf.library_search_path = std::get<0>(links);
//            link_conf.static_link_libs = std::get<1>(links);
//            link_conf.library_path = std::get<2>(links);
            if (const auto link_args = yaml::get<std::vector<std::string>>(node.at("build"), "link_args")) {
                link_conf.other_args = *link_args;
            }
        }
        auto link()
        {
            return core::compiler::link(link_conf, verbose);
        }

        void configure_static_lib(const std::vector<std::string>& obj_files_path)
        {
            static_lib_conf.project_name = project_name;
            static_lib_conf.output_root = io::path::current_build_lib_dir;
            static_lib_conf.obj_files_path = obj_files_path;
        }
        auto gen_static_lib()
        {
            return core::compiler::gen_static_lib(static_lib_conf, verbose);
        }

        void configure_dynamic_lib(const std::vector<std::string>& obj_files_path)
        {
            dynamic_lib_conf.system = compiler;
            dynamic_lib_conf.project_name = project_name;
            // outputを一箇所か分散か選べるように．boost::hoghoeみたいに，enumのオプションを渡すとOK
            // 一箇所ってのは，./ poac build -> ./_buildだけど，depsも./_buildに配置されるやつ
            dynamic_lib_conf.output_root = io::path::current_build_lib_dir;
            dynamic_lib_conf.obj_files_path = obj_files_path;
        }
        auto gen_dynamic_lib()
        {
            return core::compiler::gen_dynamic_lib(dynamic_lib_conf, verbose);
        }

        // TODO: poac.ymlのhashもcheck
        // TODO: 自らのinclude，dirも，(存在するなら！) includeパスに渡してほしい．そうすると，poacでinclude<poac/poac.hpp>できる
        // TODO: この段階で，どこまでするのかが分かれば，コンパイルしないのに，コンパイル用の設定を生成した，とかが無くなって良さそう．
        explicit builder(const bool verbose, const boost::filesystem::path& base_dir=boost::filesystem::current_path())
        {
            namespace yaml = io::yaml;

            const auto config_file = yaml::load_config_by_dir_with_throw(base_dir);
            node = yaml::get_by_width(config_file, "name", "version", "cpp_version", "build");


            // Create link configure and include search path
            if (const auto deps_node = yaml::get<std::map<std::string, YAML::Node>>(config_file, "deps")) {
                make_link(*deps_node);
                make_include_search_path();
            }


            compiler = field::standard::detect_command();
            project_name = name::slash_to_hyphen(node.at("name").as<std::string>());
            this->base_dir = base_dir;
            this->verbose = verbose;
        }
    };
} // end namespace
#endif // POAC_CORE_STROITE_CORE_BUILDER_HPP

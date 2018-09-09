#ifndef POAC_UTIL_BUILDSYSTEM_HPP
#define POAC_UTIL_BUILDSYSTEM_HPP

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

#include <boost/filesystem.hpp>
#include <boost/optional.hpp>
#include <boost/algorithm/string.hpp>

#include "./compiler.hpp"
#include "./manage_hash.hpp"
#include "../core/exception.hpp"
#include "../io/file/path.hpp"
#include "./package.hpp"


// TODO: Input:
// TODO: Output: Result<T, E>

// TODO: --cache-strict -> hash, nothing -> time stamp ???
namespace poac::util {
    class buildsystem {
    private:
        util::compiler compiler;
        const std::map<std::string, YAML::Node> node;
        const std::string project_name;

        //       cpp_name,             cpp_deps_name, hash
        std::map<std::string, std::map<std::string, std::string>> cpp_hash;


        auto make_source_files() {
            namespace fs = boost::filesystem;

            std::vector<std::string> source_files;
            if (io::file::path::validate_dir(fs::current_path() / "src"))
                for (const fs::path& p : fs::recursive_directory_iterator(fs::current_path() / "src"))
                    if (!fs::is_directory(p) && p.extension().string() == ".cpp")
                        source_files.push_back(p.string());
            return source_files;
        }

        auto make_include_search_path() {
            namespace fs = boost::filesystem;

            std::vector<std::string> include_search_path;
            // TODO: depsをpoac.ymlに書かれているのなら，include_search_pathに追加する！！！！
            // TODO: 無いのならそれはそれで良いので，エラーを放つな！！！！！！！！
            for (const auto& [name, next_node] : node.at("deps").as<std::map<std::string, YAML::Node>>()) {
                const std::string src = package::get_source(next_node);
                const std::string version = package::get_version(next_node, src);
                const std::string pkgname = package::cache_to_current(package::github_conv_pkgname(name, version));
                const fs::path pkgpath = io::file::path::current_deps_dir / pkgname;

                if (const fs::path include_dir = pkgpath / "include"; fs::exists(include_dir))
                    include_search_path.push_back(include_dir.string());
            }
            return include_search_path;
        }

        auto make_macro_defns() {
            std::vector<std::string> macro_defns;
            // poacは，自動で，projectのrootディレクトリの絶対パスをpreprocessor defineします
            macro_defns.push_back(compiler.make_macro_defn("POAC_AUTO_DEF_PROJECT_ROOT", std::getenv("PWD")));
            // TODO: 上のを置き換える
            macro_defns.push_back(compiler.make_macro_defn("POAC_ROOT", std::getenv("PWD")));
            const std::string def_macro_name = boost::to_upper_copy<std::string>(project_name) + "_VERSION";
            macro_defns.push_back(compiler.make_macro_defn(def_macro_name, node.at("version").as<std::string>()));
            return macro_defns;
        }

        auto make_other_args() {
            std::vector<std::string> other_args;
            // TODO: 抽象化
            other_args.push_back("-pthread");
            return other_args;
        }

        // cacheにobjファイルが存在し，hashが存在し，hashが一致する*.cppファイルをsource_filesから除外する
        std::vector<std::string> check_src_cpp(
                const std::string& system,
                const std::string& version_prefix,
                const unsigned int& cpp_version,
                const std::vector<std::string>& include_search_path,
                const std::vector<std::string>& source_files )
        {
            namespace fs = boost::filesystem;

            std::vector<std::string> new_source_files;
            for (const auto& sf : source_files) {
                if (const auto pre_hash = manage_hash::load(manage_hash::to_cache_hash_path(sf))) {
                    if (const auto cur_hash = manage_hash::gen(system, version_prefix, cpp_version, include_search_path, sf)) {
                        // 既に存在するhashファイルと現在のcppファイルのhashが一致するので，
                        // 編集されていないため，コンパイル不要と見做し，除外します．
                        if (*pre_hash == *cur_hash) {
                            continue;
                        }
                            // 既に存在するhashファイルと現在のcppファイルのhashが一致しないので，
                            // コンパイル対象から除外せず，上書き用にhashを記録します．
                        else {
                            cpp_hash[manage_hash::to_cache_hash_path(sf)] = *cur_hash;
                        }
                    }
                }
                else {
                    // hashファイルは存在しないので，hashを生成し，コンパイルします．
                    if (const auto cur_hash = manage_hash::gen(system, version_prefix, cpp_version, include_search_path, sf)) {
                        cpp_hash[manage_hash::to_cache_hash_path(sf)] = *cur_hash;
                    }
                }
                new_source_files.push_back(sf);
            }
            return new_source_files;
        }

        boost::optional<std::vector<std::string>> _compile(
                const bool usemain=false,
                const bool verbose=false )
        {
            const unsigned int& cpp_version = node.at("cpp_version").as<unsigned int>();
            auto source_files = make_source_files();
            if (usemain) source_files.push_back("main.cpp"); // TODO: 存在確認
            const auto include_search_path = make_include_search_path();
            const auto macro_defns = make_macro_defns();
            const auto other_args = make_other_args();

            const std::vector<std::string> new_source_files = check_src_cpp(compiler.system, compiler.version_prefix, cpp_version, include_search_path, source_files);
            // TODO: compileが不要，もしくは，最初から代入されていない場合
            // 既にobjファイルが存在し，hashファイルの検証の結果，変更がなかったため，
            // 既に存在するobj_fileのリストだけ返して，
            // コンパイルはしない．
            // 完全に変更がないということは，リンクする必要も無い
            if (new_source_files.empty())
                return boost::none;

            // cacheの都合上外しているので，compiler.compileの返り値は無視する．
            std::vector<std::string> obj_files;
            for (const auto& s : source_files) {
                obj_files.push_back(compiler.to_cache_obj_path(s));
            }

            const auto ret = compiler.compile(
                    cpp_version,
                    new_source_files,
                    include_search_path,
                    macro_defns,
                    other_args,
                    verbose
            );
            if (ret) {
                namespace fs = boost::filesystem;
                // compileに成功したら，hashを保存しておく
                std::ofstream ofs;
                for (const auto& [hash_name, data] : cpp_hash) {
                    std::string output_string;
                    for (const auto& [fname, hash] : data) {
                        output_string += fname + ": " + hash + "\n";
                    }
                    fs::create_directories(fs::path(hash_name).parent_path());
                    io::file::path::write_to_file(ofs, hash_name, output_string);
                }
                return obj_files;
            }
            else {
                return boost::none;
            }
        }

        // TODO: compileみたいに細かく...
        boost::optional<std::string> _link(const std::vector<std::string>& obj_files, const bool verbose=false) {
            namespace fs = boost::filesystem;

            const boost::filesystem::path& output_path = io::file::path::current_build_bin_dir;

            std::vector<std::string> library_search_path;
            std::vector<std::string> static_link_libs;
            for (const auto& [name, next_node] : node.at("deps").as<std::map<std::string, YAML::Node>>()) {
                const std::string src = package::get_source(next_node);
                const std::string version = package::get_version(next_node, src);
                const std::string pkgname = package::cache_to_current(package::github_conv_pkgname(name, version));
                const fs::path pkgpath = io::file::path::current_deps_dir / pkgname;

                if (const fs::path lib_dir = pkgpath / "lib"; fs::exists(lib_dir)) {
                    // compile: x, link: o, gen_static_lib: x, gen_dynamic_lib: x
                    library_search_path.push_back(lib_dir.string());

                    if (const auto link_config = io::file::yaml::get_by_width(next_node, "link")) {
                        if (const auto link_include_config = io::file::yaml::get_by_width((*link_config).at("link"),
                                                                                          "include")) {
                            for (const auto& c : (*link_include_config).at("include").as<std::vector<std::string>>()) {
                                // compile: x, link: o, gen_static_lib: x, gen_dynamic_lib: x
                                static_link_libs.push_back(c);
                            }
                        }
                        else {
                            static_link_libs.push_back(pkgname);
                        }
                    }
                }
            }
            // TODO: 抽象化 poac.yml????
            std::vector<std::string> other_args;
            other_args.push_back("-pthread");

            return compiler.link(
                    obj_files,
                    output_path,
                    library_search_path,
                    static_link_libs,
                    other_args,
                    verbose
            );
        }

        auto _gen_static_lib(
            const std::vector<std::string>& obj_files,
            const bool verbose=false )
        {
            return compiler.gen_static_lib(
                    obj_files,
                    io::file::path::current_build_lib_dir,
                    verbose
            );
        }
        auto _gen_dynamic_lib(
            const std::vector<std::string>& obj_files,
            const bool verbose=false )
        {
            return compiler.gen_dynamic_lib(
                    obj_files,
                    io::file::path::current_build_lib_dir,
                    verbose
            );
        }

    public:
        buildsystem() :
            node(io::file::yaml::load_setting_file("name", "version",
                                                   "cpp_version",
                                                   "deps", "build")), // TODO: depsを良い感じに外す！！！
            project_name(node.at("name").as<std::string>())  // TODO: asじゃなく，安全なやつで！(nameしか書かれてなかったらエラーになる)
        {
            namespace exception = core::exception;

            compiler.project_name = project_name;
            if (const char* cxx = std::getenv("CXX")) {
                compiler.system = cxx;
            }
            else { // TODO: コンパイラの自動選択
                if (util::command("command -v g+ >/dev/null 2>&1").exec()) {}
                throw exception::error("Environment variable \"CXX\" was not found.\n"
                                       "       Select the compiler and export it.");
            }
        }

        // TODO: compileには成功した．全部成功した．など．．．
        // \ret: compiler.linkの結果をそのまま流す
        boost::optional<std::string> build_bin(const bool usemain=false, const bool verbose=false) {
            if (const auto obj_files = _compile(usemain, verbose)) {
                if (const auto bin_path = _link(*obj_files, verbose)) {
                    return bin_path;
                }
                else {
                    // TODO: リンクの失敗
                    return boost::none;
                }
            }
            else {
                // TODO: コンパイルの失敗，もしくはcacheから不要と判断された
                // TODO: コンパイル失敗後(linkも含む)に，もう一度する時に，hashが残って，
                // TODO: コンパイルできないので，いずれかの失敗があれば，hashは削除すべき
                // TODO: つまり，hashの管理をbuidsystem側に持ってこればできるということ．
                return boost::none;
            }
        }

        boost::optional<std::string> build_stlink_lib(const bool verbose=false) {
            if (const auto obj_files = _compile(false, verbose)) {
                if (const auto stlink_path = _gen_static_lib(*obj_files, verbose)) {
                    return stlink_path;
                }
                else {
                    // スタティックリンクの失敗
                    return boost::none;
                }
            }
            else {
                // TODO: コンパイルの失敗，もしくはcacheから不要と判断された
                return boost::none;
            }
        }

        boost::optional<std::string> build_dylink_lib(const bool verbose=false) {
            if (const auto obj_files = _compile(false, verbose)) {
                if (const auto dylink_path = _gen_dynamic_lib(*obj_files, verbose)) {
                    return dylink_path;
                }
                else {
                    // ダイナミックリンクの失敗
                    return boost::none;
                }
            }
            else {
                // TODO: コンパイルの失敗，もしくはcacheから不要と判断された
                return boost::none;
            }
        }
    };
} // end namespace
#endif // !POAC_UTIL_BUILDSYSTEM_HPP

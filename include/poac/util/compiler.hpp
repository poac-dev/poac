#ifndef POAC_UTIL_COMPILER_HPP
#define POAC_UTIL_COMPILER_HPP

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

#include "./command.hpp"
#include "../io/file/path.hpp"


// TODO: compilerが，cacheの管理までするのはおかしい！！！
// TODO: もう一段抽象化しよう！！！
// TODO: object fileのvectorを渡すとリンクするだけ．sourceの一覧とその他ほげほげ渡すと，コンパイルするだけ．
// TODO: みたいなアーキテクチャで！！！！
namespace poac::util {
    class compiler {
    private:
        std::string version_prefix = "-std=c++";
        std::vector<std::string> include_search_path;
        std::vector<std::string> library_search_path;
        std::vector<std::string> static_link_libs;
        std::vector<std::string> macro_defn;
        std::vector<std::string> compile_other_args;
        std::vector<std::string> link_other_args;
        //       cpp name,             cpp deps name, hash
        std::map<std::string, std::map<std::string, std::string>> cpp_hash;

        boost::optional<std::string> calc_deps(const std::string& src_cpp) {
            command cmd(system);
            cmd += version_prefix + std::to_string(cpp_version);
            for (const auto& isp : include_search_path)
                cmd += "-I" + isp;
            cmd += "-M " + src_cpp;
            return cmd.exec();
        }

        std::vector<std::string> split(const std::string& raw, const std::string& delim) {
            std::vector<std::string> ret_value;
            boost::split(ret_value, raw, boost::is_any_of(delim), boost::algorithm::token_compress_on);
            return ret_value;
        }

        std::string to_cache_obj_name(const std::string& s) {
            namespace fs = boost::filesystem;
            namespace iopath = io::file::path;
            return (iopath::current_build_cache_obj_dir / fs::relative(s)).replace_extension("o").string();
        }

        std::string to_cache_hash_name(const std::string& s) {
            namespace fs = boost::filesystem;
            namespace iopath = io::file::path;
            return (iopath::current_build_cache_hash_dir / fs::relative(s)).string() + ".hash";
        }

    public:
        std::string project_name;
        std::string system; // gcc or clang or ...
        unsigned int cpp_version;
        std::string main_cpp;
        std::vector<std::string> mutable source_files;
        std::vector<std::string> mutable obj_files;
        boost::filesystem::path output_path;

        compiler() {}

        int manup_obj(util::command& cmd, const std::string& src_cpp) const {
            namespace fs = boost::filesystem;

            // TODO: そのobjを使用する．
            const fs::path src_cpp_hash = (io::file::path::current_build_cache_hash_dir / src_cpp).string() + ".hash";
            const fs::path src_o = (io::file::path::current_build_cache_obj_dir / src_cpp).replace_extension("o");
            if (fs::exists(src_cpp_hash) && fs::exists(src_o)) {
                const auto src_cpp_hash_str = io::file::path::read_file(src_cpp_hash.string());
                const auto src_cpp_str      = io::file::path::read_file(src_cpp);
                if (src_cpp_hash_str && src_cpp_str) {
                    std::stringstream sstream(*src_cpp_hash_str);
                    std::size_t result;
                    sstream >> result;
                    // TODO: Compileやlinkなどに失敗した時，cacheを残さない！！！
                    if (result == std::hash<std::string>{}(*src_cpp_str)) {
                        obj_files.push_back(src_o.string());
                        return 1;
                    }
                    else {
                        fs::create_directories(src_cpp_hash.parent_path());
                        std::ofstream ofs(src_cpp_hash.string());
                        ofs << std::hash<std::string>{}(*src_cpp_str);

                        cmd += src_o.string();
                        obj_files.push_back(src_o.string());
                        return 0;
                    }
                }
            }
            else {
                if (const auto src_cpp_str = io::file::path::read_file(src_cpp)) {
                    fs::create_directories(src_cpp_hash.parent_path());
                    std::ofstream ofs(src_cpp_hash.string());
                    ofs << std::hash<std::string>{}(*src_cpp_str);

                    cmd += src_o.string();
                    obj_files.push_back(src_o.string());
                    fs::create_directories(src_o.parent_path());
                    return 0;
                }
            }
            return 0;
        }

        boost::optional<std::map<std::string, std::string>>
        load_hash(const std::string& src_cpp_hash) {
            namespace fs = boost::filesystem;

            if (!fs::exists(src_cpp_hash))
                return boost::none;

            std::ifstream ifs(src_cpp_hash);
            if(!ifs.is_open()){
                return boost::none;
            }

            std::string buff;
            std::map<std::string, std::string> hash;
            while (std::getline(ifs, buff)) {
                std::vector<std::string> list_string = split(buff, ": \n");
                hash[list_string[0]] = list_string[1];
            }
            return hash;
        }

        // *.cpp -> hash
        boost::optional<std::map<std::string, std::string>>
        gen_hash(const std::string& src_cpp) {
            if (const auto ret = calc_deps(src_cpp)) {
                std::vector<std::string> deps_headers = split(*ret, " \n\\");

                deps_headers.erase(deps_headers.begin()); // main.o:
                deps_headers.erase(deps_headers.begin()); // main.cpp

                std::map<std::string, std::string> hash;
                for (const auto& name : deps_headers) {
                    if (const auto str = io::file::path::read_file(name)) {
                        hash.insert(std::make_pair(name, std::to_string(std::hash<std::string>{}(*str))));
                    }
                }
                // sourceファイル自体のhashを計算
                if (const auto str = io::file::path::read_file(src_cpp)) {
                    hash.insert(std::make_pair(src_cpp, std::to_string(std::hash<std::string>{}(*str))));
                }
                return hash;
            }
            return boost::none;
        }

        // cacheにobjファイルが存在し，hashが存在し，hashが一致する*.cppファイルをsource_filesから除外する
        void check_src_cpp() {
            const auto check_hash = [this](const auto& s) {
                namespace fs = boost::filesystem;
                if (const auto pre_hash = load_hash(to_cache_hash_name(s))) {
                    if (const auto cur_hash = gen_hash(s)) {
                        // 既に存在するhashファイルと現在のcppファイルのhashが一致するので，
                        // 編集されていないため，コンパイル不要と見做し，除外します．
                        if (*pre_hash == *cur_hash) {
                            return true;
                        }
                        // 既に存在するhashファイルと現在のcppファイルのhashが一致しないので，
                        // コンパイル対象から除外せず，上書き用にhashを記録します．
                        else { // コンパイルすることが確定したので，output directoryを作成します．
                            fs::create_directories(fs::path(to_cache_hash_name(s)).parent_path());
                            fs::create_directories(fs::path(to_cache_obj_name(s)).parent_path());
                            cpp_hash[to_cache_hash_name(s)] = *cur_hash;
                        }
                    }
                }
                else {
                    // hashファイルは存在しないので，hashを用意し，コンパイルします．
                    if (const auto cur_hash = gen_hash(s)) {
                        fs::create_directories(fs::path(to_cache_hash_name(s)).parent_path());
                        fs::create_directories(fs::path(to_cache_obj_name(s)).parent_path());
                        cpp_hash[to_cache_hash_name(s)] = *cur_hash;
                    }
                }
                return false;
            };
            auto itr_new_end = std::remove_if(source_files.begin(), source_files.end(), check_hash);
            source_files.erase(itr_new_end, source_files.end());
        }

        // hashでチェック後，新たにコンパイルが必要なファイルだけ，compileする．
        boost::optional<std::string> _compile(const bool verbose=false) {
            namespace fs = boost::filesystem;

            // 一旦空にする
            obj_files.clear();
            // listを作る
            for (const auto& s : source_files) {
                obj_files.push_back(to_cache_obj_name(s));
            }

            check_src_cpp();
            // TODO: compileが不要，もしくは，最初から代入されていない場合
            if (source_files.empty())
                return boost::none;

            // compile
            command cmd(system);
            cmd += version_prefix + std::to_string(cpp_version);
            cmd += "-c";
            for (const auto& s : source_files)
                cmd += s;
            for (const auto& isp : include_search_path)
                cmd += "-I" + isp;
            for (const auto& oa : compile_other_args)
                cmd += oa;
            for (const auto& md : macro_defn)
                cmd += md;
            cmd += "-o";
            for (const auto& s : source_files)
                cmd += to_cache_obj_name(s);


            // compileに成功したら，hashを保存しておく
            // TODO: linkにも成功後でなくても大丈夫？？？
            if (verbose) std::cout << cmd << std::endl;
            if (const auto ret = cmd.exec()) {
                std::ofstream ofs;
                for (const auto& [hash_name, data] : cpp_hash) {
                    std::string output_string;
                    for (const auto& [fname, hash] : data) {
                        output_string += fname + ": " + hash + "\n";
                    }
                    io::file::path::write_to_file(ofs, hash_name, output_string);
                }
                return *ret;
            }
            else {
                return boost::none;
            }
        }

        boost::optional<std::string> link(const bool verbose=false) const {
            namespace fs = boost::filesystem;

            fs::create_directories(output_path);
            const std::string project_path = (output_path / project_name).string();

            // Link to executable file
            command cmd(system);
            for (const auto& o : obj_files)
                cmd += o;
            for (const auto& lsp : library_search_path)
                cmd += "-L" + lsp;
            for (const auto& sll : static_link_libs)
                cmd += "-l" + sll;
            for (const auto& loa : link_other_args)
                cmd += loa;
            cmd += "-o " + project_path;

            if (verbose) std::cout << cmd << std::endl;
            return cmd.exec();
        }
        // TODO: current_build_lib_dirに依存しない設計にする．抽象化
        boost::optional<std::string> gen_static_lib(const bool verbose=false) const {
            namespace fs = boost::filesystem;

            command cmd("ar rcs");
            cmd += (io::file::path::current_build_lib_dir / project_name).string() + ".a";
            for (const auto& o : obj_files)
                cmd += o;
            if (verbose)
                std::cout << cmd << std::endl;
            return cmd.exec();
        }
        boost::optional<std::string> gen_dynamic_lib(const bool verbose=false) const {
            namespace fs = boost::filesystem;

            // TODO: check existance .dylib then check cache // なんの話？？
            // TODO: 両方とも生成できない場合がある．その場合の話
            command cmd(system);
            cmd += "-dynamiclib"; // -shared
            for (const auto& o : obj_files)
                cmd += o;
            cmd += "-o";
            cmd += (io::file::path::current_build_lib_dir / project_name).string() + ".dylib";

            if (verbose)
                std::cout << cmd << std::endl;
            return cmd.exec();
        }

        void enable_gnu() {
            version_prefix = "-std=gnu++";
        }

        void add_source_file(const std::string& s) {
            source_files.push_back(s);
        }
        void add_include_search_path(const boost::filesystem::path& p) {
            include_search_path.push_back(p.string());
        }
        void add_library_search_path(const boost::filesystem::path& p) {
            library_search_path.push_back(p.string());
        }
        void add_static_link_lib(const std::string& p) {
            static_link_libs.push_back(p);
        }
        void add_macro_defn(const std::pair<std::string, std::string>& m) {
            macro_defn.push_back("-D" + m.first + "=" + R"(\")" + m.second + R"(\")");
        }
        void add_compile_other_args(const std::string& p) {
            compile_other_args.push_back(p);
        }
        void add_link_other_args(const std::string& p) {
            link_other_args.push_back(p);
        }
    };
} // end namespace
#endif // !POAC_UTIL_COMPILER_HPP

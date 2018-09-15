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
#include "./build_deps.hpp"
#include "../core/exception.hpp"
#include "../io/file/path.hpp"
#include "./package.hpp"
#include "../io/cli.hpp"
#include "../io/file/yaml.hpp"


namespace poac::util {
    struct compile_configure {
        std::string system;
        std::string version_prefix;
        unsigned int cpp_version;
        std::vector<std::string> source_files;
        std::vector<std::string> include_search_path;
        std::vector<std::string> other_args;
        std::vector<std::string> macro_defns;
        boost::filesystem::path output_path;
        bool verbose;
    };
    struct link_configure {
        std::string system;
        std::string project_name;
        boost::filesystem::path output_path;
        std::vector<std::string> obj_files_path;
        std::vector<std::string> library_search_path;
        std::vector<std::string> static_link_libs;
        std::vector<std::string> other_args;
        bool verbose;
    };
    struct static_lib_configure {
        std::string project_name;
        boost::filesystem::path output_path;
        std::vector<std::string> obj_files_path;
        bool verbose;
    };
    struct dynamic_lib_configure {
        std::string system;
        std::string project_name;
        boost::filesystem::path output_path;
        std::vector<std::string> obj_files_path;
        bool verbose;
    };

    template<typename Conf>
    void enable_gnu(Conf& conf) {
        conf.version_prefix = "-std=gnu++";
    }
    std::string default_version_prefix() {
        return "-std=c++";
    }
    std::string make_macro_defn(const std::string& first, const std::string& second) {
        return "-D" + first + "=" + R"(\")" + second + R"(\")";
    }

    auto auto_select_compiler() {
        namespace exception = core::exception;
        // Automatic selection of compiler
        if (util::command("command -v g++ >/dev/null 2>&1").exec())
            return "g++";
        else if (util::command("command -v clang++ >/dev/null 2>&1").exec())
            return "clang++";
        else
            throw exception::error("Environment variable \"CXX\" was not found.\n"
                                   "       Select the compiler and export it.");
    }

    struct buildsystem {
        compile_configure compile_conf;
        link_configure link_conf;
        static_lib_configure static_lib_conf;
        dynamic_lib_configure dynamic_lib_conf;

        std::string system;
        std::string project_name;

        const std::map<std::string, YAML::Node> node;
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
            // Does the key `deps` exist?
            if (const auto deps_node = io::file::yaml::load_setting_file_opt("deps")) {
                // Is it convertible with the specified type?
                if (const auto deps = io::file::yaml::get<std::map<std::string, YAML::Node>>((*deps_node).at("deps"))) {
                    for (const auto&[name, next_node] : *deps) {
                        const std::string src = package::get_source(next_node);
                        const std::string version = package::get_version(next_node, src);
                        const std::string pkgname = package::cache_to_current(
                                package::github_conv_pkgname(name, version));
                        const fs::path pkgpath = io::file::path::current_deps_dir / pkgname;

                        if (const fs::path include_dir = pkgpath / "include"; fs::exists(include_dir))
                            include_search_path.push_back(include_dir.string());
                    }
                }
            }
            return include_search_path;
        }

        auto make_macro_defns() {
            std::vector<std::string> macro_defns;
            // poac automatically define the absolute path of the project's root directory.
            macro_defns.push_back(make_macro_defn("POAC_AUTO_DEF_PROJECT_ROOT", std::getenv("PWD")));
            auto upper_letter = boost::to_upper_copy<std::string>(project_name);
            // ISO C99 requires whitespace after the macro name [-Wc99-extensions]
            std::replace(upper_letter.begin(), upper_letter.end(), '-', '_');
            const std::string def_macro_name = upper_letter + "_VERSION";
            macro_defns.push_back(make_macro_defn(def_macro_name, node.at("version").as<std::string>()));
            return macro_defns;
        }

        auto make_compile_other_args() {
            if (const auto compile_args = io::file::yaml::get1<std::vector<std::string>>(node.at("build"), "compile_args")) {
                return *compile_args;
            }
            else {
                return std::vector<std::string>{};
            }
        }


        std::string to_cache_hash_path(const std::string& s) {
            namespace fs = boost::filesystem;
            namespace iopath = io::file::path;
            return (iopath::current_build_cache_hash_dir / fs::relative(s)).string() + ".hash";
        }

        boost::optional<std::map<std::string, std::string>> hash_load(const std::string& src_cpp_hash) {
            std::ifstream ifs(src_cpp_hash);
            if(!ifs.is_open()){
                return boost::none;
            }

            std::string buff;
            std::map<std::string, std::string> hash;
            while (std::getline(ifs, buff)) {
                std::vector<std::string> list_string = io::file::path::split(buff, ": \n");
                hash[list_string[0]] = list_string[1];
            }
            return hash;
        }

        void insert_file(
                const std::string& filename,
                std::map<std::string, std::string>& hash )
        {
            if (const auto str = io::file::path::read_file(filename)) {
                hash.emplace(
                        filename,
                        std::to_string(
                                std::hash<std::string>{}(*str)
                        )
                );
            }
        }

        // *.cpp -> hash
        boost::optional<std::map<std::string, std::string>> hash_gen(
                const std::string& system,
                const std::string& version_prefix,
                const unsigned int& cpp_version,
                const std::vector<std::string>& include_search_path,
                const std::vector<std::string>& compile_other_args,
                const std::string& src_cpp,
                const bool verbose )
        {
            if (const auto deps_headers = build_deps::gen(
                    system,
                    version_prefix,
                    cpp_version,
                    include_search_path,
                    compile_other_args,
                    src_cpp,
                    verbose))
            {
                std::map<std::string, std::string> hash;
                for (const auto& name : *deps_headers) {
                    // Calculate the hash of the source dependent files.
                    insert_file(name, hash);
                }
                // Calculate the hash of the source file itself.
                insert_file(src_cpp, hash);
                return hash;
            }
            return boost::none;
        }

        // TODO: Divide it finer...
        auto check_src_cpp(
            const std::string& system,
            const std::string& version_prefix,
            const unsigned int& cpp_version,
            const std::vector<std::string>& include_search_path,
            const std::vector<std::string>& compile_other_args,
            const std::vector<std::string>& source_files,
            const bool verbose )
        {
            namespace fs = boost::filesystem;

            std::vector<std::string> new_source_files;
            for (const auto& sf : source_files) {
                if (const auto pre_hash = hash_load(to_cache_hash_path(sf))) {
                    if (const auto cur_hash = hash_gen(
                            system,
                            version_prefix,
                            cpp_version,
                            include_search_path,
                            compile_other_args,
                            sf,
                            verbose) )
                    {
                        // Since hash of already existing hash file
                        //  does not match hash of current cpp file,
                        //  it does not exclude it from compilation,
                        //  and generates hash for overwriting.
                        if (*pre_hash != *cur_hash) {
                            cpp_hash[to_cache_hash_path(sf)] = *cur_hash;
                            new_source_files.push_back(sf);
                        }
                    }
                }
                else {
                    // Since hash file does not exist, generates hash and compiles source file.
                    if (const auto cur_hash = hash_gen(
                            system,
                            version_prefix,
                            cpp_version,
                            include_search_path,
                            compile_other_args,
                            sf,
                            verbose) )
                    {
                        cpp_hash[to_cache_hash_path(sf)] = *cur_hash;
                        new_source_files.push_back(sf);
                    }
                }
            }
            return new_source_files;
            // TODO: データとして持たず，すぐさま書き込み，
            // TODO:  後々不必要な場合に消せば？名称(hashの)だけ保持しておけば，
            // TODO:  メモリ使用量の抑制になる
        }

        auto hash_source_files(
            std::vector<std::string>&& source_files,
            const bool usemain=false,
            const bool verbose=false )
        {
            if (usemain) {
                namespace fs = boost::filesystem;
                if (!fs::exists("main.cpp"))
                    throw core::exception::error("main.cpp does not exists");
                else
                    source_files.push_back("main.cpp");
            }
            return check_src_cpp(
                    compile_conf.system,
                    compile_conf.version_prefix,
                    compile_conf.cpp_version,
                    compile_conf.include_search_path,
                    compile_conf.other_args,
                    source_files,
                    verbose
            );
        }

        void configure_compile(
            const bool usemain=false,
            const bool verbose=false )
        {
            compile_conf.system = system;
            compile_conf.version_prefix = default_version_prefix();
            compile_conf.cpp_version = node.at("cpp_version").as<unsigned int>();
            compile_conf.include_search_path = make_include_search_path();
            compile_conf.other_args = make_compile_other_args();
            compile_conf.source_files = hash_source_files(make_source_files(), usemain, verbose);
            compile_conf.macro_defns = make_macro_defns();
            compile_conf.output_path = io::file::path::current_build_cache_obj_dir;
            compile_conf.verbose = verbose;
        }
        boost::optional<std::vector<std::string>> _compile()
        {
            if (const auto ret = compiler::compile(compile_conf)) {
                namespace fs = boost::filesystem;
                // Since compile succeeded, save hash
                std::ofstream ofs;
                for (const auto& [hash_name, data] : cpp_hash) {
                    std::string output_string;
                    for (const auto& [fname, hash] : data) {
                        output_string += fname + ": " + hash + "\n";
                    }
                    fs::create_directories(fs::path(hash_name).parent_path());
                    io::file::path::write_to_file(ofs, hash_name, output_string);
                }
                // Because it is excluded for the convenience of cache,
                //  ignore the return value of compiler.compile.
                std::vector<std::string> obj_files;
                for (const auto& s : compile_conf.source_files) {
                    obj_files.push_back(
                            (compile_conf.output_path / fs::relative(s))
                                    .replace_extension("o")
                                    .string()
                    );
                }
                return obj_files;
            }
            else {
                return boost::none;
            }
        }

        auto make_link_other_args() {
            if (const auto link_args = io::file::yaml::get1<std::vector<std::string>>(node.at("build"), "link_args")) {
                return *link_args;
            }
            else {
                return std::vector<std::string>{};
            }
        }
        // TODO: Divide it finer...
        auto make_link() {
            namespace fs = boost::filesystem;

            std::vector<std::string> library_search_path;
            std::vector<std::string> static_link_libs;
            if (const auto deps_node = io::file::yaml::load_setting_file_opt("deps")) {
                if (const auto deps = io::file::yaml::get<std::map<std::string, YAML::Node>>((*deps_node).at("deps"))) {
                    for (const auto&[name, next_node] : *deps) {
                        const std::string src = package::get_source(next_node);
                        const std::string version = package::get_version(next_node, src);
                        const std::string pkgname = package::cache_to_current(
                                package::github_conv_pkgname(name, version));
                        const fs::path pkgpath = io::file::path::current_deps_dir / pkgname;

                        if (const fs::path lib_dir = pkgpath / "lib"; fs::exists(lib_dir)) {
                            library_search_path.push_back(lib_dir.string());

                            if (const auto link_config = io::file::yaml::get_by_width(next_node, "link")) {
                                if (const auto link_include_config = io::file::yaml::get_by_width(
                                        (*link_config).at("link"),
                                        "include")) {
                                    for (const auto &c : (*link_include_config).at(
                                            "include").as<std::vector<std::string>>()) {
                                        static_link_libs.push_back(c);
                                    }
                                } else {
                                    static_link_libs.push_back(pkgname);
                                }
                            }
                        }
                    }
                }
            }
            return std::make_pair(library_search_path, static_link_libs);
        }
        void configure_link(
            const std::vector<std::string>& obj_files_path,
            const bool verbose=false )
        {
            link_conf.system = system;
            link_conf.project_name = project_name;
            link_conf.output_path = io::file::path::current_build_bin_dir;
            link_conf.obj_files_path = obj_files_path;
            const auto links = make_link();
            link_conf.library_search_path = links.first;
            link_conf.static_link_libs = links.second;
            link_conf.other_args = make_link_other_args();
            link_conf.verbose = verbose;
        }
        auto _link()
        {
            return compiler::link(link_conf);
        }

        void configure_static_lib(
                const std::vector<std::string>& obj_files_path,
                const bool verbose=false )
        {
            static_lib_conf.project_name = project_name;
            static_lib_conf.output_path = io::file::path::current_build_lib_dir;
            static_lib_conf.obj_files_path = obj_files_path;
            static_lib_conf.verbose = verbose;
        }
        auto _gen_static_lib()
        {
            return compiler::gen_static_lib(static_lib_conf);
        }

        void configure_dynamic_lib(
            const std::vector<std::string>& obj_files_path,
            const bool verbose=false )
        {
            dynamic_lib_conf.system = system;
            dynamic_lib_conf.project_name = project_name;
            dynamic_lib_conf.output_path = io::file::path::current_build_lib_dir;
            dynamic_lib_conf.obj_files_path = obj_files_path;
            dynamic_lib_conf.verbose = verbose;
        }
        auto _gen_dynamic_lib()
        {
            return compiler::gen_dynamic_lib(dynamic_lib_conf);
        }

        // TODO: poac.ymlのhashもcheckしてほしい
        buildsystem() :
            node(io::file::yaml::load_setting_file(
                    "name", "version", "cpp_version", "build"))
        {
            project_name = node.at("name").as<std::string>();
            if (const char* cxx = std::getenv("CXX"))
                system = cxx;
            else // Automatic selection of compiler
                system = auto_select_compiler();
        }
    };
} // end namespace
#endif // !POAC_UTIL_BUILDSYSTEM_HPP

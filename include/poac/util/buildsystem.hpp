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

    // TODO: これ以外は，poac.ymlによって決定される
    auto default_compile_configure() {
        compile_configure compile_conf;
        compile_conf.system = auto_select_compiler();
        compile_conf.version_prefix = "-std=c++";
        return compile_conf;
    }

    struct buildsystem {
        compile_configure compile_conf;
        link_configure link_conf;
        static_lib_configure static_lib_conf;
        dynamic_lib_configure dynamic_lib_conf;

        std::string system;
        std::string project_name;

        const std::map<std::string, YAML::Node> node;
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
            const std::string def_macro_name = boost::to_upper_copy<std::string>(project_name) + "_VERSION";
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

        // TODO: Divide it finer...
        auto check_src_cpp(
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
                        // It is considered unnecessary to compile and excluded
                        //  from the source file because it is not edited
                        //  that the hash file which already exists and
                        //  hash of the current cpp file matches.
                        if (*pre_hash == *cur_hash) {
                            continue;
                        }
                        // Since hash of already existing hash file
                        //  does not match hash of current cpp file,
                        //  it does not exclude it from compilation,
                        //  and generates hash for overwriting.
                        else {
                            cpp_hash[manage_hash::to_cache_hash_path(sf)] = *cur_hash;
                        }
                    }
                }
                else {
                    // Since hash file does not exist, generates hash and compiles source file.
                    if (const auto cur_hash = manage_hash::gen(system, version_prefix, cpp_version, include_search_path, sf)) {
                        cpp_hash[manage_hash::to_cache_hash_path(sf)] = *cur_hash;
                    }
                }
                new_source_files.push_back(sf);
            }
            return new_source_files;
        }

        auto hash_source_files(const bool usemain=false) {
            auto source_files = make_source_files();
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
                    source_files
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
            compile_conf.source_files = hash_source_files(usemain);
            compile_conf.other_args = make_compile_other_args();
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

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
            macro_defns.push_back(compiler.make_macro_defn("POAC_AUTO_DEF_PROJECT_ROOT", std::getenv("PWD")));
            const std::string def_macro_name = boost::to_upper_copy<std::string>(project_name) + "_VERSION";
            macro_defns.push_back(compiler.make_macro_defn(def_macro_name, node.at("version").as<std::string>()));
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

        // TODO: Divide it finer...
        boost::optional<std::vector<std::string>> _compile(
                const bool usemain=false,
                const bool verbose=false )
        {
            const unsigned int& cpp_version = node.at("cpp_version").as<unsigned int>();
            auto source_files = make_source_files();
            if (usemain) {
                if (!boost::filesystem::exists("main.cpp"))
                    throw core::exception::error("main.cpp does not exists");
                else
                    source_files.push_back("main.cpp");
            }
            const auto include_search_path = make_include_search_path();
            const auto macro_defns = make_macro_defns();
            const auto other_args = make_compile_other_args();

            const auto new_source_files = check_src_cpp(compiler.system, compiler.version_prefix, cpp_version, include_search_path, source_files);
            // Since the obj file already exists and has not been changed as a result
            //  of verification of the hash file, return only the list of existing obj_files
            //  and do not compile.
            // There is no necessity of linking that there is no change completely.
            if (new_source_files.empty())
                return std::vector<std::string>{};

            // Because it is excluded for the convenience of cache,
            //  ignore the return value of compiler.compile.
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
        boost::optional<std::string> _link(
                const std::vector<std::string>& obj_files,
                const bool verbose=false,
                const bool run=true )
        {
            namespace fs = boost::filesystem;

            const boost::filesystem::path& output_path = io::file::path::current_build_bin_dir;

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
            const auto other_args = make_link_other_args();

            if (run) {
                return compiler.link(
                        obj_files,
                        output_path,
                        library_search_path,
                        static_link_libs,
                        other_args,
                        verbose
                );
            }
            else { // Only return bin_name without link
                return (output_path / project_name).string();
            }
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
            node(io::file::yaml::load_setting_file("name", "version", "cpp_version", "build")),
            project_name(node.at("name").as<std::string>())
        {
            namespace exception = core::exception;

            compiler.project_name = project_name;
            if (const char* cxx = std::getenv("CXX")) {
                compiler.system = cxx;
            }
            else {
                // Automatic selection of compiler
                if (util::command("command -v g++ >/dev/null 2>&1").exec()) {
                    compiler.system = "g++";
                }
                else if (util::command("command -v clang++ >/dev/null 2>&1").exec()) {
                    compiler.system = "clang++";
                }
                else {
                    throw exception::error("Environment variable \"CXX\" was not found.\n"
                                           "       Select the compiler and export it.");
                }
            }
        }

        boost::optional<std::string> build_bin(const bool usemain=false, const bool verbose=false) {
            namespace fs = boost::filesystem;

            if (const auto obj_files = _compile(usemain, verbose)) {
                if ((*obj_files).empty()) { // No need for compile and link
                    const std::string bin_path = *(_link(*obj_files, verbose, false));
                    std::cout << io::cli::yellow << "Warning: " << io::cli::reset
                              << "There is no change. Binary exists in `" +
                                 fs::relative(bin_path).string() + "`."
                              << std::endl;
                    return bin_path;
                }
                else if (const auto bin_path = _link(*obj_files, verbose)) {
                    std::cout << io::cli::green << "Compiled: " << io::cli::reset
                              << "Output to `" +
                                 fs::relative(*bin_path).string() +
                                 "`"
                              << std::endl;
                    return bin_path;
                }
                else {
                    // Link failure
                    return boost::none;
                }
            }
            else {
                // Compile failure
                return boost::none;
            }
        }

        boost::optional<std::string> build_link_libs(const bool verbose = false) {
            namespace fs = boost::filesystem;

            if (const auto obj_files = _compile(false, verbose)) {
                if ((*obj_files).empty()) { // No need for compile and link
                    const std::string lib_path = *(_link(*obj_files, verbose, false));
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
                    return lib_path;
                }

                if (const auto lib_path = _gen_static_lib(*obj_files, verbose)) {
                    std::cout << io::cli::green << "Generated: " << io::cli::reset
                              << "Output to `" +
                                 fs::relative(*lib_path).string() +
                                 ".a" + "`"
                              << std::endl;
//                    return lib_path;
                }
                else { // Static link library generation failed
//                    return boost::none;
                }

                if (const auto lib_path = _gen_dynamic_lib(*obj_files, verbose)) {
                    std::cout << io::cli::green << "Generated: " << io::cli::reset
                              << "Output to `" +
                                 fs::relative(*lib_path).string() +
                                 ".dylib" + "`"
                              << std::endl;
//                    return lib_path;
                }
                else {
                    // Dynamic link library generation failed
//                    return boost::none;
                }

                // TODO:
                return boost::none;
            }
            else {
                // Compile failure
                return boost::none;
            }
        }
    };
} // end namespace
#endif // !POAC_UTIL_BUILDSYSTEM_HPP

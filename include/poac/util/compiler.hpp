#ifndef POAC_UTIL_COMPILER_HPP
#define POAC_UTIL_COMPILER_HPP

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <iterator>
#include <functional>

#include <boost/filesystem.hpp>
#include <boost/optional.hpp>

#include "./command.hpp"
#include "../io/file/path.hpp"


namespace poac::util {
    class compiler {
    private:
        std::string version_prefix = "-std=c++";
        std::vector<std::string> source_files;
        std::vector<std::string> include_search_path;
        std::vector<std::string> library_search_path;
        std::vector<std::string> static_link_libs;
        std::vector<std::string> macro_defn;
        std::vector<std::string> other_args;
        std::vector<std::string> mutable obj_files;
        bool mutable cache_use = false;

    public:
        std::string project_name;
        std::string system; // gcc or clang or ...
        unsigned int cpp_version;
        std::string main_cpp;
        boost::filesystem::path output_path;

        compiler() {}


        int manup_obj(util::command& cmd, const std::string& src_cpp) const {
            namespace fs = boost::filesystem;

            const fs::path src_cpp_hash = (io::file::path::current_build_cache_hash_dir / src_cpp).string() + ".hash";
            const fs::path src_o = (io::file::path::current_build_cache_obj_dir / src_cpp).replace_extension("o");
            if (fs::exists(src_cpp_hash) && fs::exists(src_o)) {
                const auto src_cpp_hash_str = io::file::path::read_file(src_cpp_hash.string());
                const auto src_cpp_str      = io::file::path::read_file(src_cpp);
                if (src_cpp_hash_str && src_cpp_str) {
                    std::stringstream sstream(*src_cpp_hash_str);
                    std::size_t result;
                    sstream >> result;
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

        boost::optional<std::string> compile(const bool verbose=false, const bool use_main=false) const {
            namespace fs = boost::filesystem;

            if (obj_files.empty()) {
                command cmd(system);
                cmd += version_prefix + std::to_string(cpp_version);
                cmd += "-c";
                if (use_main && !main_cpp.empty())
                    cmd += main_cpp;
                for (const auto& s : source_files)
                    cmd += s;
                if (!include_search_path.empty())
                    for (const auto& isp : include_search_path)
                        cmd += "-I" + isp;
                if (!other_args.empty())
                    for (const auto& oa : other_args)
                        cmd += oa;
                if (!macro_defn.empty())
                    for (const auto& md : macro_defn)
                        cmd += md;

                int use_cache_count = 0;
                cmd += "-o";
                if (use_main && !main_cpp.empty())
                    use_cache_count = manup_obj(cmd, main_cpp);
                for (const auto& s : source_files)
                    use_cache_count += manup_obj(cmd, fs::relative(s).string());

                if (use_cache_count != static_cast<int>(obj_files.size())) {
                    if (verbose) std::cout << cmd << std::endl;
                    return cmd.run();
                }
                else {
                    cache_use = true;
                    return boost::none;
                }
            }
            else if (cache_use) {
                return boost::none;
            }
            else {
                return std::string();
            }
        }
        boost::optional<std::string> link(const bool verbose=false) const {
            namespace fs = boost::filesystem;

            if (compile(verbose, true)) {
                const std::string project_path = (output_path / project_name).string();

                // Link to executable file
                command cmd(system);
                for (const auto& o : obj_files)
                    cmd += o;
                for (const auto& lsp : library_search_path)
                    cmd += "-L" + lsp;
                for (const auto& sll : static_link_libs)
                    cmd += "-l" + sll;
                cmd += "-o " + project_path;

                if (verbose) std::cout << cmd << std::endl;
                return cmd.run();
            }
            else {
                return boost::none;
            }
        }
        boost::optional<std::string> gen_static_lib(const bool verbose=false) const {
            namespace fs = boost::filesystem;

            if (compile(verbose)) {
                command cmd("ar rcs");
                cmd += (io::file::path::current_build_lib_dir / project_name).string() + ".a";
                for (const auto& o : obj_files)
                    cmd += o;
                if (verbose) std::cout << cmd << std::endl;
                return cmd.run();
            }
            else {
                return boost::none;
            }
        }
        boost::optional<std::string> gen_dynamic_lib(const bool verbose=false) const {
            namespace fs = boost::filesystem;

            // TODO: check existance .dylib then check cache
            if (compile(verbose)) {
                command cmd(system);
                cmd += "-dynamiclib"; // -shared
                for (const auto& o : obj_files)
                    cmd += o;
                cmd += "-o";
                cmd += (io::file::path::current_build_lib_dir / project_name).string() + ".dylib";

                if (verbose) std::cout << cmd << std::endl;
                return cmd.run();
            }
            else {
                return boost::none;
            }
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
        void add_other_args(const std::string& p) {
            other_args.push_back(p);
        }
    };
} // end namespace
#endif // !POAC_UTIL_COMPILER_HPP

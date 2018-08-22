#ifndef POAC_UTIL_COMPILER_HPP
#define POAC_UTIL_COMPILER_HPP

#include <iostream>
#include <string>
#include <vector>

#include <boost/filesystem.hpp>
#include <boost/optional.hpp>

#include "./command.hpp"
#include "../io/file/path.hpp"


namespace poac::util {
    class compiler {
    private:
        const std::string project_name;
        std::string system;
        std::string version_prefix = "-std=c++";
        std::string version;
        std::string libname;
        std::vector<std::string> source_files;
        std::vector<std::string> include_search_path;
        std::vector<std::string> library_search_path;
        std::vector<std::string> static_link_libs;
        std::vector<std::string> macro_defn;
        std::vector<std::string> other_args;
        std::vector<std::string> obj_files;
        boost::filesystem::path output_path;

    public:
        compiler() {}
        compiler(const std::string& s) : project_name(s) {}

        // TODO: Use cache.
        // Generate object file
        boost::optional<std::string> gen_object(bool verbose=false) {
            namespace fs = boost::filesystem;

            if (obj_files.empty()) {
                command cmd(system);
                cmd += version_prefix + version;
                cmd += "-c";
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
                cmd += "-o";
                for (const auto& s : source_files) {
                    fs::path tmp = fs::path(s).replace_extension("o");
                    fs::path source = io::file::path::current_build_cache_dir / fs::relative(tmp, "./src");
                    cmd += source.string();
                    obj_files.push_back(source.string());
                    fs::create_directories(source.parent_path());
                }

                if (verbose) std::cout << cmd << std::endl;
                return cmd.run();
            }
            else {
                return std::string();
            }
        }
        boost::optional<std::string> compile(bool verbose=false) {
            namespace fs = boost::filesystem;

            if (gen_object(verbose)) {
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
        boost::optional<std::string> gen_static_lib(bool verbose=false) {
            namespace fs = boost::filesystem;

            if (gen_object(verbose)) {
                command cmd("ar rcs");
                cmd += (io::file::path::current_build_lib_dir / libname).string() + ".a";
                for (const auto& o : obj_files)
                    cmd += o;
                if (verbose) std::cout << cmd << std::endl;
                return cmd.run();
            }
            else {
                return boost::none;
            }
        }
        boost::optional<std::string> gen_dynamic_lib(bool verbose=false) {
            namespace fs = boost::filesystem;

            if (gen_object(verbose)) {
                command cmd("clang++");
                cmd += "-dynamiclib";
                for (const auto& o : obj_files)
                    cmd += o;
                cmd += "-o";
                cmd += (io::file::path::current_build_lib_dir / libname).string() + ".dylib";

                if (verbose) std::cout << cmd << std::endl;
                return cmd.run();
            }
            else {
                return boost::none;
            }
        }


        void set_system(const std::string& s) {
            system = s;
        }
        void set_version(const unsigned int& v) {
            version = std::to_string(v);
        }
        void enable_gnu() {
            version_prefix = "-std=gnu++";
        }

        void set_output_path(const boost::filesystem::path& p) {
            output_path = p;
        }

        void set_libname(const std::string& s) {
            libname = s;
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

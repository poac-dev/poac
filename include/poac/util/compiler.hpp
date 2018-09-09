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

#include "./command.hpp"
#include "./build_deps.hpp"
#include "./manage_hash.hpp"
#include "../io/file/path.hpp"


namespace poac::util {
    class compiler {
    public:
        std::string version_prefix = "-std=c++";

        std::string to_cache_obj_path(const std::string& s) {
            namespace fs = boost::filesystem;
            namespace iopath = io::file::path;
            return (iopath::current_build_cache_obj_dir / fs::relative(s)).replace_extension("o").string();
        }

        // Use binary name, static link library name, dynamic link library name
        std::string project_name;
        std::string system; // gcc or clang or ...

        compiler() {}

        // hashでチェック後，新たにコンパイルが必要なファイルだけ，compileする．
        // このコンパイルに**関わった**(関わっていない，つまりcacheの検知によって除外された場合は含まれない)，
        // obj_fileを返り値として返す．
        // コンパイルしなければnone
        boost::optional<std::vector<std::string>> compile(
                const unsigned int& cpp_version,
                const std::vector<std::string>& source_files,
                const std::vector<std::string>& include_search_path,
                const std::vector<std::string>& macro_defns,
                const std::vector<std::string>& other_args,
                const bool verbose )
        {
            namespace fs = boost::filesystem;

            // compile
            command cmd(system);
            cmd += version_prefix + std::to_string(cpp_version);
            cmd += "-c";
            for (const auto& s : source_files)
                cmd += s;
            for (const auto& isp : include_search_path)
                cmd += "-I" + isp;
            for (const auto& oa : other_args)
                cmd += oa;
            for (const auto& md : macro_defns)
                cmd += md;
            cmd += "-o";
            std::vector<std::string> obj_files;
            for (const auto& s : source_files) {
                const std::string obj_path = to_cache_obj_path(s);
                obj_files.push_back(obj_path);
                fs::create_directories(fs::path(obj_path).parent_path());
                cmd += obj_path;
            }

            if (verbose) std::cout << cmd << std::endl;

            if (const auto ret = cmd.exec()) {
                return obj_files;
            }
            else { // コンパイルに失敗
                return boost::none;
            }
        }

        boost::optional<std::string> link(
                const std::vector<std::string>& obj_files,
                const boost::filesystem::path& output_path,
                const std::vector<std::string>& library_search_path,
                const std::vector<std::string>& static_link_libs,
                const std::vector<std::string>& other_args,
                const bool verbose )
        {
            namespace fs = boost::filesystem;

            fs::create_directories(output_path);
            const std::string bin_path = (output_path / project_name).string();

            // Link to executable file
            command cmd(system);
            for (const auto& o : obj_files)
                cmd += o;
            for (const auto& lsp : library_search_path)
                cmd += "-L" + lsp;
            for (const auto& sll : static_link_libs)
                cmd += "-l" + sll;
            for (const auto& oa : other_args)
                cmd += oa;
            cmd += "-o " + bin_path;

            if (verbose) std::cout << cmd << std::endl;

            if (const auto ret = cmd.exec()) {
                return bin_path;
            }
            else {
                return boost::none;
            }
        }

        boost::optional<std::string> gen_static_lib(
            const std::vector<std::string>& obj_files,
            const boost::filesystem::path& output_path,
            const bool verbose=false ) const
        {
            namespace fs = boost::filesystem;

            command cmd("ar rcs");
            const std::string stlib_path = (output_path / project_name).string() + ".a";
            cmd += stlib_path;
            for (const auto& o : obj_files)
                cmd += o;

            if (verbose)
                std::cout << cmd << std::endl;

            if (const auto ret = cmd.exec())
                return stlib_path;
            else
                return boost::none;
        }
        boost::optional<std::string> gen_dynamic_lib(
            const std::vector<std::string>& obj_files,
            const boost::filesystem::path& output_path,
            const bool verbose=false ) const
        {
            namespace fs = boost::filesystem;

            command cmd(system);
            cmd += "-dynamiclib"; // -shared
            for (const auto& o : obj_files)
                cmd += o;
            cmd += "-o";
            const std::string dylib_path = (output_path / project_name).string() + ".dylib";
            cmd += dylib_path;

            if (verbose)
                std::cout << cmd << std::endl;

            if (const auto ret = cmd.exec())
                return dylib_path;
            else
                return boost::none;
        }


        void enable_gnu() {
            version_prefix = "-std=gnu++";
        }
        std::string make_macro_defn(const std::string& first, const std::string& second) {
            return "-D" + first + "=" + R"(\")" + second + R"(\")";
        }
    };
} // end namespace
#endif // !POAC_UTIL_COMPILER_HPP

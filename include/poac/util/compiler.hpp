#ifndef POAC_UTIL_COMPILER_HPP
#define POAC_UTIL_COMPILER_HPP

#include <iostream>
#include <fstream>
#include <string>
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
        const std::string project_name;
        std::string system;
        std::string version_prefix = "-std=c++";
        std::string version;
        std::string main_cpp;
        std::vector<std::string> source_files;
        std::vector<std::string> include_search_path;
        std::vector<std::string> library_search_path;
        std::vector<std::string> static_link_libs;
        std::vector<std::string> macro_defn;
        std::vector<std::string> other_args;
        std::vector<std::string> mutable obj_files;
        boost::filesystem::path output_path;

    public:
        compiler() {}
        compiler(const std::string& s) : project_name(s) {}

        void manup_obj(util::command& cmd, const std::string& src_cpp) const {
            namespace fs = boost::filesystem;

            if (std::ifstream ifs(src_cpp); !ifs.fail()) {
                std::istreambuf_iterator<char> it(ifs);
                std::istreambuf_iterator<char> last;
                std::string str(it, last);
                const fs::path src_cpp_hash = (io::file::path::current_build_cache_hash_dir / src_cpp).string() + ".hash";
                fs::create_directories(src_cpp_hash.parent_path());
                std::ofstream ofs(src_cpp_hash.string());
                ofs << std::hash<std::string>{}(str);
            }
            const fs::path src_o = (io::file::path::current_build_cache_obj_dir / src_cpp).replace_extension("o");
            // TODO: hppとかの依存関係のチェックができたら，include_search_pathも影響を受けるはず！
            cmd += src_o.string(); // TODO: もし，すでにhashファイルが存在していて，そのファイルの内容が一致しているならば，cmdにaddしない．
            obj_files.push_back(src_o.string()); // TODO: ただし，Linkには必要なので，ここはaddする．
            fs::create_directories(src_o.parent_path());
        }

        // Generate object file
        boost::optional<std::string> gen_object(const bool verbose=false, const bool use_main=false) const {
            namespace fs = boost::filesystem;

            if (obj_files.empty()) {
                command cmd(system);
                cmd += version_prefix + version;
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
                cmd += "-o";

                if (use_main && !main_cpp.empty())
                    manup_obj(cmd, main_cpp);
                for (const auto& s : source_files)
                    manup_obj(cmd, fs::relative(s).string());

                if (verbose) std::cout << cmd << std::endl;
                return cmd.run();
            }
            else {
                return std::string();
            }
        }
        boost::optional<std::string> compile(const bool verbose=false) const {
            namespace fs = boost::filesystem;

            if (gen_object(verbose, true)) {
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

            if (gen_object(verbose)) {
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

            if (gen_object(verbose)) {
                command cmd("clang++");
                cmd += "-dynamiclib";
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

        void set_main_cpp(const std::string& s) {
            main_cpp = s;
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

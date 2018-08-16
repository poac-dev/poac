#ifndef POAC_UTIL_COMPILER_HPP
#define POAC_UTIL_COMPILER_HPP

#include <iostream>
#include <string>
#include <vector>

#include <boost/filesystem.hpp>
#include <boost/optional.hpp>

#include "./command.hpp"


namespace poac::util {
    class compiler {
    private:
        const std::string project_name;
        std::string system;
        std::string version_prefix = "-std=c++";
        std::string version;
        std::string source_file;
        std::vector<std::string> include_search_path;
        std::vector<std::string> library_search_path;
        std::vector<std::string> static_link_libs;
        std::vector<std::string> other_args;
        boost::filesystem::path output_path;

    public:
//        std::string data() const { return cmd; }
// compier.hppの対応をし！srcの中のincludeを外にだす！
        compiler(const std::string& s) : project_name(s) {}
        boost::optional<std::string> compile() {
            return create_command().run();
        }

        friend std::ostream& operator<<(std::ostream& stream, const compiler& c) {
            stream << c.create_command();
            return stream;
        }

        command create_command() const {
            const std::string project_path = (output_path / project_name).string();
            const std::string object_file_path = project_path + ".o";

            // Generate object file
            command cmd(system);
            cmd += "-o " + object_file_path;
            cmd += version_prefix + version;
            for (const auto& isp : include_search_path)
                cmd += "-I" + isp;
            for (const auto& oa : other_args)
                cmd += oa;
            cmd += "-c " + source_file;

            cmd &= "\n";

            // Link to executable file
            cmd += system;
            cmd += "-o " + project_path;
            for (const auto& lsp : library_search_path)
                cmd += "-L" + lsp;
            for (const auto& sll : static_link_libs)
                cmd += "-l" + sll;
            cmd += object_file_path;

            return cmd;
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

        void set_source_file(const std::string& s) {
            source_file = s;
        }
        void set_output_path(const boost::filesystem::path& p) {
            output_path = p;
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
        void add_other_args(const std::string& p) {
            other_args.push_back(p);
        }
    };
} // end namespace
#endif // !POAC_UTIL_COMPILER_HPP

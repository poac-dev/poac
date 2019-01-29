#ifndef STROITE_UTILS_OPTIONS_HPP
#define STROITE_UTILS_OPTIONS_HPP

#include <string>
#include <vector>
#include <numeric>

#include <boost/filesystem.hpp>

#include "../../command.hpp"


namespace stroite::utils::options {
    namespace fs = boost::filesystem;
    using command = poac::util::command;

    struct compile {
        std::string system; // TODO: systemだけ別の管理にして，compiler.hppに，system, std::string optsとして渡したい．
        std::string version_prefix;
        unsigned int cpp_version;
        std::vector<std::string> source_files;
        std::vector<std::string> include_search_path;
        std::vector<std::string> other_args;
        std::vector<std::string> macro_defns;
        boost::filesystem::path base_dir;
        boost::filesystem::path output_root;
        bool verbose; // TODO: これ，別で渡せない？？？
    };
    std::string to_string(const compile& c) {
        command opts;
        opts += c.version_prefix + std::to_string(c.cpp_version);
        opts += "-c";
        opts += accumulate(begin(c.source_files), end(c.source_files), command());
        opts += accumulate(begin(c.include_search_path), end(c.include_search_path), command(),
                [](command acc, auto s) { return acc + ("-I" + s); });
        opts += accumulate(begin(c.other_args), end(c.other_args), command());
        opts += accumulate(begin(c.macro_defns), end(c.macro_defns), command());
        opts += "-o";
        for (const auto& s : c.source_files) {
            auto obj_path = c.output_root / fs::relative(s);
            obj_path.replace_extension("o");
            fs::create_directories(obj_path.parent_path());
            opts += obj_path.string();
        }
        return opts.data();
    }

    struct link {
        std::string system;
        std::string project_name;
        boost::filesystem::path output_root;
        std::vector<std::string> obj_files_path;
        std::vector<std::string> library_search_path;
        std::vector<std::string> static_link_libs;
        std::vector<std::string> library_path;
        std::vector<std::string> other_args;
        bool verbose;
    };
    std::string to_string(const link& l) {
        command opts;
        opts += accumulate(begin(l.obj_files_path), end(l.obj_files_path), command());
        opts += accumulate(begin(l.library_search_path), end(l.library_search_path), command(),
                           [](command acc, auto s) { return acc + ("-L" + s); });
        opts += accumulate(begin(l.static_link_libs), end(l.static_link_libs), command(),
                           [](command acc, auto s) { return acc + ("-l" + s); });
        opts += accumulate(begin(l.library_path), end(l.library_path), command());
        opts += accumulate(begin(l.other_args), end(l.other_args), command());
        opts += "-o " + (l.output_root / l.project_name).string();
        return opts.data();
    }

    struct static_lib {
        std::string project_name;
        boost::filesystem::path output_root;
        std::vector<std::string> obj_files_path;
        bool verbose;
    };
    std::string to_string(const static_lib& s) {
        command opts;
        opts += (s.output_root / s.project_name).string() + ".a";
        opts += accumulate(begin(s.obj_files_path), end(s.obj_files_path), command());
        return opts.data();
    }

    struct dynamic_lib {
        std::string system;
        std::string project_name;
        boost::filesystem::path output_root;
        std::vector<std::string> obj_files_path;
        bool verbose;
    };
    std::string to_string(const dynamic_lib& d) {
        command opts;
#ifdef __APPLE__
        opts += "-dynamiclib";
        const std::string extension = ".dylib";
#else
        opts += "-shared -fPIC";
        const std::string extension = ".so";
#endif
        opts += accumulate(begin(d.obj_files_path), end(d.obj_files_path), command());
        opts += "-o";
        opts += (d.output_root / d.project_name).string() + extension;
        return opts.data();
    }
} // end namespace
#endif // STROITE_UTILS_OPTIONS_HPP

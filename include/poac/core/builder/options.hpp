#ifndef POAC_CORE_BUILDER_OPTIONS_HPP
#define POAC_CORE_BUILDER_OPTIONS_HPP

#include <cstdint>
#include <string>
#include <sstream>
#include <vector>
#include <numeric>

#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>

#include "./absorb.hpp"
#include "../except.hpp"
#include "../name.hpp"
#include "../resolver/lock.hpp"
#include "../../io/path.hpp"
#include "../../io/cli.hpp"
#include "../../io/yaml.hpp"
#include "../../util/semver.hpp"
#include "../../util/shell.hpp"

namespace poac::core::builder::options {
    struct compile {
        std::string system; // TODO: systemだけ別の管理にして，compiler.hppに，system, std::string optsとして渡したい．
        std::string std_version;
        std::vector<std::string> source_files;
        std::string source_file;
        std::vector<std::string> include_search_path;
        std::vector<std::string> other_args;
        std::vector<std::string> macro_defns;
        boost::filesystem::path base_dir;
        boost::filesystem::path output_root;
    };
    std::string to_string(const compile& c) {
        namespace fs = boost::filesystem;
        using command = util::shell;

        command opts;
        opts += c.std_version;
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
        return opts.string();
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
    };
    std::string to_string(const link& l) {
        using command = util::shell;

        command opts;
        opts += accumulate(begin(l.obj_files_path), end(l.obj_files_path), command());
        opts += accumulate(begin(l.library_search_path), end(l.library_search_path), command(),
                           [](command acc, auto s) { return acc + ("-L" + s); });
        opts += accumulate(begin(l.static_link_libs), end(l.static_link_libs), command(),
                           [](command acc, auto s) { return acc + ("-l" + s); });
        opts += accumulate(begin(l.library_path), end(l.library_path), command());
        opts += accumulate(begin(l.other_args), end(l.other_args), command());
        opts += "-o " + (l.output_root / l.project_name).string();
        return opts.string();
    }

    struct static_lib {
        std::string project_name;
        boost::filesystem::path output_root;
        std::vector<std::string> obj_files_path;
    };
    std::string to_string(const static_lib& s) {
        using command = util::shell;

        command opts;
        opts += (s.output_root / s.project_name).string() + ".a";
        opts += accumulate(begin(s.obj_files_path), end(s.obj_files_path), command());
        return opts.string();
    }

    struct dynamic_lib {
        std::string system;
        std::string project_name;
        boost::filesystem::path output_root;
        std::vector<std::string> obj_files_path;
    };
    std::string to_string(const dynamic_lib& d) {
        using command = util::shell;

        command opts;
        opts += absorb::dynamic_lib_option;
        const std::string extension = absorb::dynamic_lib_extension;
        opts += accumulate(begin(d.obj_files_path), end(d.obj_files_path), command());
        opts += "-o";
        opts += (d.output_root / d.project_name).string() + extension;
        return opts.string();
    }


    template <typename T>
    std::string make_macro_defn(const std::string& first, const T& second) {
        return make_macro_defn(first, std::to_string(second));
    }
    template <>
    std::string make_macro_defn<std::string>(const std::string& first, const std::string& second) {
        return "-D" + first + "=" + R"(\")" + second + R"(\")";
    }
    template <>
    std::string make_macro_defn<std::uint64_t>(const std::string& first, const std::uint64_t& second) {
        std::ostringstream oss;
        oss << second;
        return make_macro_defn(first, oss.str());
    }

    std::vector<std::string>
    make_macro_defns(const std::map<std::string, YAML::Node>& node) {
        namespace fs = boost::filesystem;
        namespace yaml = io::yaml;

        std::vector<std::string> macro_defns;
        // poac automatically define the absolute path of the project's root directory.
        // TODO: これ，依存関係もこれ使ってたら，それも，ルートのにならへん？header-only libの時
        macro_defns.emplace_back(make_macro_defn("POAC_PROJECT_ROOT", fs::current_path().string()));
        const auto version = resolver::semver::Version(yaml::get_with_throw<std::string>(node.at("version")));
        macro_defns.emplace_back(make_macro_defn("POAC_VERSION", version.get_full()));
        macro_defns.emplace_back(make_macro_defn("POAC_MAJOR_VERSION", version.major));
        macro_defns.emplace_back(make_macro_defn("POAC_MINOR_VERSION", version.minor));
        macro_defns.emplace_back(make_macro_defn("POAC_PATCH_VERSION", version.patch));
        return macro_defns;
    }

    std::vector<std::string>
    make_compile_other_args(const std::map<std::string, YAML::Node>& node) {
        namespace yaml = io::yaml;
        if (const auto compile_args = yaml::get<std::vector<std::string>>(node.at("build"), "compile_args")) {
            return *compile_args;
        }
        else {
            return {};
        }
    }
} // end namespace
#endif // POAC_CORE_BUILDER_OPTIONS_HPP

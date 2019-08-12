#ifndef POAC_CORE_BUILDER_OPTIONS_HPP
#define POAC_CORE_BUILDER_OPTIONS_HPP

#include <cstdint>
#include <string>
#include <sstream>
#include <vector>
#include <numeric>

#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>

#include <poac/core/builder/absorb.hpp>
#include <poac/io/config.hpp>
#include <poac/util/semver/semver.hpp>
#include <poac/util/shell.hpp>

namespace poac::core::builder::options {
    template <typename SinglePassRange, typename T>
    T accumulate(const SinglePassRange& rng, T init) {
        return std::accumulate(std::cbegin(rng), std::cend(rng), init);
    }
    template <typename SinglePassRange, typename T, typename BinaryOp>
    T accumulate(const SinglePassRange& rng, T init, BinaryOp binary_op) {
        return std::accumulate(std::cbegin(rng), std::cend(rng), init, binary_op);
    }

    struct compile {
        std::string system;
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
        util::shell opts;
        opts += c.std_version;
        opts += "-c";
        opts += accumulate(c.source_files, util::shell());
        opts += accumulate(c.include_search_path, util::shell(),
                [](util::shell acc, auto s) { return acc + ("-I" + s); });
        opts += accumulate(c.other_args, util::shell());
        opts += accumulate(c.macro_defns, util::shell());
        opts += "-o";
        for (const auto& s : c.source_files) {
            auto obj_path = c.output_root / boost::filesystem::relative(s);
            obj_path.replace_extension("o");
            boost::filesystem::create_directories(obj_path.parent_path());
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
        util::shell opts;
        opts += accumulate(l.obj_files_path, util::shell());
        opts += accumulate(l.library_search_path, util::shell(),
                [](util::shell acc, auto s) { return acc + ("-L" + s); });
        opts += accumulate(l.static_link_libs, util::shell(),
                [](util::shell acc, auto s) { return acc + ("-l" + s); });
        opts += accumulate(l.library_path, util::shell());
        opts += accumulate(l.other_args, util::shell());
        opts += "-o " + (l.output_root / l.project_name).string();
        return opts.string();
    }

    struct static_lib {
        std::string project_name;
        boost::filesystem::path output_root;
        std::vector<std::string> obj_files_path;
    };
    std::string to_string(const static_lib& s) {
        util::shell opts;
        opts += (s.output_root / s.project_name).string() + ".a";
        opts += accumulate(s.obj_files_path, util::shell());
        return opts.string();
    }

    struct dynamic_lib {
        std::string system;
        std::string project_name;
        boost::filesystem::path output_root;
        std::vector<std::string> obj_files_path;
    };
    std::string to_string(const dynamic_lib& d) {
        util::shell opts;
        opts += absorb::dynamic_lib_option;
        opts += accumulate(d.obj_files_path, util::shell());
        opts += "-o";
        opts += (d.output_root / d.project_name).string() + absorb::dynamic_lib_extension;
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
    make_macro_defns(const io::config::Config&) {
        namespace fs = boost::filesystem;

        std::vector<std::string> macro_defns;
        // poac automatically define the absolute path of the project's root directory.
        macro_defns.emplace_back(make_macro_defn("POAC_PROJECT_ROOT", fs::current_path().string()));
//        const auto version = semver::Version(config->version); // TODO: versionが無い
//        macro_defns.emplace_back(make_macro_defn("POAC_VERSION", version.get_full()));
//        macro_defns.emplace_back(make_macro_defn("POAC_MAJOR_VERSION", version.major));
//        macro_defns.emplace_back(make_macro_defn("POAC_MINOR_VERSION", version.minor));
//        macro_defns.emplace_back(make_macro_defn("POAC_PATCH_VERSION", version.patch));
        return macro_defns;
    }

//    std::vector<std::string>
//    make_compile_other_args(const std::map<std::string, YAML::Node>& node) {
//        namespace yaml = io::yaml;
//        if (const auto compile_args = io::yaml::get<std::vector<std::string>>(node.at("build"), "compile_args")) {
//            return *compile_args;
//        }
//        else {
//            return {};
//        }
//    }
} // end namespace
#endif // POAC_CORE_BUILDER_OPTIONS_HPP

#ifndef POAC_CORE_STROITE_UTILS_CONFIG_HPP
#define POAC_CORE_STROITE_UTILS_CONFIG_HPP

#include <iostream>
#include <string>
#include <sstream>
#include <vector>

#include <boost/algorithm/string.hpp>

#include "../../exception.hpp"
#include "../../naming.hpp"
#include "../../deper/lock.hpp"
#include "../../deper/semver.hpp"
#include "../../../io/file/path.hpp"
#include "../../../io/cli.hpp"
#include "../../../io/file/yaml.hpp"


namespace poac::core::stroite::utils::config { // FIXME: configという名称はここには適切ではない．
    template <typename Opts>
    void enable_gnu(Opts& opts) { // TODO:
        opts.version_prefix = "-std=gnu++";
    }

    // TODO: できれば，implに，
    std::string default_version_prefix() {
        return "-std=c++";
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
        namespace yaml = io::file::yaml;

        std::vector<std::string> macro_defns;
        // poac automatically define the absolute path of the project's root directory.
        // TODO: これ，依存関係もこれ使ってたら，それも，ルートのにならへん？header-only libの時
        macro_defns.emplace_back(make_macro_defn("POAC_PROJECT_ROOT", fs::current_path().string()));
        const auto version = deper::semver::Version(yaml::get_with_throw<std::string>(node.at("version")));
        macro_defns.emplace_back(make_macro_defn("POAC_VERSION", version.get_full()));
        macro_defns.emplace_back(make_macro_defn("POAC_MAJOR_VERSION", version.major));
        macro_defns.emplace_back(make_macro_defn("POAC_MINOR_VERSION", version.minor));
        macro_defns.emplace_back(make_macro_defn("POAC_PATCH_VERSION", version.patch));
        return macro_defns;
    }

    std::vector<std::string>
    make_include_search_path(const bool exist_deps_key) {
        namespace fs = boost::filesystem;
        namespace lock = deper::lock;
        namespace yaml = io::file::yaml;
        namespace path = io::file::path;

        std::vector<std::string> include_search_path;
        if (exist_deps_key) { // depsキーが存在する // TODO: subcmd/build.hppで，存在確認が取れている
            if (const auto locked_deps = lock::load_ignore_timestamp()) {
                for (const auto& [name, dep] : locked_deps->backtracked) {
                    const std::string current_package_name = naming::to_current(dep.source, name, dep.version);
                    const fs::path include_dir = path::current_deps_dir / current_package_name / "include";

                    if (path::validate_dir(include_dir)) {
                        include_search_path.push_back(include_dir.string());
                    }
                    else {
                        throw exception::error(
                                name + " is not installed.\n"
                                "Please build after running `poac install`");
                    }
                }
            }
            else {
                throw exception::error(
                        "Could not load poac.lock.\n"
                        "Please build after running `poac install`");
            }
        }
        return include_search_path;
    }

    std::vector<std::string>
    make_compile_other_args(const std::map<std::string, YAML::Node>& node) {
        namespace yaml = io::file::yaml;
        if (const auto compile_args = yaml::get<std::vector<std::string>>(node.at("build"), "compile_args")) {
            return *compile_args;
        }
        else {
            return {};
        }
    }
} // end namespace
#endif // POAC_CORE_STROITE_UTILS_CONFIG_HPP

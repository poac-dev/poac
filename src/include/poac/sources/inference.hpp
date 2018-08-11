#ifndef POAC_SOURCES_INFERENCE_HPP
#define POAC_SOURCES_INFERENCE_HPP

#include <iostream>
#include <vector>
#include <string>
#include <tuple>
#include <functional>
#include <map>

#include <boost/optional.hpp>
#include <yaml-cpp/yaml.h>

#include "cache.hpp"
#include "current.hpp"
#include "github.hpp"
#include "tarball.hpp"
#include "../core/except.hpp"
#include "../io.hpp"
#include "../util/command.hpp"
#include "../util/step_functions.hpp"


namespace poac::sources::inference {
    static void info(const std::string& name, const std::string& version) {
        std::cout << name << ": " << version;
    }

    static void _download(const std::string& url, const std::string& pkgname) {
        namespace fs  = boost::filesystem;
        namespace src = sources;

        const std::string pkg_dir = (io::file::path::poac_cache_dir / pkgname).string();
        const std::string tarname = pkg_dir + ".tar.gz";
        io::network::get_file(url, tarname);

        // ~/.poac/cache/package.tar.gz -> ~/.poac/cache/username-repository-tag/...
        io::file::tarball::extract_spec(tarname, pkg_dir);
        fs::remove(tarname);
    }
    // TODO: LICENSEなどが消えてしまう
    static void _cmake_build(
            const std::string& pkgname,
            const std::map<std::string, std::string>& cmake_envs)
    {
        namespace fs     = boost::filesystem;
        namespace except = core::except;

        const fs::path filepath = io::file::path::poac_cache_dir / pkgname;

        util::command cmd("cd " + filepath.string());
        cmd &= "mkdir build";
        cmd &= "cd build";
        util::command cmake_cmd("cmake ..");
        for (const auto& [key, val] : cmake_envs)
            cmake_cmd.env(key, val);
        cmd &= cmake_cmd.std_err();
        cmd &= util::command("make -j4").std_err();
        cmd &= util::command("make install").env("DESTDIR", "./").std_err();

        if (auto result = cmd.run()) {
            const std::string filepath_tmp = filepath.string() + "_tmp";
            fs::rename(filepath, filepath_tmp);
            fs::create_directories(filepath);

            const fs::path build_after_dir(fs::path(filepath_tmp) / "build" / "usr" / "local");

            // Write to cache.yml and recurcive copy
            for (const auto& s : std::vector<std::string>({ "bin", "include", "lib" }))
                if (io::file::path::validate_dir(build_after_dir / s))
                    io::file::path::recursive_copy(build_after_dir / s, fs::path(filepath) / s);
            fs::remove_all(filepath_tmp);
        }
        else { /* error */ }
    }
    static void _manual_build(const std::string& pkgname, const util::command& cmd) {
        namespace fs     = boost::filesystem;
        namespace except = core::except;

        const fs::path filepath = io::file::path::poac_cache_dir / pkgname;


        const std::string filepath_tmp = filepath.string() + "_tmp";
        fs::rename(filepath, filepath_tmp);

        if (auto result = cmd.run()) {
            // TODO: boost build is return 1 always
            fs::remove_all(filepath_tmp);
        }
        else { /* error */ }
    }
    static void _header_only(const std::string& pkgname) {
        namespace fs = boost::filesystem;
        const fs::path filepath = io::file::path::poac_cache_dir / pkgname;

        const std::string filepath_tmp = filepath.string() + "_tmp";

        fs::rename(filepath, filepath_tmp);
        fs::create_directories(filepath);
        io::file::path::recursive_copy(fs::path(filepath_tmp) / "include", fs::path(filepath) / "include");
        fs::remove_all(filepath_tmp);
    }
    static void _copy(const std::string& pkgname) {
        namespace fs = boost::filesystem;

        fs::create_directories(io::file::path::current_deps_dir);
        // Copy package to ./deps
        // If it exists in cache and it is not in the current directory copy it to the current.
        io::file::path::recursive_copy(io::file::path::poac_cache_dir / pkgname, io::file::path::current_deps_dir / pkgname);
    }
    static void _placeholder() {}


    std::tuple<util::step_functions, std::function<void()>, std::string>
    cache(const std::string& name, const std::string& version, const std::string& pkgname) {
        return std::make_tuple(
                util::step_functions(
                        std::bind(&_copy, pkgname)
                ),
                std::bind(&info, name, version),
                "cache"
        );
    }
    std::tuple<util::step_functions, std::function<void()>, std::string>
    notfound(const std::string& name, const std::string& version) {
        return std::make_tuple(
                util::step_functions(
                        std::bind(&_placeholder)
                ),
                std::bind(&info, name, version),
                "notfound"
        );
    }
    std::tuple<util::step_functions, std::function<void()>, std::string>
    cmake(
            const std::string& url,
            const std::string& name,
            const std::string& version,
            const std::string& pkgname,
            const std::map<std::string, std::string>& cmake_envs,
            const std::string& source)
    {
        return std::make_tuple(
                util::step_functions(
                        std::bind(&_download, url, pkgname),
                        std::bind(&_cmake_build, pkgname, cmake_envs),
                        std::bind(&_copy, pkgname)
                ),
                std::bind(&info, name, version),
                source
        );
    }
    std::tuple<util::step_functions, std::function<void()>, std::string>
    manual(
            const std::string& url,
            const std::string& name,
            const std::string& version,
            const std::string& pkgname,
            const util::command& cmd,
            const std::string& source)
    {
        return std::make_tuple(
                util::step_functions(
                        std::bind(&_download, url, pkgname),
                        std::bind(&_manual_build, pkgname, cmd),
                        std::bind(&_copy, pkgname)
                ),
                std::bind(&info, name, version),
                source
        );
    }
    std::tuple<util::step_functions, std::function<void()>, std::string>
    header_only(
            const std::string& url,
            const std::string& name,
            const std::string& version,
            const std::string& pkgname,
            const std::string& source)
    {
        return std::make_tuple(
                util::step_functions(
                        std::bind(&_download, url, pkgname),
                        std::bind(&_header_only, pkgname),
                        std::bind(&_copy, pkgname)
                ),
                std::bind(&info, name, version),
                source
        );
    }

    boost::optional<std::tuple<util::step_functions, std::function<void()>, std::string>>
    resolve(
            const YAML::Node& node,
            const std::string& name,
            const std::string& source)
    {
        namespace except = core::except;

        std::string version;
        std::string pkgname;
        std::string url;
        if (source == "github") {
            version = node["tag"].as<std::string>();
            pkgname = util::package::github_conv_pkgname(name, version);
            url = github::resolve(name, version);
        }
        else if (source == "tarball") {
            version = "nothing";
            pkgname = util::package::basename(name);
            url = node["url"].as<std::string>();
        }
        else {
            throw except::error("poac.yml error\nWhat source is " + source + "?");
        }

        if (current::resolve(pkgname)) {
            return boost::none;
        }
        else if (cache::resolve(pkgname)) {
            return cache(name, version, pkgname);
        }
        else if ((source == "github") ? github::installable(name, version) : true) {
            if (const auto build_system = io::file::yaml::get<std::string>(node, "build"); !build_system) {
                if (const auto build_system2 = io::file::yaml::get2<std::string>(node, "build", "system")) {
                    if (*build_system2 == "cmake") {
                        if (const auto cmake_envs = io::file::yaml::get2<std::map<std::string, std::string>>(node, "build", "environment")) {
                            return cmake(url, name, version, pkgname, *cmake_envs, source);
                        }
                    }
                    else if (*build_system2 == "manual") {
                        if (const auto steps = io::file::yaml::get2<std::vector<std::string>>(node, "build", "steps")) {
                            util::command cmd;
                            int count = 0;
                            for (const auto &s : *steps) {
                                if (count++ == 0) cmd.init(s);
                                else cmd &= s;
                            }
                            return manual(url, name, version, pkgname, cmd, source);
                        }
                    }
                }
            }
            else if (*build_system == "cmake") {
                return cmake(url, name, version, pkgname, std::map<std::string, std::string>(), source);
            } // manual must always describe the step
        }
        return notfound(name, version);
    }

    boost::optional<std::tuple<util::step_functions, std::function<void()>, std::string>>
    resolve_poac(
            [[maybe_unused]] const YAML::Node& node,
            const std::string& name,
            const std::string& version)
    {
        const std::string pkgname = util::package::github_conv_pkgname(name, version);

        if (current::resolve(pkgname)) {
            return boost::none;
        }
        else if (cache::resolve(pkgname)) {
            return cache(name, version, pkgname);
        }
        else {
            return notfound(name, version);
        }
    }
} // end namespace
#endif // !POAC_SOURCES_INFERENCE_HPP

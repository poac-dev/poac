#ifndef POAC_SUBCMD_PUBLISH_HPP
#define POAC_SUBCMD_PUBLISH_HPP

#include <iostream>
#include <string>
#include <vector>

#include <boost/filesystem.hpp>
#include <yaml-cpp/yaml.h>

#include "../io/cli.hpp"
#include "../io/file.hpp"
#include "../core/except.hpp"


namespace poac::subcmd { struct publish {
    static const std::string summary() { return "Beta: Publish a package."; }
    static const std::string options() { return "[<pkg-name>]"; }

    template <typename VS>
    void operator()(VS&& vs) { _main(vs); }
    template <typename VS>
    void _main([[maybe_unused]] VS&& vs) {
        namespace fs     = boost::filesystem;
        namespace except = core::except;

        if (!fs::exists("poac.yml")) {
            throw except::error("ERROR: poac.yml does not exist");
        }
        else if (!fs::exists("src") || !fs::is_directory("src") || fs::is_empty("src")) {
            throw except::error("ERROR: src directory does not exist");
        }
        if (!fs::exists("LICENSE")) {
            std::cerr << io::cli::yellow << "WARN: LICENSE does not exist" << std::endl;
        }
        if (!fs::exists("README.md")) {
            std::cerr << io::cli::yellow << "WARN: README.md does not exist" << std::endl;
        }

        if (YAML::Node config = YAML::LoadFile("poac.yml"); validity_check(config)) {
//            search(config["deps"].as<std::string>())
        // 存在しても，API Keyが大丈夫ならOK!
            std::cout << "name: " << config["name"].as<std::string>() << std::endl;
        }
        else {
            throw except::error("ERROR: poac.yml is invalid");
        }
        // Validate yaml, directory, ...
        // Ignore deps/
        // Compress to tar
        // Post to API
    }

    template <typename C>
    bool validity_check(const C& config) {
        const std::vector<std::string> requires{
                "name", "version", "cpp_version", "compilers",
                "description", "authors", "deps"
        };
        for (const auto& r : requires)
            if (!config[r]) return false;
        return true;
    }
};} // end namespace
#endif // !POAC_SUBCMD_PUBLISH_HPP

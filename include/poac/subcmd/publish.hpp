#ifndef POAC_SUBCMD_PUBLISH_HPP
#define POAC_SUBCMD_PUBLISH_HPP

#include <iostream>
#include <string>
#include <vector>

#include <boost/filesystem.hpp>
#include <yaml-cpp/yaml.h>

#include "../io/cli.hpp"
#include "../io/file.hpp"
#include "../core/exception.hpp"
#include "../util/command.hpp"


namespace poac::subcmd { struct publish {
    static const std::string summary() { return "Beta: Publish a package."; }
    static const std::string options() { return "<Nothing>"; }

    template <typename VS, typename = std::enable_if_t<std::is_rvalue_reference_v<VS&&>>>
    void operator()(VS&& argv) { _main(argv); }
    template <typename VS>
    void _main(VS&& argv) {
        namespace fs     = boost::filesystem;
        namespace except = core::exception;

        check_arguments(argv);
        check_requirements();

        /* To tarball
        const std::string project_dir = fs::absolute(fs::current_path()).string();
        const std::string temp   = *(util::command("mktemp -d").run());
        const std::string temp_path(temp, 0, temp.size()-1); // rm \n
        // TODO:                                               poac.yml -> name-version.tar.gz
        const std::string output_dir  = (fs::path(temp_path) / fs::basename(project_dir)).string() + ".tar.gz";

        io::file::tarball::compress_spec_exclude(project_dir, output_dir, {"deps"});

        std::cout << output_dir << std::endl;
         */

        // Markdown to json. post to API.
        if (const auto res = io::file::markdown::to_json("# hoge\n## hoge2")) {
            std::cout << "Parse: " << *res << std::endl;
        }
        else {
            std::cerr << "Parse failed." << std::endl;
        }

        // poac.yml to json. Post to API.


        // Post tarball to API.


        // Packaging...
        // Add poac.yml
        // Add test/
        // Add ho...
        // Validate token...
        // Please login!!
        // Login succeed
        // Uploading
        // 20% [====>             ]
        //
        // Publish succeed!
        // Please access to https://poac.pm/packages/hoge/0.2.1
    }

    void check_arguments(const std::vector<std::string>& argv) {
        namespace except = core::exception;

        if (!argv.empty())
            throw except::invalid_second_arg("publish");
    }

    void check_requirements() {
        namespace fs     = boost::filesystem;
        namespace except = core::exception;

        if (!fs::exists("poac.yml"))
            throw except::error("poac.yml does not exist");
        else if (!fs::exists("src") || !fs::is_directory("src") || fs::is_empty("src"))
            throw except::error("src directory does not exist");

        if (!fs::exists("LICENSE"))
            std::cerr << io::cli::yellow << "WARN: LICENSE does not exist" << std::endl;
        if (!fs::exists("README.md"))
            std::cerr << io::cli::yellow << "WARN: README.md does not exist" << std::endl;

        if (YAML::Node config = YAML::LoadFile("poac.yml"); validity_check(config)) {
//            std::cout << "name: " << config["name"].as<std::string>() << std::endl;
        }
        else {
            throw except::error("poac.yml is invalid");
        }
    }

    template <typename C>
    bool validity_check(const C& config) {
        const std::vector<std::string> requires{
                "name", "version", "cpp_version", "compilers",
                "description", "owners", "deps"
        };
        for (const auto& r : requires)
            if (!config[r]) return false;
        return true;
    }
};} // end namespace
#endif // !POAC_SUBCMD_PUBLISH_HPP

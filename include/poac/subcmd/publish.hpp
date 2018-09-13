#ifndef POAC_SUBCMD_PUBLISH_HPP
#define POAC_SUBCMD_PUBLISH_HPP

#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <fstream>

#include <boost/filesystem.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <yaml-cpp/yaml.h>

#include "../io.hpp"
#include "../core/exception.hpp"
#include "../util/command.hpp"
#include "../util/argparse.hpp"


namespace poac::subcmd { struct publish {
    static const std::string summary() { return "Beta: Publish a package."; }
    static const std::string options() { return "[-v | --verbose]"; }

    const std::string url = "https://poac-test.herokuapp.com";

    template <typename VS, typename = std::enable_if_t<std::is_rvalue_reference_v<VS&&>>>
    void operator()(VS&& argv) { _main(std::move(argv)); }
    template <typename VS, typename = std::enable_if_t<std::is_rvalue_reference_v<VS&&>>>
    void _main(VS&& argv) {
        namespace fs     = boost::filesystem;
        namespace except = core::exception;

        check_arguments(argv);
        check_requirements();

        const bool verbose = util::argparse::use(argv, "-v", "--verbose");


        const std::string output_dir = compress_project();
        if (verbose) std::cout << output_dir << std::endl;

        const std::string json_string = create_json();
        if (verbose) std::cout << json_string << std::endl;

        // Post json to API. login処理を行う
        const std::string uuid = io::network::post(url + "/create", json_string);
        if (uuid == "err") throw except::error("Could not create package");
        if (verbose) std::cout << uuid << std::endl;

        // Post tarball to API.
        io::network::put_file(url + "/upload?uuid="+uuid, output_dir);

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

    void poac_yml_to_json(boost::property_tree::ptree& pt) {
        namespace except = core::exception;

        if (const auto op_filename = io::file::yaml::exists_setting_file()) {
            const std::string yaml_content = *(io::file::path::read_file(*op_filename));
            std::stringstream json_content;
            json_content << io::network::post(url + "/yaml2json", yaml_content);
            if (json_content.str() == "err")
                throw except::error("Could not create package");
            boost::property_tree::json_parser::read_json(json_content, pt);
        }
        else {
            throw except::error("poac.yml does not exists");
        }
    }

    std::string create_json() {
        namespace except = core::exception;

        boost::property_tree::ptree json;
        if (const auto token = io::file::path::read_file(io::file::path::poac_token_dir))
            json.put("token", *token);
        else
            throw except::error("Could not read token");

        boost::property_tree::ptree setting;
        poac_yml_to_json(setting);
        json.add_child("setting", setting);

        std::stringstream ss;
        boost::property_tree::json_parser::write_json(ss, json);
        return ss.str();
    }

    std::string compress_project() {
        namespace fs     = boost::filesystem;

        const std::string project_dir = fs::absolute(fs::current_path()).string();
        const std::string temp = *(util::command("mktemp -d").exec());
        const std::string temp_path(temp, 0, temp.size()-1); // rm \n
        // TODO:                                               poac.yml -> name-version.tar.gz
        // TODO:                                               project_dir x -> poac.yml o
        const std::string output_dir = (fs::path(temp_path) / fs::basename(project_dir)).string() + ".tar.gz";
        io::file::tarball::compress_spec_exclude(project_dir, output_dir, {"deps"});

        return output_dir;
    }


    void check_arguments(const std::vector<std::string>& argv) {
        namespace except = core::exception;

        if (argv.size() > 1)
            throw except::invalid_second_arg("publish");
    }

    void check_requirements() {
        namespace fs     = boost::filesystem;
        namespace except = core::exception;

        if (!fs::exists("poac.yml"))
            throw except::error("poac.yml does not exist");

        if (!fs::exists("LICENSE"))
            std::cerr << io::cli::yellow << "WARN: LICENSE does not exist" << std::endl;
        if (!fs::exists("README.md"))
            std::cerr << io::cli::yellow << "WARN: README.md does not exist" << std::endl;
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

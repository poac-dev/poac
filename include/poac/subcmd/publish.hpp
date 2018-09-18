#ifndef POAC_SUBCMD_PUBLISH_HPP
#define POAC_SUBCMD_PUBLISH_HPP

#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <fstream>
#include <future>
#include <functional>

#include <boost/filesystem.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <yaml-cpp/yaml.h>

#include "../io.hpp"
#include "../core/exception.hpp"
#include "../util.hpp"


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


        std::cout << std::endl;
        const auto status = std::bind(status_func, "Packaging...", std::placeholders::_1);
        const std::string output_dir = util::async_func<std::string>(compress_project, status);
        status_done("Packaged");
        if (verbose) std::cout << output_dir << std::endl;

        const std::string json_string = create_json();
        if (verbose) std::cout << json_string << std::endl;


        // Post json to API.
        const std::string uuid = io::network::post(url + "/create", json_string);
        // login error
        if (uuid == "err") throw except::error("Could not create package");
        if (verbose) std::cout << uuid << std::endl;

        // Post tarball to API.
//        const std::string res = io::network::post_file(url + "/upload", output_dir, verbose);
//        if (verbose) std::cout << res << std::endl;
        std::cout << std::endl;
        const auto status2 = std::bind(status_func, "Uploading...", std::placeholders::_1);
        const auto post_func = std::bind(io::network::http2post, url + "/upload", uuid, output_dir, verbose);
        util::async_func<void>(post_func, status2);
        status_done("Uploaded");
//        io::network::http2post(url + "/upload", uuid, output_dir, verbose);

        // Delete file
        fs::remove_all(fs::path(output_dir).parent_path());

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
            boost::property_tree::ptree json;
            json.put("yaml", yaml_content);
            std::stringstream ss;
            boost::property_tree::json_parser::write_json(ss, json);

            std::stringstream json_content;
            json_content << io::network::post(url + "/yaml2json", ss.str());
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
        boost::property_tree::json_parser::write_json(ss, json, false);
        return ss.str();
    }

    static std::string compress_project() {
        namespace fs = boost::filesystem;

        const std::string project_dir = fs::absolute(fs::current_path()).string();
        const std::string temp = *(util::command("mktemp -d").exec());
        const std::string temp_path(temp, 0, temp.size()-1); // rm \n
        const std::string output_dir = (fs::path(temp_path) / fs::basename(project_dir)).string() + ".tar.gz";
        const std::vector<std::string> excludes({ "deps", "_build", "build", "cmake-build-debug" });
        io::file::tarball::compress_spec_exclude(project_dir, output_dir, excludes);

        return output_dir;
    }

    void status_done(const std::string& msg) {
        std::cout << io::cli::up(1)
                  << '\b'
                  << io::cli::to_green(" Done.")
                  << "  " + msg
                  << std::endl;
    }
    static void status_func(const std::string& msg, int count) {
        const int size = io::cli::spinners.size();
        std::cout << io::cli::up(1)
                  << '\b'
                  << " " + io::cli::spinners[count%size] + "  " + msg
                  << std::endl;
    }


    void check_arguments(const std::vector<std::string>& argv) {
        namespace except = core::exception;

        if (argv.size() > 1)
            throw except::invalid_second_arg("publish");
    }

    void check_requirements() {
        namespace fs     = boost::filesystem;
        namespace except = core::exception;

        io::file::yaml::load_setting_file("name", "version", "cpp_version",
                                          "compilers", "description", "owners");

        if (!fs::exists("LICENSE"))
            std::cerr << io::cli::yellow << "WARN: LICENSE does not exist" << std::endl;
        if (!fs::exists("README.md"))
            std::cerr << io::cli::yellow << "WARN: README.md does not exist" << std::endl;
    }
};} // end namespace
#endif // !POAC_SUBCMD_PUBLISH_HPP

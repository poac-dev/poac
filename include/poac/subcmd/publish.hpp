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

    const std::string url = "https://poac.pm/api/v1";

    template <typename VS, typename = std::enable_if_t<std::is_rvalue_reference_v<VS&&>>>
    void operator()(VS&& argv) { _main(std::move(argv)); }
    template <typename VS, typename = std::enable_if_t<std::is_rvalue_reference_v<VS&&>>>
    void _main(VS&& argv) {
        namespace fs     = boost::filesystem;
        namespace except = core::exception;

        check_arguments(argv);
        check_requirements();

        const bool verbose = util::argparse::use(argv, "-v", "--verbose");


        const std::string project_dir = fs::absolute(fs::current_path()).string();
        status_func("Packaging " + project_dir + "...");
        const std::string output_dir = compress_project(project_dir);
        if (verbose) std::cout << output_dir << std::endl;

        const std::string json_string = create_json();
        if (verbose) std::cout << json_string << std::endl;

        // Get token
        boost::property_tree::ptree json;
        if (const auto token = io::file::path::read_file(io::file::path::poac_token_dir))
            json.put("token", *token);
        else
            throw except::error("Could not read token");
        std::stringstream ss;
        boost::property_tree::json_parser::write_json(ss, json, false);

        // Validating
        status_func("Validating...");
        if (io::network::post(url + "/packages/validate", ss.str()) == "err")
            throw except::error("Token verification failed");

        // Post tarball to API.
        status_func("Uploading...");
        io::network::post_file(url + "/packages/upload", output_dir, json_string, verbose);

        // Delete file
        status_func("Cleanup...");
        fs::remove_all(fs::path(output_dir).parent_path());

        status_func("Done.");
    }

    std::string create_json() {
        namespace except = core::exception;

        boost::property_tree::ptree json;
        if (const auto token = io::file::path::read_file(io::file::path::poac_token_dir))
            json.put("token", *token);
        else
            throw except::error("Could not read token");

        if (const auto op_filename = io::file::yaml::exists_setting_file()) {
            const std::string yaml_content = *(io::file::path::read_file(*op_filename));
            json.put("setting", yaml_content);
        }
        else {
            throw except::error("poac.yml does not exists");
        }

        std::stringstream ss;
        boost::property_tree::json_parser::write_json(ss, json, false);
        return ss.str();
    }

    std::string compress_project(const std::string& project_dir) {
        namespace fs = boost::filesystem;

        const std::string temp = *(util::command("mktemp -d").exec());
        const std::string temp_path(temp, 0, temp.size()-1); // rm \n
        const std::string output_dir = (fs::path(temp_path) / fs::basename(project_dir)).string() + ".tar.gz";
        const std::vector<std::string> excludes({ "deps", "_build", "build", "cmake-build-debug" });
        io::file::tarball::compress_spec_exclude(project_dir, output_dir, excludes);

        return output_dir;
    }

    void status_func(const std::string& msg) {
        std::cout << io::cli::to_green("==>")
                  << " " + msg
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

#ifndef POAC_SUBCMD_PUBLISH_HPP
#define POAC_SUBCMD_PUBLISH_HPP

#include <iostream>
#include <string>
#include <string_view>
#include <sstream>
#include <vector>
#include <fstream>
#include <future>
#include <functional>
#include <thread>
#include <map>
#include <cstdlib>

#include <boost/filesystem.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <yaml-cpp/yaml.h>

#include "../io.hpp"
#include "../core/exception.hpp"
#include "../util.hpp"
#include "../config.hpp"


namespace poac::subcmd {
    namespace _publish {
        boost::filesystem::path rename_copy(const boost::filesystem::path& project_dir) {
            namespace fs = boost::filesystem;

            const fs::path temp_path = io::file::path::create_temp();
            const fs::path copy_file = temp_path / fs::basename(project_dir);
            io::file::path::recursive_copy(project_dir, copy_file);

            const auto node = io::file::yaml::load_config("name", "version");
            std::string name = node.at("name").as<std::string>();
            std::replace(name.begin(), name.end(), '/', '-'); // boost/config -> boost-config

            const auto filename = name + "-" + node.at("version").as<std::string>();
            const fs::path file_path = temp_path / filename;
            fs::rename(copy_file, file_path);

            return file_path;
        }

        std::string compress_project(const boost::filesystem::path& project_dir) {
            namespace fs = boost::filesystem;

            const fs::path file_path = rename_copy(project_dir);
            const fs::path temp_path = io::file::path::create_temp();

            std::vector<std::string> excludes({ "deps", "_build", ".git", ".gitignore" });
            // Read .gitignore
            if (const auto ignore = io::file::path::read_file(".gitignore")) {
                auto tmp = io::file::path::split(*ignore, "\n");
                const auto itr = std::remove_if(tmp.begin(), tmp.end(), [](std::string x) {
                    return (x[0] == '#');
                });
                excludes.insert(excludes.end(), tmp.begin(), itr-1);
            }
            const std::string output_dir = (temp_path / file_path.filename()).string() + ".tar.gz";

            io::file::tarball::compress_spec_exclude(fs::relative(file_path), output_dir, excludes);

            fs::remove_all(file_path.parent_path());

            return output_dir;
        }

        void check_arguments(const std::vector<std::string>& argv) {
            namespace exception = core::exception;
            if (argv.size() > 1) {
                throw exception::invalid_second_arg("publish");
            }
        }

        void check_requirements() {
            namespace fs = boost::filesystem;

            io::file::yaml::load_config("name", "version", "cpp_version", "description", "owners");

            // TODO: licenseの項があるのに，LICENSEファイルが存在しない => error
            // TODO: licenseの項が無いのに，LICENSEファイルが存在する => error
            if (!fs::exists("LICENSE")) {
                std::cerr << io::cli::to_yellow("WARN: ") << "LICENSE does not exist" << std::endl;
            }
            if (!fs::exists("README.md")) {
                std::cerr << io::cli::to_yellow("WARN: ") << "README.md does not exist" << std::endl;
            }
        }

        template <typename VS>
        int _main(VS&& argv) {
            namespace fs = boost::filesystem;
            namespace exception = core::exception;
            namespace cli = io::cli;

            check_arguments(argv);
            check_requirements();

            const bool yes = util::argparse::use(argv, "-y", "--yes");
            if (!yes) {
                std::cout << "Are you sure publish this package? [Y/n] ";
                std::string yes_or_no;
                std::cin >> yes_or_no;
                std::transform(yes_or_no.begin(), yes_or_no.end(), yes_or_no.begin(), ::tolower);
                if (!(yes_or_no == "yes" || yes_or_no == "y")) {
                    std::cout << "canceled." << std::endl;
                    return EXIT_FAILURE;
                }
            }

            const bool verbose = util::argparse::use(argv, "-v", "--verbose");

            // TODO: poac.ymlに，system: manualが含まれている場合はpublishできない
            // TODO: ヘッダの名前衝突が起きそうな気がしました、#include <package_name/header_name.hpp>だと安心感がある
            // TODO: descriptionに，TODOが含まれてたらエラーではなく，**TODO: Add description**と完全一致ならエラー


            const std::string project_dir = fs::absolute(fs::current_path()).string();
            cli::echo(cli::status, "Packaging ", project_dir, "...");
            const std::string output_dir = compress_project(project_dir);
            if (verbose) std::cout << output_dir << std::endl;

            // Get token
            boost::property_tree::ptree json;
            std::string token;
            if (const auto token_opt = io::file::path::read_file(io::file::path::poac_token_dir)) {
                const std::string temp = *token_opt;
                const std::string temp_path(temp, 0, temp.size()-1); // delete \n
                token = temp_path;
                json.put("token", token);
            }
            else {
                throw exception::error("Could not read token");
            }
            {
                const auto node = io::file::yaml::load_config("owners");
                boost::property_tree::ptree children;
                for (const auto& s : node.at("owners").as<std::vector<std::string>>()) {
                    boost::property_tree::ptree child;
                    child.put("", s);
                    children.push_back(std::make_pair("", child));
                }
                json.add_child("owners", children);
            }
            std::string json_s;
            {
                std::stringstream ss;
                boost::property_tree::json_parser::write_json(ss, json, false);
                json_s = ss.str();
            }

            // Validating
            cli::echo(cli::to_status("Validating..."));
            if (verbose) {
                std::cout << json_s << std::endl;
            }
            if (io::network::post(POAC_TOKENS_VALIDATE_API, json_s) == "err") {
                throw exception::error("Token verification failed.\n"
                                    "Please check the following check lists.\n"
                                    "1. Does token really belong to you?\n"
                                    "2. Is the user ID described `owners` in poac.yml\n"
                                    "    the same as that of GitHub account?");
            }

            const auto node = io::file::yaml::load_config("name", "version");
            const auto node_name = node.at("name").as<std::string>();
            const auto node_version = node.at("version").as<std::string>();
            if (io::network::get(POAC_PACKAGES_API + node_name + "/" + node_version + "/exists") == "true") {
                throw exception::error(node_name + ": " + node_version + " already exists");
            }

            // Post tarball to API.
            cli::echo(cli::to_status("Uploading..."));
            if (!fs::exists("poac.yml")) {
                throw exception::error("poac.yml does not exists");
            }
            if (const auto res = io::network::post_file(token, output_dir); res != "ok") {
                throw exception::error(res); // TODO: Check exists packageは飛ばして，Delete fileはしてほしい
            }

            // Check exists package
            io::network::Headers headers;
            headers.emplace(io::network::http::field::cache_control, "no-cache");
            const std::string target = POAC_PACKAGES_API + node_name + "/" + node_version + "/exists";
            const std::string res = io::network::get(target, POAC_API_HOST, headers);
            if (res != "true") {
                std::cerr << io::cli::to_red("ERROR: ") << "Could not create package." << std::endl;
            }

            // Delete file
            cli::echo(cli::to_status("Cleanup..."));
            fs::remove_all(fs::path(output_dir).parent_path());

            cli::echo(cli::to_status("Done."));
            return EXIT_SUCCESS;
        }
    }

    struct publish {
        static std::string summary() { return "Publish a package"; }
        static std::string options() { return "[-v | --verbose, -y | --yes]"; }
        template<typename VS>
        int operator()(VS&& argv) {
            return _publish::_main(std::forward<VS>(argv));
        }
    };
} // end namespace
#endif // !POAC_SUBCMD_PUBLISH_HPP

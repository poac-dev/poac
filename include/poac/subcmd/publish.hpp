#ifndef POAC_SUBCMD_PUBLISH_HPP
#define POAC_SUBCMD_PUBLISH_HPP

#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <fstream>
#include <future>
#include <functional>
#include <thread>
#include <map>

#include <boost/filesystem.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <yaml-cpp/yaml.h>

#include "../io.hpp"
#include "../core/exception.hpp"
#include "../util.hpp"


namespace poac::subcmd {
    namespace _publish {
        std::string read_config() {
            namespace except = core::exception;
            if (const auto op_filename = io::file::yaml::exists_config()) {
                return *(io::file::path::read_file(*op_filename));
            }
            else {
                throw except::error("poac.yml does not exists");
            }
        }

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

        void status_func(const std::string& msg) {
            std::cout << io::cli::to_green("==> ")
                      << msg
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

            io::file::yaml::load_config("name", "version", "cpp_version",
                                        "description", "owners");

            // TODO: licenseの項があるのに，LICENSEファイルが存在しない => error
            // TODO: licenseの項が無いのに，LICENSEファイルが存在する => error
            if (!fs::exists("LICENSE"))
                std::cerr << io::cli::yellow << "WARN: LICENSE does not exist" << std::endl;
            if (!fs::exists("README.md"))
                std::cerr << io::cli::yellow << "WARN: README.md does not exist" << std::endl;
        }

        template <typename VS, typename = std::enable_if_t<std::is_rvalue_reference_v<VS&&>>>
        void _main(VS&& argv) {
            namespace fs     = boost::filesystem;
            namespace except = core::exception;

            check_arguments(argv);
            check_requirements();

            const bool verbose = util::argparse::use(argv, "-v", "--verbose");
//            bool error = false;

            // TODO: poac.ymlに，system: manualが含まれている場合はpublishできない
            // TODO: ヘッダの名前衝突が起きそうな気がしました、#include <package_name/header_name.hpp>だと安心感がある
            // TODO: descriptionに，TODOが含まれてたらエラーではなく，**TODO: Add description**と完全一致ならエラー
            // TODO: その依存関係が解決できるものかチェック


            const std::string project_dir = fs::absolute(fs::current_path()).string();
            status_func("Packaging " + project_dir + "...");
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
                throw except::error("Could not read token");
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
            std::stringstream ss;
            boost::property_tree::json_parser::write_json(ss, json, false);

            // Validating
//            if (!error)
            status_func("Validating...");
            if (verbose) std::cout << ss.str() << std::endl;
            if (io::network::post(POAC_API_TOKENS_VALIDATE, ss.str()) == "err")
                throw except::error("Token verification failed.\n"
                                    "Please check the following check lists.\n"
                                    "1. Does token really belong to you?\n"
                                    "2. Is the user ID described `owners` in poac.yml\n"
                                    "    the same as that of GitHub account?");
            const auto node = io::file::yaml::load_config("name", "version");
            const auto node_name = node.at("name").as<std::string>();

            // TODO: subcmd::newに同じものが存在する．
            const auto is_slash = [](const char c) {
                return c == '/';
            };
            // /name
            if (is_slash(node_name[0])) {
                throw except::error("Invalid name.\n"
                                    "It is prohibited to add /(slash)\n"
                                    " at the begenning of a project name.");
            }
            // org/name/sub
            else if (std::count_if(node_name.begin(), node_name.end(), is_slash) > 1) {
                throw except::error("Invalid name.\n"
                                    "It is prohibited to use two\n"
                                    " /(slashes) in a project name.");
            }

            const auto node_version = node.at("version").as<std::string>();
            if (io::network::get(POAC_API_PACKAGES + node_name + "/" + node_version + "/exists") == "true")
                throw except::error(node_name + ": " + node_version + " already exists");

            // Post tarball to API.
            status_func("Uploading...");
            const std::string config = read_config();
            if (verbose) std::cout << config << std::endl;
            // could not get response
            io::network::post_file(POAC_API_PACKAGE_UPLOAD, output_dir, config, token, verbose);

            // Check exists package
            std::map<std::string, std::string> headers;
            headers.insert(std::make_pair("Cache-Control", "no-cache"));
            const std::string res = io::network::get(POAC_API_PACKAGES + node_name + "/" + node_version + "/exists", headers);
            if (res != "true")
                std::cerr << io::cli::to_red("ERROR: ") << "Could not create package." << std::endl;

            // Delete file
            status_func("Cleanup...");
            fs::remove_all(fs::path(output_dir).parent_path());

            status_func("Done.");
        }
    }

    struct publish {
        static const std::string summary() { return "Publish a package."; }
        static const std::string options() { return "[-v | --verbose]"; }
        template<typename VS, typename = std::enable_if_t<std::is_rvalue_reference_v<VS&&>>>
        void operator()(VS&& argv) { _publish::_main(std::move(argv)); }
    };
} // end namespace
#endif // !POAC_SUBCMD_PUBLISH_HPP

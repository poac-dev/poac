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
#include "../core/except.hpp"
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

            io::file::tar::compress_spec_exclude(fs::relative(file_path), output_dir, excludes);

            fs::remove_all(file_path.parent_path());

            return output_dir;
        }

        void check_arguments(const std::vector<std::string>& argv) {
            namespace except = core::except;
            if (argv.size() > 1) {
                throw except::invalid_second_arg("publish");
            }
        }

        void check_requirements() {
            namespace fs = boost::filesystem;
            io::file::yaml::load_config("name", "version", "cpp_version", "description", "owners");
            if (!fs::exists("README.md")) {
                // TODO: もう少しほんわかと識別したい．README.txtやreadme.md等
                // TODO: readmeが接頭辞にありつつ，最短なファイル．README.md, README-ja.mdだと，README.mdを優先
                std::cerr << io::cli::to_yellow("WARN: ") << "README.md does not exist" << std::endl;
            }
        }

        template <typename VS>
        int _main(VS&& argv) {
            namespace fs = boost::filesystem;
            namespace except = core::except;
            namespace cli = io::cli;
            using namespace std::string_literals;

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
                throw except::error(except::msg::could_not_read("token"));
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
            {
                const io::net::requests req{};
                const auto res = req.post(POAC_TOKENS_VALIDATE_API, json_s);
                if (res.data() != "ok"s) {
                    throw except::error(res.data());
                }
            }

            const auto node = io::file::yaml::load_config("name", "version");
            const auto node_name = node.at("name").as<std::string>();
            const auto node_version = node.at("version").as<std::string>();
            {
                const io::net::requests req{};
                const auto res = req.get(POAC_EXISTS_API + "/"s + node_name + "/" + node_version);
                if (res.data() == "true"s) {
                    throw except::error(
                            except::msg::already_exist(node_name + ": " + node_version));
                }
            }

            // Post tarball to API.
            cli::echo(cli::to_status("Uploading..."));
            if (!fs::exists("poac.yml")) {
                throw except::error(
                        except::msg::does_not_exist("poac.yml"));
            }
            {
                io::net::multiPartForm mp_form;
                mp_form.set("token", token);
                std::map<io::net::http::field, std::string> h;
                h[io::net::http::field::content_type] = "application/x-gzip";
                h[io::net::http::field::content_transfer_encoding] = "binary";
                mp_form.set("file", output_dir, h);

                const io::net::requests req{};
                if (const auto res = req.post(POAC_UPLOAD_API, std::move(mp_form)); res != "ok") {
                    std::cerr << io::cli::to_red("ERROR: ") << res << std::endl;
                }
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

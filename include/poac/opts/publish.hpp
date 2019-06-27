#ifndef POAC_OPTS_PUBLISH_HPP
#define POAC_OPTS_PUBLISH_HPP

#include <cstdlib>
#include <cstdint>
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
#include <optional>

#include <boost/filesystem.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <yaml-cpp/yaml.h>

#include "../core/resolver/semver.hpp"
#include "../core/except.hpp"
#include "../io/cli.hpp"
#include "../io/net.hpp"
#include "../config.hpp"
#include "../util/termcolor2.hpp"
#include "../util/pretty.hpp"

namespace poac::opts::publish {
    constexpr auto summary = termcolor2::make_string("Publish a package");
    constexpr auto options = termcolor2::make_string("[-v | --verbose, -y | --yes]");

//    struct PackageInfo {
//        std::string name;
//        core::resolver::semver::Version version;
//        std::optional<std::string> description;
//        std::uint16_t cpp_version;
//        std::optional<std::string> license;
//        std::string package_type;
//    };
//
//    int
//    do_register() {
//
//    }
//
//    template <typename VS>
//    int
//    confirm(const VS& argv) {
//        const bool yes = util::argparse::use(argv, "-y", "--yes");
//        if (!yes) {
//            std::cout << "Are you sure publish this package? [Y/n] ";
//            if (!io::cli::yes_or_no()) {
//                std::cout << "canceled." << std::endl;
//                return EXIT_FAILURE;
//            }
//        }
//        return EXIT_SUCCESS;
//    }
//
//    void
//    summarize(const PackageInfo& package_info) {
//        namespace pretty = util::pretty;
//        std::cout << "Summary:"
//                  << "\n  Name: " << package_info.name
//                  << "\n  Version: " << package_info.version.get_version() // TODO: GET /repos/:owner/:repo/releases/latest tag_name -> SemVerに即している必要がある．
//                  << "\n  Description: " << pretty::clip_string(package_info.description.value_or("null"), 50) // TODO: GET /repos/:owner/:repo, なければ，nullとなる．-> 長い場合，prettyする？50くらい
//                  << "\n  C++ Version (minimum required version): " << package_info.cpp_version
//                  << "\n  License: " << package_info.license.value_or("null") // TODO: /repos/poacpm/poac/license\?ref\=0.2.1 -> license -> name
//                  << "\n  Package Type: " << package_info.package_type
//                  << "\n\n" << std::endl;
//    }
//
//    std::string
//    get_package_type() {
//
//    }
//
//    std::optional<std::string>
//    get_license(const std::string& full_name) {
//
//    }
//
//    std::uint16_t
//    get_cpp_version() {
//        return io::yaml::load_config("cpp_version").as<std::uint16_t>();
//    }
//
//    std::optional<std::string>
//    get_description(const std::string& full_name) {
//
//    }
//
//    core::resolver::semver::Version
//    get_version(const std::string& full_name) {
//        // https://developer.github.com/v3/repos/releases/#get-the-latest-release
//        const io::net::requests req{ GITHUB_API_HOST };
//        const auto res = req.get(GITHUB_REPOS_API + full_name + "/releases/latest");
//
//        std::stringstream ss;
//        ss << res.data();
//
//        boost::property_tree::ptree pt;
//        boost::property_tree::json_parser::read_json(ss, pt);
//    }
//
//    std::string_view
//    extract_full_name(std::string_view repository) {
//        auto first = repository.find("https://github.com/");
//        auto last = repository.find(".git", first);
//        return repository.substr(first, last - first);
//    }
//
//    std::string
//    get_name() {
//        const std::string repository = io::yaml::load_config("repository").as<std::string>();
//        std::string_view full_name = extract_full_name(repository);
//        return std::string(full_name);
//    }
//
//    PackageInfo
//    gather_package_info() {
//        const std::string full_name = get_name();
//        return PackageInfo{
//                full_name,
//                get_version(full_name),
//                get_description(full_name),
//                get_cpp_version(),
//                get_license(full_name),
//                get_package_type()
//        };
//    }
//
//    PackageInfo
//    report_publish_start() { // TODO: optionalで返さなくてOK????
//        std::cout << "Verifying your package ...\n" << std::endl;
//        const PackageInfo package_info = gather_package_info();
//        summarize(package_info);
//        return package_info;
//    }

    std::optional<core::except::Error>
    check_arguments(const std::vector<std::string>& argv) noexcept {
        namespace except = core::except;
        if (!argv.empty()) {
            return except::Error::InvalidSecondArg::Publish;
        }
        return std::nullopt;
    }

    std::optional<core::except::Error>
    _main(const std::vector<std::string>& argv) {
        namespace fs = boost::filesystem;
        namespace except = core::except;
        namespace cli = io::cli;
        using namespace std::string_literals;
        using termcolor2::color_literals::operator""_red;

        if (const auto result = check_arguments(argv)) {
            return result;
        }

//        const auto maybeKnownVersions = Registry.getVersions(pkg, registry);
//        if (report_publish_start() != EXIT_SUCCESS) {
//            return EXIT_FAILURE;
//        }
////
////        verifyVersion(env, pkg, version, maybeKnownVersions);
////        git = getGit
////        commitHash = verifyTag(git, manager, pkg, version);
////        verifyNoChanges(git, commitHash, version);
////        zipHash = verifyZip(env, pkg, version);
////
////        Task.io $ putStrLn "";
//        if (confirm(argv) != EXIT_SUCCESS) {
//            return EXIT_FAILURE;
//        }
//        do_register(manager, pkg, version, docs, commitHash, zipHash);
////        Task.io $ putStrLn "Success!";



        const bool verbose = util::argparse::use(argv, "-v", "--verbose");

        // Get token
        boost::property_tree::ptree json;
        std::string token;
        {
            boost::property_tree::ptree children;
            for (const auto& s : io::yaml::load_config("owners").as<std::vector<std::string>>()) {
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
        std::cout << cli::status << "Validating..." << std::endl;
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

        const auto node = io::yaml::load_config("name", "version");
        const auto node_name = node.at("name").as<std::string>();
        const auto node_version = node.at("version").as<std::string>();
        {
            const io::net::requests req{};
            const auto res = req.get(POAC_EXISTS_API + "/"s + node_name + "/" + node_version);
            if (res.data() == "true"s) {
                return except::Error::General{
                        except::msg::already_exist(node_name + ": " + node_version)
                };
            }
        }

        // Post tarball to API.
        std::cout << cli::status << "Uploading..." << std::endl;
        if (!fs::exists("poac.yml")) {
            return except::Error::General{
                    except::msg::does_not_exist("poac.yml")
            };
        }
        {
            io::net::multiPartForm mp_form;
            mp_form.set("token", token);
            std::map<io::net::http::field, std::string> h;
            h[io::net::http::field::content_type] = "application/x-gzip";
            h[io::net::http::field::content_transfer_encoding] = "binary";
//            mp_form.set("file", output_dir, h);

            const io::net::requests req{};
            if (const auto res = req.post(POAC_UPLOAD_API, std::move(mp_form)); res != "ok") {
                std::cerr << "ERROR: "_red << res << std::endl;
            }
        }

        // Delete file
        std::cout << cli::status << "Cleanup..." << std::endl;
//        fs::remove_all(fs::path(output_dir).parent_path());

        std::cout << cli::status << "Done." << std::endl;
        return std::nullopt;
    }
} // end namespace
#endif // !POAC_OPTS_PUBLISH_HPP

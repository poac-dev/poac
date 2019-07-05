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
#include <map>
#include <optional>

#include <boost/filesystem.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <yaml-cpp/yaml.h>

#include <poac/core/except.hpp>
#include <poac/io/net.hpp>
#include <poac/io/path.hpp>
#include <poac/io/term.hpp>
#include <poac/io/yaml.hpp>
#include <poac/util/argparse.hpp>
#include <poac/util/pretty.hpp>
#include <poac/util/semver.hpp>
#include <poac/util/shell.hpp>
#include <poac/util/termcolor2.hpp>
#include <poac/config.hpp>

namespace poac::opts::publish {
    constexpr auto summary = termcolor2::make_string("Publish a package");
    constexpr auto options = termcolor2::make_string("[-v | --verbose, -y | --yes]");

    struct PackageInfo {
        std::string name;
        semver::Version version;
        std::optional<std::string> description;
        std::uint16_t cpp_version;
        std::optional<std::string> license;
        std::string package_type;
    };

//    int do_register() {
//
//    }

    std::optional<core::except::Error>
    confirm(const std::vector<std::string>& argv) {
        const bool yes = util::argparse::use(argv, "-y", "--yes");
        if (!yes) {
            std::cout << "Are you sure publish this package? [Y/n] ";
            if (!io::term::yes_or_no()) {
                return core::except::Error::InterruptedByUser;
            }
        }
        return std::nullopt;
    }

    void summarize(const PackageInfo& package_info) {
        using termcolor2::color_literals::operator""_bold;
        namespace pretty = util::pretty;
        std::cout << "Summary:"_bold
                  << "\n  Name: "_bold << package_info.name
                  << "\n  Version: "_bold << package_info.version.get_version()
                  << "\n  Description: "_bold << pretty::clip_string(package_info.description.value_or("null"), 50)
                  << "\n  C++ Version (minimum required version): "_bold << package_info.cpp_version
                  << "\n  License: "_bold << package_info.license.value_or("null")
                  << "\n  Package Type: "_bold << package_info.package_type
                  << "\n" << std::endl;
    }

    std::string get_package_type() {
        // poac.ymlに，
        // buildキーがなければ，header-only library
        // 例外：buildキーがあり，libとbinキーがなければ，エラー
        // buildキーがあり，libキーがありtrue，binキーがない，もしくは，falseなれば，build-required library
        // buildキーがあり，binキーがあれば，application -> 現状は，applicationはpublishできない
        const auto node = io::yaml::load_config();
        if (io::yaml::get(node, "build", "bin")) {
            return "application";
        }
        else if (io::yaml::get(node, "build", "lib")) {
            return "build-required library";
        }
        else { // TODO: buildキーはあるのに，binとlibが無くて，header-onlyとなってしまう問題がある
            return "header-only library";
        }
    }

    std::optional<std::string>
    get_license(const std::string& full_name, const std::string& version) {
        // https://developer.github.com/v3/licenses/#get-the-contents-of-a-repositorys-license
        const io::net::requests req{ GITHUB_API_HOST };
        std::string_view target = GITHUB_REPOS_API + ("/" + full_name) + "/license?ref=" + version;
#ifdef POAC_USE_GITHUB_TOKEN_FROM_ENV
        io::net::Headers headers;
        headers.emplace(io::net::http::field::authorization, "token " + io::path::dupenv("POAC_GITHUB_API_TOKEN").value());
        const auto res = req.get(target, headers);
#else
        const auto res = req.get(target);
#endif

        std::stringstream ss;
        ss << res.data();

        boost::property_tree::ptree pt;
        boost::property_tree::json_parser::read_json(ss, pt);
        if (const auto license = pt.get_optional<std::string>("license.name")) {
            if (license.get() == "null") {
                return std::nullopt;
            }
            return license.get();
        }
        return std::nullopt;
    }

    std::uint16_t get_cpp_version() {
        return io::yaml::load_config("cpp_version").as<std::uint16_t>();
    }

    std::optional<std::string>
    get_description(const std::string& full_name) {
        // https://developer.github.com/v3/repos/#get
        const io::net::requests req{ GITHUB_API_HOST };
        std::string_view target = GITHUB_REPOS_API + ("/" + full_name);
#ifdef POAC_USE_GITHUB_TOKEN_FROM_ENV
        io::net::Headers headers;
        headers.emplace(io::net::http::field::authorization, "token " + io::path::dupenv("POAC_GITHUB_API_TOKEN").value());
        const auto res = req.get(target);
#else
        const auto res = req.get(target);
#endif

        std::stringstream ss;
        ss << res.data();

        boost::property_tree::ptree pt;
        boost::property_tree::json_parser::read_json(ss, pt);
        if (const auto description = pt.get_optional<std::string>("description")) {
            if (description.get() == "null") {
                return std::nullopt;
            }
            return description.get();
        }
        return std::nullopt;
    }

    semver::Version
    get_version(const std::string& full_name) {
        // https://developer.github.com/v3/repos/releases/#get-the-latest-release
        const io::net::requests req{ GITHUB_API_HOST };
        std::string_view target = GITHUB_REPOS_API + ("/" + full_name) + "/releases/latest";
#ifdef POAC_USE_GITHUB_TOKEN_FROM_ENV
        io::net::Headers headers;
        headers.emplace(io::net::http::field::authorization, "token " + io::path::dupenv("POAC_GITHUB_API_TOKEN").value());
        const auto res = req.get(target);
#else
        const auto res = req.get(target);
#endif

        std::stringstream ss;
        ss << res.data();

        boost::property_tree::ptree pt;
        boost::property_tree::json_parser::read_json(ss, pt);
        if (const auto version = pt.get_optional<std::string>("tag_name")) {
            // If version do not obey SemVer, error.
            return semver::Version{ version.get() };
        }
        throw core::except::error(
                "Could not find latest release.\n"
                "Please execute the following commands:\n"
                "  git tag 0.1.0\n"
                "  git push origin 0.1.0");
    }

    std::optional<std::string_view>
    extract_str(std::string_view target, std::string_view prefix, std::string_view suffix) {
        auto first = target.find(prefix);
        if (first == std::string_view::npos) {
            return std::nullopt;
        }
        else {
            first += prefix.size();
        }
        auto last = target.find(suffix, first);
        return target.substr(first, last - first);
    }

    std::string_view extract_full_name(std::string_view repository) {
        if (const auto sub = extract_str(repository, "https://github.com/", ".git")) {
            return sub.value();
        }
        else {
            if (const auto sub2 = extract_str(repository, "git@github.com:", ".git")) {
                return sub2.value();
            }
            throw core::except::error(
                    "Invalid repository name");
        }
    }

    std::string get_name() {
        if (const auto repository = util::shell("git config --get remote.origin.url").exec()) {
            return std::string(extract_full_name(repository.value()));
        }
        throw core::except::error(
                "Could not find origin url.\n"
                "Please execute the following command:\n"
                "  git remote add origin https://github.com/:owner/:repo.git");
    }

    PackageInfo gather_package_info() {
        const std::string full_name = get_name();
        const semver::Version version = get_version(full_name);

        return PackageInfo{
                full_name,
                version,
                get_description(full_name),
                get_cpp_version(),
                get_license(full_name, version.get_version()),
                get_package_type()
        };
    }

    PackageInfo report_publish_start() {
        std::cout << "Verifying your package ...\n" << std::endl;
        const PackageInfo package_info = gather_package_info();
        summarize(package_info);
        return package_info;
    }

    std::optional<core::except::Error>
    check_arguments(const std::vector<std::string>& argv) noexcept {
        namespace except = core::except;
        if (!argv.empty()) {
            return except::Error::InvalidSecondArg::Publish;
        }
        return std::nullopt;
    }

    std::optional<core::except::Error>
    exec(const std::vector<std::string>& argv) {
        namespace fs = boost::filesystem;
        namespace except = core::except;
        using namespace std::string_literals;

        if (const auto result = check_arguments(argv)) {
            return result;
        }

        const auto package_info = report_publish_start();

        // if(is_known_version(package_info.version)) {
        //     return except::Error::General{"hoge is already exists"}
        // };

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

        if (const auto result = confirm(argv)) {
            return result;
        }
        throw except::error("hoge");



//        const bool verbose = util::argparse::use(argv, "-v", "--verbose");

//        const auto node = io::yaml::load_config("name", "version");
//        const auto node_name = node.at("name").as<std::string>();
//        const auto node_version = node.at("version").as<std::string>();
//        {
//            const io::net::requests req{};
//            const auto res = req.get(POAC_EXISTS_API + "/"s + node_name + "/" + node_version);
//            if (res.data() == "true"s) {
//                return except::Error::General{
//                        except::msg::already_exist(node_name + ": " + node_version)
//                };
//            }
//        }
//
//        // Post tarball to API.
//        std::cout << io::term::status << "Uploading..." << std::endl;
//        if (!fs::exists("poac.yml")) {
//            return except::Error::DoesNotExist{
//                    "poac.yml"
//            };
////            return except::Error::General{
////                    except::msg::does_not_exist("poac.yml")
////            };
//        }
//        {
//            io::net::multiPartForm mp_form;
////            mp_form.set("token", token);
//            std::map<io::net::http::field, std::string> h;
//            h[io::net::http::field::content_type] = "application/x-gzip";
//            h[io::net::http::field::content_transfer_encoding] = "binary";
////            mp_form.set("file", output_dir, h);
//
//            const io::net::requests req{};
//            if (const auto res = req.post(POAC_UPLOAD_API, std::move(mp_form)); res != "ok") {
//                std::cerr << io::term::error << res << std::endl;
//            }
//        }
//
//        // Delete file
//        std::cout << io::term::status << "Cleanup..." << std::endl;
////        fs::remove_all(fs::path(output_dir).parent_path());
//
//        std::cout << io::term::status << "Done." << std::endl;
        return std::nullopt;
    }
} // end namespace
#endif // !POAC_OPTS_PUBLISH_HPP

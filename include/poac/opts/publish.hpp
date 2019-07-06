#ifndef POAC_OPTS_PUBLISH_HPP
#define POAC_OPTS_PUBLISH_HPP

#include <cstdlib>
#include <cstdint>
#include <iostream>
#include <string>
#include <string_view>
#include <sstream>
#include <vector>
#include <optional>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

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
    constexpr auto options = termcolor2::make_string("[-v, --verbose | -y, --yes]");

    struct Options {
        bool verbose;
        bool yes;
    };

    enum class PackageType {
        HeaderOnlyLib,
        BuildReqLib,
        Application
    };

    std::string
    to_string(PackageType package_type) noexcept {
        switch (package_type) {
            case PackageType::HeaderOnlyLib:
                return "header-only library";
            case PackageType::BuildReqLib:
                return "build-required library";
            case PackageType::Application:
                return "application";
        }
    }

    struct PackageInfo {
        std::string name;
        semver::Version version;
        std::optional<std::string> description;
        std::uint16_t cpp_version;
        std::optional<std::string> license;
        PackageType package_type;
    };

    std::optional<core::except::Error>
    do_register(const PackageInfo& package_info) {
        std::cout << "Registering " << package_info.name << ": "
                  << package_info.version.get_full() << " ..." << std::endl;

        return std::nullopt;
        // TODO: POST
    }

    std::optional<core::except::Error>
    verify_no_changes(const PackageInfo& package_info) {
        // https://stackoverflow.com/questions/3878624/how-do-i-programmatically-determine-if-there-are-uncommited-changes
        const std::string cmd = "git diff-index --quiet " + package_info.version.get_full() + " --";
        if (!util::shell(cmd).exec_ignore()) {
            return core::except::Error::General{
                "Changes from ", package_info.version.get_full(), " was detected.\n"
                "Use the following command, depending on the situation:\n"
                "    git stash\n"
                "    git checkout ", package_info.version.get_full()
            };
        }
        return std::nullopt;
    }

    std::optional<core::except::Error>
    verify_version(const PackageInfo& package_info) {
        if (io::net::api::exists(package_info.name, package_info.version.get_full())) {
            return core::except::Error::General{
                package_info.name, ": ", package_info.version.get_full(), " is already exists."
            };
        }
        return std::nullopt;
    }

    std::optional<core::except::Error>
    verify_commit_sha(const PackageInfo& package_info, std::string_view remote_commit_sha) {
        // https://stackoverflow.com/questions/1862423/how-to-tell-which-commit-a-tag-points-to-in-git
        // Check if the local git tag commit sha and obtained commit sha match.
        const std::string cmd = "git rev-list -n 1 " + package_info.version.get_full();
        if (const auto local_commit_hash = util::shell(cmd).exec()) {
            if (remote_commit_sha == local_commit_hash->substr(0, local_commit_hash->size() - 1)) { // Delete \n
                return std::nullopt;
            }
        }
        return core::except::Error::General{
            "GitHub release commit sha does not match local tag commit sha.\n"
            "Make sure that the same tag indicates the same commit.\n"
            "Perhaps, with the same tag name, someone else had rewrite B\n"
            "  by executing the command as shown below in the past, which may not be synchronized locally:\n"
            "    git tag -d ", package_info.version.get_full(), "\n"
            "    git push origin :", package_info.version.get_full(), "\n"
            "    git commit --amend  # commit sha has changed! (https://stackoverflow.com/q/23791999)\n"
            "    git tag ", package_info.version.get_full(), "\n"
            "    git push origin ", package_info.version.get_full(), "\n"
            "In that case, may solve it by the following command:\n"
            "    git fetch origin ", package_info.version.get_full()
        };
    }

    std::optional<core::except::Error>
    verify_tag(const PackageInfo& package_info) noexcept {
        // https://developer.github.com/v3/repos/releases/#get-a-release-by-tag-name
        // Get commit sha using obtained tag.
        // https://stackoverflow.com/questions/28496319/github-a-tag-but-not-a-release
        // Note: GitHub regards tags and releases as the same thing, and the method to get tags is also legal.
        try {
            const std::string target = "/" + package_info.name + "/git/refs/tags/" + package_info.version.get_full();
            const auto pt = io::net::api::github::repos(target);
            if (const auto commit_sha = pt.get_optional<std::string>("object.sha")) {
                if (const auto result = verify_commit_sha(package_info, commit_sha.value())) {
                    return result;
                } else {
                    return std::nullopt;
                }
            } else {
                throw core::except::error{""};
            }
        }
        catch (...) {
            return core::except::Error::General{
                    "Could not find a current tag release.\n"
                    "Please execute the following command:\n"
                    "    git push origin ", package_info.version.get_full()
            };
        }
    }

    std::optional<core::except::Error>
    verify_package(const PackageInfo& package_info) {
        if (const auto result = verify_tag(package_info)) {
            return result;
        }
        if (const auto result = verify_version(package_info)) {
            return result;
        }
//        if (const auto result = verify_no_changes(package_info)) {
//            return result;
//        }
        return std::nullopt;
    }

    std::optional<core::except::Error>
    confirm(const publish::Options& opts) {
        if (!opts.yes && !io::term::yes_or_no("Are you sure publish this package?")) {
            return core::except::Error::InterruptedByUser;
        }
        return std::nullopt;
    }

    void summarize(const PackageInfo& package_info) {
        using termcolor2::color_literals::operator""_bold;
        using util::pretty::clip_string;
        std::cout << "Summary:"_bold
                  << "\n  Name: "_bold << package_info.name
                  << "\n  Version: "_bold << package_info.version.get_full()
                  << "\n  Description: "_bold << clip_string(package_info.description.value_or("null"), 50)
                  << "\n  C++ Version (minimum required version): "_bold << package_info.cpp_version
                  << "\n  License: "_bold << package_info.license.value_or("null")
                  << "\n  Package Type: "_bold << to_string(package_info.package_type)
                  << "\n" << std::endl;
    }

    PackageType get_package_type() {
        const auto node = io::yaml::load_config();
        if (io::yaml::get(node, "build", "bin")) {
            // bin: true
            return PackageType::Application;
        } else if (io::yaml::get(node, "build", "lib")) {
            // lib: true
            return PackageType::BuildReqLib;
        } else if (io::yaml::contains(node, "build")) {
            return PackageType::BuildReqLib;
        } else {
            return PackageType::HeaderOnlyLib;
        }
    }

    std::optional<std::string>
    get_license(const std::string& full_name, const std::string& version) {
        // https://developer.github.com/v3/licenses/#get-the-contents-of-a-repositorys-license
        const auto pt = io::net::api::github::repos("/" + full_name + "/license?ref=" + version);
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
        const auto pt = io::net::api::github::repos("/" + full_name);
        if (const auto description = pt.get_optional<std::string>("description")) {
            if (description.get() == "null") {
                return std::nullopt;
            }
            return description.get();
        }
        return std::nullopt;
    }

    semver::Version
    get_version() {
        // https://stackoverflow.com/questions/3404936/show-which-git-tag-you-are-on
        // Get current tag from workspace git information.
        if (const auto tag = util::shell("git describe --tags --abbrev=0").exec()) {
            return semver::Version{ tag->substr(0, tag->size() - 1) }; // Delte \n
        }
        else {
            throw core::except::error(
                    "Could not get a current tag.\n"
                    "Please execute the following command:\n"
                    "    git tag 0.1.0");
        }
    }

    std::optional<std::string_view>
    extract_str(std::string_view target, std::string_view prefix, std::string_view suffix) {
        auto first = target.find(prefix);
        if (first == std::string_view::npos) {
            return std::nullopt;
        } else {
            first += prefix.size();
        }
        auto last = target.find(suffix, first);
        return target.substr(first, last - first);
    }

    std::string_view extract_full_name(std::string_view repository) {
        if (const auto sub = extract_str(repository, "https://github.com/", ".git")) {
            return sub.value();
        } else {
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
                "    git remote add origin https://github.com/:owner/:repo.git");
    }

    PackageInfo gather_package_info() {
        const std::string full_name = get_name();
        const semver::Version version = get_version();

        return PackageInfo {
                full_name,
                version,
                get_description(full_name),
                get_cpp_version(),
                get_license(full_name, version.get_full()),
                get_package_type()
        };
    }

    PackageInfo report_publish_start() {
        std::cout << "Verifying your package ...\n" << std::endl;
        return gather_package_info();
    }

    std::optional<core::except::Error>
    publish(const publish::Options& opts) {
        const auto package_info = report_publish_start();

        // TODO: Currently, we can not publish an application.
        if (package_info.package_type == PackageType::Application) {
            return core::except::Error::General{
                "Sorry, we can not publish an application currently."
            };
        }
        if (const auto result = verify_package(package_info)) {
            return result;
        }

        summarize(package_info);

        if (const auto result = confirm(opts)) {
            return result;
        }
        if (const auto result = do_register(package_info)) {
            return result;
        }

        std::cout << "\n" << io::term::status << "Done." << std::endl;
        return std::nullopt;
    }

    std::optional<core::except::Error>
    exec(const std::vector<std::string>& args) {
        publish::Options opts{};
        opts.verbose = util::argparse::use(args, "-v", "--verbose");
        opts.yes = util::argparse::use(args, "-y", "--yes");
        return publish::publish(opts);
    }
} // end namespace
#endif // !POAC_OPTS_PUBLISH_HPP

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
#include <utility>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include <poac/core/except.hpp>
#include <poac/io/net.hpp>
#include <poac/io/path.hpp>
#include <poac/io/term.hpp>
#include <poac/io/config.hpp>
#include <poac/util/argparse.hpp>
#include <poac/util/pretty.hpp>
#include <poac/util/semver.hpp>
#include <poac/util/shell.hpp>
#include <poac/util/termcolor2.hpp>
#include <poac/config.hpp>

namespace poac::opts::publish {
    constexpr auto summary = termcolor2::make_string("Publish a package");
    constexpr auto options = termcolor2::make_string("[-y, --yes]");

    struct Options {
        bool yes;
    };

    struct PackageInfo {
        std::string owner;
        std::string repo;
        semver::Version version;
        std::optional<std::string> description;
        std::uint16_t cpp_version;
        std::optional<std::string> license;
        io::config::PackageType package_type;
        std::string local_commit_sha;
    };

    std::string
    create_full_name(const std::string& owner, const std::string& repo) {
        return owner + "/" + repo;
    }

    std::string
    get_full_name(const PackageInfo& package_info) {
        return create_full_name(package_info.owner, package_info.repo);
    }

    [[nodiscard]] std::optional<core::except::Error>
    do_register(const PackageInfo& package_info) {
        std::cout << "\nRegistering `" << get_full_name(package_info) << ": "
                  << package_info.version.get_full() << "` ..." << std::endl;

        io::net::MultiPartForm mpf{};
        mpf.set("owner", package_info.owner);
        mpf.set("repo", package_info.repo);
        mpf.set("version", package_info.version.get_full());
        mpf.set("description", package_info.description.value_or("null"));
        mpf.set("cpp_version", std::to_string(package_info.cpp_version));
        mpf.set("package_type", to_string(package_info.package_type));
        mpf.set("commit_sha", package_info.local_commit_sha);

        io::net::MultiPartForm::header_type header;
        header[io::net::http::field::content_type] = "text/plain";
        mpf.set("poac.yml", "poac.yml", header);

        const io::net::requests req{ POAC_API_HOST };
        if (const auto res = req.post(POAC_REGISTER_API, std::move(mpf)); res != "ok") {
            return core::except::Error::General{ res };
        }
        return std::nullopt;
    }

    [[nodiscard]] std::optional<core::except::Error>
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

    [[nodiscard]] std::optional<core::except::Error>
    verify_exists(const PackageInfo& package_info) {
        if (io::net::api::exists(get_full_name(package_info), package_info.version.get_full())) {
            return core::except::Error::General{
                get_full_name(package_info), ": ", package_info.version.get_full(), " is already registered."
            };
        }
        return std::nullopt;
    }

    [[nodiscard]] std::optional<core::except::Error>
    verify_commit_sha(const PackageInfo& package_info, std::string_view remote_commit_sha) {
        // Check if the local git tag commit sha and obtained commit sha match.
        if (remote_commit_sha == package_info.local_commit_sha) {
            return std::nullopt;
        }
        return core::except::Error::General{
            "GitHub release commit sha does not match local tag commit sha.\n"
            "Make sure that the same tag indicates the same commit.\n"
            "Perhaps, with the same tag name, someone else had rewrite commit sha\n"
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

    [[nodiscard]] std::optional<core::except::Error>
    verify_tag(const PackageInfo& package_info) noexcept {
        // https://developer.github.com/v3/repos/releases/#get-a-release-by-tag-name
        // Get commit sha using obtained tag.
        // https://stackoverflow.com/questions/28496319/github-a-tag-but-not-a-release
        // Note: GitHub regards tags and releases as the same thing, and the method to get tags is also legal.
        const std::string target = "/" + get_full_name(package_info) + "/git/refs/tags/" + package_info.version.get_full();
        try {
            const auto pt = io::net::api::github::repos(target);
            if (const auto commit_sha = pt.get_optional<std::string>("object.sha")) {
                if (const auto error = verify_commit_sha(package_info, commit_sha.value())) {
                    return error;
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

    [[nodiscard]] std::optional<core::except::Error>
    verify_cpp_version(const std::uint16_t& cpp_version) {
        switch (cpp_version) {
            case 98:
                [[fallthrough]];
            case 3:
                [[fallthrough]];
            case 11: // Include TR1
                [[fallthrough]];
            case 14:
                [[fallthrough]];
            case 17:
                [[fallthrough]];
            case 20:
                return std::nullopt;
            default:
                return core::except::Error::General{
                    "Invalid C++ version ", cpp_version, "\n"
                    "Please specify one of the following versions:\n"
                    "    98, 3, 11, 14, 17, 20"
                };
        }
    }

    [[nodiscard]] std::optional<core::except::Error>
    verify_package(const PackageInfo& package_info) {
        if (const auto error = verify_cpp_version(package_info.cpp_version)) {
            return error;
        }
        if (const auto error = verify_tag(package_info)) {
            return error;
        }
        if (const auto error = verify_exists(package_info)) {
            return error;
        }
        if (const auto error = verify_no_changes(package_info)) {
            return error;
        }
        return std::nullopt;
    }

    [[nodiscard]] std::optional<core::except::Error>
    confirm(const publish::Options& opts) {
        if (!opts.yes) {
            if (const auto error = io::term::yes_or_no("Are you sure publish this package?")) {
                return error;
            }
        }
        return std::nullopt;
    }

    void
    summarize(const PackageInfo& package_info) {
        using termcolor2::color_literals::operator""_bold;
        using termcolor2::color_literals::operator""_yellow;
        using util::pretty::clip_string;
        std::cout << "Summary:"_bold
                  << "\n  Name: "_bold << get_full_name(package_info)
                  << "\n  Version: "_bold << package_info.version.get_full()
                  << "\n  Description: "_bold << clip_string(package_info.description.value_or("null"), 50)
                  << "\n  C++ Version (minimum required version): "_bold << package_info.cpp_version
                  << "\n  License: "_bold << package_info.license.value_or("null"_yellow)
                  << "\n  Package Type: "_bold << to_string(package_info.package_type)
                  << "\n" << std::endl;
    }

    std::string
    get_local_commit_sha(const std::string& version) {
        // https://stackoverflow.com/questions/1862423/how-to-tell-which-commit-a-tag-points-to-in-git
        const std::string cmd = "git rev-list -n 1 " + version;
        if (const auto local_commit_sha = util::shell(cmd).exec()) {
            return local_commit_sha->substr(0, local_commit_sha->size() - 1); // Delete \n
        } else {
            throw core::except::error(
                    "Could not get current tag commit sha.\n"
                    "Is tag ", version, " exists?");
        }
    }

    io::config::PackageType
    get_package_type(const std::optional<io::config::Config>& config) {
        if (config->build.has_value()) {
            if (config->build->bin.has_value() && config->build->bin.value()) {
                // bin: true
                return io::config::PackageType::Application;
            } else if (config->build->lib.has_value() && config->build->lib.value()) {
                // lib: true
                return io::config::PackageType::BuildReqLib;
            } else {
                return io::config::PackageType::BuildReqLib;
            }
        } else {
            return io::config::PackageType::HeaderOnlyLib;
        }
    }

    std::optional<std::string>
    get_license(const std::string& full_name, const std::string& version) {
        // https://developer.github.com/v3/licenses/#get-the-contents-of-a-repositorys-license
        try {
            const auto pt = io::net::api::github::repos("/" + full_name + "/license?ref=" + version);
            if (const auto license = pt.get_optional<std::string>("license.name")) {
                if (license.get() == "null") {
                    return std::nullopt;
                }
                return license.get();
            }
            return std::nullopt;
        }
        catch (...) {
            return std::nullopt;
        }
    }

    std::uint16_t
    get_cpp_version(const std::optional<io::config::Config>& config) {
        return config->cpp_version.value();
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

    std::string_view
    extract_full_name(std::string_view repository) {
        if (const auto sub = extract_str(repository, "https://github.com/", ".git")) {
            return sub.value();
        } else {
            if (const auto sub2 = extract_str(repository, "git@github.com:", ".git")) {
                return sub2.value();
            }
            throw core::except::error(
                    "Could not extract repository name.\n"
                    "Is the URL that can be acquired by the following command the URL of GitHub?:\n"
                    "    git config --get remote.origin.url\n"
                    "If not, please execute the following command:\n"
                    "    git remote add origin https://github.com/:owner/:repo.git\n"
                    "Note: Currently, it can only publish on GitHub.\n"
                    "      This condition may change in the future.");
        }
    }

    std::pair<std::string, std::string>
    get_name() {
        if (const auto repository = util::shell("git config --get remote.origin.url").exec()) {
            std::string_view full_name = extract_full_name(repository.value());
            auto found = full_name.find('/');
            return { std::string(full_name.substr(0, found)), std::string(full_name.substr(found + 1)) };
        }
        throw core::except::error(
                "Could not find origin url.\n"
                "Please execute the following command:\n"
                "    git remote add origin https://github.com/:owner/:repo.git");
    }

    PackageInfo
    gather_package_info(const std::optional<io::config::Config>& config) {
        const auto [owner, repo] = get_name();
        const semver::Version version = get_version();

        return PackageInfo {
                owner,
                repo,
                version,
                get_description(create_full_name(owner, repo)),
                get_cpp_version(config),
                get_license(create_full_name(owner, repo), version.get_full()),
                get_package_type(config),
                get_local_commit_sha(version.get_full()),
        };
    }

    PackageInfo
    report_publish_start(const std::optional<io::config::Config>& config) {
        std::cout << "Verifying your package ...\n" << std::endl;
        return gather_package_info(config);
    }

    [[nodiscard]] std::optional<core::except::Error>
    publish(std::optional<io::config::Config>&& config, publish::Options&& opts) {
        const auto package_info = report_publish_start(config);

        // TODO: Currently, we can not publish an application.
        if (package_info.package_type == io::config::PackageType::Application) {
            summarize(package_info);
            return core::except::Error::General{
                "Sorry, you can not publish an application currently.\n"
                "This condition may change in the future."
            };
        }
        if (const auto error = verify_package(package_info)) {
            return error;
        }

        summarize(package_info);

        if (const auto error = confirm(opts)) {
            return error;
        }
        if (const auto error = do_register(package_info)) {
            return error;
        }

        std::cout << io::term::status << "Done." << std::endl;
        return std::nullopt;
    }

    [[nodiscard]] std::optional<core::except::Error>
    exec(std::optional<io::config::Config>&& config, std::vector<std::string>&& args) {
        publish::Options opts{};
        opts.yes = util::argparse::use(args, "-y", "--yes");
        return publish::publish(std::move(config), std::move(opts));
    }
} // end namespace
#endif // !POAC_OPTS_PUBLISH_HPP

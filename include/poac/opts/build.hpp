#ifndef POAC_OPTS_BUILD_HPP
#define POAC_OPTS_BUILD_HPP

#include <iostream>
#include <string>
#include <map>
#include <optional>

#include <boost/filesystem.hpp>

#include <poac/core/builder.hpp>
#include <poac/core/except.hpp>
#include <poac/core/name.hpp>
#include <poac/io/term.hpp>
#include <poac/io/path.hpp>
#include <poac/io/config.hpp>
#include <poac/util/argparse.hpp>
#include <poac/util/termcolor2.hpp>

namespace poac::opts::build {
    constexpr auto summary = termcolor2::make_string("Compile a project and all sources that depend on its");
    constexpr auto options = termcolor2::make_string("[-v | --verbose]");

    struct Options {
        bool verbose;
    };

    semver::Version
    get_version() {
        // https://stackoverflow.com/questions/3404936/show-which-git-tag-you-are-on
        // Get current tag from workspace git information.
        if (const auto tag = util::shell("git describe --tags --abbrev=0").dump_stderr().exec()) {
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

    [[nodiscard]] std::optional<core::except::Error>
    build(std::optional<io::config::Config>&& config, build::Options&& opts) {
        const auto [repo, name] = get_name();
        const int base_size = 12;
        std::cout << std::right << std::setw(base_size + termcolor2::green<>.size() + termcolor2::reset<>.size())
                  << termcolor2::to_green("Compiling ") << name << " v" << get_version() << std::endl;

        // if (const auto error = core::resolver::install_deps()) {
        //    return error;
        // }
        core::Builder bs(opts.verbose, boost::filesystem::current_path());
        // if (const auto error = bs.build()) {
        //    return error;
        // }
//        bs.build();

        // TODO: install package to ~/.poac/src/
        // TODO: header-only -> -I~/.poac/src/${package}/include
        // TODO: build-required -> cp ~/.poac/src/${package} target/debug/deps/ -> build

        return std::nullopt;
    }

    [[nodiscard]] std::optional<core::except::Error>
    exec(std::optional<io::config::Config>&& config, std::vector<std::string>&& args) {
        if (args.size() > 1) {
            return core::except::Error::InvalidSecondArg::Build;
        }
        build::Options opts{};
        opts.verbose = util::argparse::use(args, "-v", "--verbose");
        return build::build(std::move(config), std::move(opts));
    }
} // end namespace
#endif // !POAC_OPTS_BUILD_HPP

#ifndef POAC_CORE_PROJECT_HPP
#define POAC_CORE_PROJECT_HPP

#include <string>
#include <string_view>
#include <utility>

#include <poac/core/except.hpp>
#include <poac/util/git2-cpp/git2.hpp>
#include <poac/util/semver.hpp>

namespace poac::core::project {
    semver::Version
    version() {
        // https://stackoverflow.com/questions/3404936/show-which-git-tag-you-are-on
        // Get current tag from workspace git information.
        try {
            git2::describe desc;
            desc.workdir(git2::repository().open("."), git2::describe_options().describe_tags());
            return semver::Version{ desc.format(git2::describe_format_options().abbreviated_size(0)) };
        } catch (...) {
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
                    "This condition may change in the future.");
        }
    }

    std::pair<std::string, std::string>
    name() {
        std::string repository;
        try {
            repository = git2::repository().open(".").config().get_string("remote.origin.url");
        } catch (...) {
            throw core::except::error(
                    "Could not find origin url.\n"
                    "Please execute the following command:\n"
                    "    git remote add origin https://github.com/:owner/:repo.git");
        }
        std::string_view full_name = extract_full_name(repository);
        auto found = full_name.find('/');
        return { std::string(full_name.substr(0, found)), std::string(full_name.substr(found + 1)) };
    }
} // end namespace
#endif // !POAC_CORE_PROJECT_HPP

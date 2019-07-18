// Idempotent package name library
#ifndef POAC_CORE_NAME_HPP
#define POAC_CORE_NAME_HPP

#include <string>
#include <string_view>
#include <regex>
#include <utility>

#include <yaml-cpp/yaml.h>

#include <poac/core/except.hpp>
#include <poac/io/yaml.hpp>

namespace poac::core::name {
    // -- poac --
    // Cache package name
    //   * opencv-3.4.2
    //   * boost-config-1.66.0
    // Current package name
    //   * opencv
    //   * boost-config

    // username/repository -> repository
    std::string basename(const std::string& name) {
        if (name.find('/') != std::string::npos) {
            return std::string(name, name.find('/') + 1);
        } else {
            return name;
        }
    }

    // boost/bind -> boost-bind
    std::string slash_to_hyphen(std::string name) {
        std::replace(name.begin(), name.end(), '/', '-');
        return name;
    }

    // boost-bind -> boost/bind
    std::string hyphen_to_slash(std::string name) {
        std::replace(name.begin(), name.end(), '-', '/');
        return name;
    }

    // 1. opencv/opencv, 3.4.2 -> opencv-3.4.2
    // 2. curl/curl, 7_61_0 -> curl-7.61.0
    // 3. matken11235/logger_slack_backend, v0.1.0 -> logger_slack_backend-0.1.0
    // 4. curl/curl, curl-7_61_0 -> curl-7_61_0

    // GitHub
    std::string to_cache_github(const std::string& name, const std::string& tag) {
        std::regex pattern("^(v?)?(?:(\\d+)(\\.|_))?(?:(\\d+)(\\.|_))?(\\*|\\d+)$");
        std::smatch matches;

        // 1. boost -> boost
        // 2. matken11235/logger_slack_backend -> logger_slack_backend
        // ...
        std::string prefix = basename(name);

        // 1. Group1: ``,  Group2: `3`, Group3: `.`, Group4: `4`,  Group5: `.`, Group6: `2`
        // 2. Group1: ``,  Group2: `7`, Group3: `_`, Group4: `61`, Group5: `_`, Group6: `0`
        // 3. Group1: `v`, Group2: `0`, Group3: `.`, Group4: `1`,  Group5: `.`, Group6: `0`
        if (std::regex_match(tag, matches, pattern)) {
            return prefix + "-" + matches[2].str() + "." + matches[4].str() + "." + matches[6].str();
        }
        // 4
        else {
            return tag;
        }
    }
    // poac
    std::string to_cache_poac(const std::string& name, const std::string& ver) {
        const auto name2 = slash_to_hyphen(name);
        return name2 + "-" + ver;
    }

    // opencv-3.4.2 -> opencv
    std::string cache_to_current(const std::string& name) {
        if (const auto pos = name.find_last_of('-'); pos != std::string::npos) {
            return std::string(name, 0, pos);
        } else {
            return name;
        }
    }

    // like `poac-0.3.0-beta`
    std::string to_cache(
            const std::string& src,
            const std::string& name,
            const std::string& version
    ) {
        if (src == "poac") {
            return to_cache_poac(name, version);
        } else if (src == "github") {
            return to_cache_github(name, version);
        } else {
            throw except::error(except::msg::unknown_source(src));
        }
    }
    // boost/bind -> boost-bind
    std::string to_current(
            const std::string& src,
            const std::string& name,
            const std::string& version
    ) {
        if (src == "poac") {
            return slash_to_hyphen(name);
        } else if (src == "github") {
            const auto cache_name = to_cache_github(name, version);
            return cache_to_current(cache_name);
        } else {
            throw except::error(except::msg::unknown_source(src));
        }
    }

    std::string get_version(const YAML::Node& node, const std::string& src) {
        if (src == "github") {
            if (const auto ver = io::yaml::get<std::string>(node)) {
                return ver.value();
            } else if (const auto ver2 = io::yaml::get<std::string>(node, "tag")) {
                return ver2.value();
            }
        }
        else if (src == "poac") {
            if (const auto ver = io::yaml::get<std::string>(node)) {
                return ver.value();
            } else if (const auto ver2 = io::yaml::get<std::string>(node, "version")) {
                return ver2.value();
            }
        }
        throw except::error(except::msg::unknown_source(src));
    }

    // github/curl/curl -> src = github, name = curl/curl
    // boost/config -> src = poac, name = boost/config
    std::pair<std::string, std::string>
    get_source(const std::string& name) {
        std::regex exp("^github\\/(.*)");
        std::smatch match_group;
        if (std::regex_match(name, match_group, exp)) {
            return std::make_pair("github", match_group[1].str());
        } else {
            return std::make_pair("poac", name);
        }
    }

    constexpr bool
    is_digit(const char& c) noexcept {
        return '0' <= c && c <= '9';
    }

    constexpr bool
    is_alphabet(const char& c) noexcept {
        return ('A' <= c && c <= 'Z')
            || ('a' <= c && c <= 'z');
    }

    constexpr bool
    is_alpha_numeric(const char& c) noexcept {
        return is_digit(c) || is_alphabet(c);
    }

    [[nodiscard]] std::optional<core::except::Error>
    validate_package_name(std::string_view s) {
        for (const auto& c : s) {
            if (!is_alpha_numeric(c) && c != '_' && c != '-' && c != '/') {
                return except::Error::General{
                        "Invalid name.\n"
                        "It is prohibited to use a character string\n"
                        " that does not match ^([a-z|\\d|_|\\-|\\/]*)$\n"
                        " in the project name."
                };
            }
        }
        return std::nullopt;
    }
} // end namespace
#endif // !POAC_CORE_NAME_HPP

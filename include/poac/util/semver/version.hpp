#ifndef SEMVER_VERSION_HPP
#define SEMVER_VERSION_HPP

#include <cstdint>
#include <string>
#include <sstream>
#include <vector>
#include <regex>
#include <algorithm>

#include <poac/util/semver/exception.hpp>

namespace semver {
    // The following Regular Expressions can be used for tokenizing,
    // validating, and parsing SemVer version strings.
    // A regular expression before binding is https://github.com/semver/semver/issues/232#issue-48635632

    // ## Numeric Identifier
    // A single `0`, or a non-zero digit followed by zero or more digits.
    const std::string NUMERIC_IDENTIFIER =
            R"(0|[1-9]\d*)";

    // ## Non-numeric Identifier
    // Zero or more digits, followed by a letter or hyphen, and then zero or
    // more letters, digits, or hyphens.
    const std::string NON_NUMERIC_IDENTIFIER =
            R"(\d*[a-zA-Z-][0-9a-zA-Z-]*)";

    // ## Main Version
    // Three dot-separated numeric identifiers.
    const std::string MAIN_VERSION =
            "(" + NUMERIC_IDENTIFIER + R"()\.)" +
            "(" + NUMERIC_IDENTIFIER + R"()\.)" +
            "(" + NUMERIC_IDENTIFIER + ")";

    // ## Pre-release Version Identifier
    // A numeric identifier, or a non-numeric identifier.
    const std::string PRE_RELEASE_IDENTIFIER =
            "(?:" + NUMERIC_IDENTIFIER +
            "|" + NON_NUMERIC_IDENTIFIER + ")";

    // ## Pre-release Version
    // Hyphen, followed by one or more dot-separated pre-release version
    // identifiers.
    const std::string PRE_RELEASE =
            "(?:-(" + PRE_RELEASE_IDENTIFIER +
            R"((?:\.)" + PRE_RELEASE_IDENTIFIER + ")*))";

    // ## Build Metadata Identifier
    // Any combination of digits, letters, or hyphens.
    const std::string BUILD_IDENTIFIER =
            "[0-9A-Za-z-]+";

    // ## Build Metadata
    // Plus sign, followed by one or more period-separated build metadata
    // identifiers.
    const std::string BUILD =
            R"((?:\+()" + BUILD_IDENTIFIER +
            R"((?:\.)" + BUILD_IDENTIFIER + ")*))";

    // ## Full Version String
    // A main version, followed optionally by a pre-release version and
    // build metadata.

    // Note that the only major, minor, patch, and pre-release sections of
    // the version string are capturing groups.  The build metadata is not a
    // capturing group, because it should not ever be used in version
    // comparison.
    const std::string FULL_PLAIN =
            "v?" + MAIN_VERSION +
            PRE_RELEASE + "?" +
            BUILD + "?";
    const std::string FULL =
            "^" + FULL_PLAIN + "$";

    // Coercion.
    // Extract anything that could conceivably be a part of a valid semver
    const std::string MAX_SAFE_COMPONENT_LENGTH = "16";
    const std::string COERCE =
            std::string(R"((?:^|[^\d]))") +
            R"((\d{1,)" + MAX_SAFE_COMPONENT_LENGTH + "})" +
            R"((?:\.(\d{1,)" + MAX_SAFE_COMPONENT_LENGTH + "}))?" +
            R"((?:\.(\d{1,)" + MAX_SAFE_COMPONENT_LENGTH + "}))?" +
            R"((?:$|[^\d]))";
    const std::string FULL_COERCE =
            "v?" + COERCE +
            PRE_RELEASE + "?" +
            BUILD + "?";

    // ## Interval of Version String
    // Something like ">1.2.0 and <=2.0.0".
    // A simple gt/lt/eq thing, or just "" to indicate "any version"
    const std::string GT_LT =
            "((?:<|>)?=?)";
    const std::string CLOSED_UNBOUNDED_INTERVAL =
            "(^" + GT_LT + "?" + FULL_PLAIN + "$)";
    const std::string BOUNDED_INTERVAL =
            "(^" + GT_LT + FULL_PLAIN + "( and )" + GT_LT + FULL_PLAIN + "$)";

    struct Version {
        // The major version, to be incremented on incompatible changes.
        std::uint_fast64_t major = 0;
        // The minor version, to be incremented when functionality is added in a
        // backwards-compatible manner.
        std::uint_fast64_t minor = 0;
        // The patch version, to be incremented when backwards-compatible bug
        // fixes are made.
        std::uint_fast64_t patch = 0;
        // The pre-release version identifier, if one exists.
        std::vector<std::string> pre = {};
        // The build metadata, ignored when determining version precedence.
        std::vector<std::string> build = {};

        explicit Version(const std::string& version) {
            std::smatch match;
            if (std::regex_match(version, match, std::regex(FULL))) {
                apply_version(match);
            }
            else if (std::regex_match(version, match, std::regex(COERCE))) {
                apply_version(match);
            }
            else {
                throw version_error("Invalid version");
            }
        }

        std::string get_version() const {
            std::string version = std::to_string(major);
            version += "." + std::to_string(minor);
            version += "." + std::to_string(patch);
            if (!pre.empty()) {
                version += "-";
                for (const auto& s : pre) {
                    version += s + ".";
                }
                version.pop_back();
            }
            return version;
        }

        std::string get_full() const {
            std::string full = get_version();
            if (!build.empty()) {
                full += "+";
                for (const auto& s : build) {
                    full += s + ".";
                }
                full.pop_back();
            }
            return full;
        }

    private:
        std::vector<std::string>
        split(const std::string& input, char delimiter) {
            std::istringstream stream(input);

            std::string field;
            std::vector<std::string> result;
            while (std::getline(stream, field, delimiter)) {
                result.push_back(field);
            }
            return result;
        }

        void apply_version(const std::smatch& sm) {
            const auto is_matched = [](const auto& m){ return m.matched; };
            const auto count = std::count_if(sm.begin(), sm.end(), is_matched);

            // X.Y.Z-B1.B2+P1.P2.P3
            switch (count) {
                case 6: // build meta data ([P1, P2, P3])
                    build = split(sm[5].str(), '.');
                    [[fallthrough]];
                case 5: // pre-release version ([B1, B2])
                    pre = split(sm[4].str(), '.');
                    [[fallthrough]];
                case 4: // patch version (Z)
                    patch = std::stoull(sm[3].str());
                    [[fallthrough]];
                case 3: // minor version (Y)
                    minor = std::stoull(sm[2].str());
                    [[fallthrough]];
                case 2: // major version (X)
                    major = std::stoull(sm[1].str());
                    [[fallthrough]];
                default:
                    break;
            }
        }
    };
} // end namespace semver

#endif // !SEMVER_VERSION_HPP

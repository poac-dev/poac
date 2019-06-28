// semver(https://semver.org) for poac
// minimalistic and dedicated to poac
// (A grammar not used by poac (or etc.) is not implemented)
// Check if the package exists within the specified range
#ifndef POAC_CORE_RESOLVER_SEMVER_HPP
#define POAC_CORE_RESOLVER_SEMVER_HPP

#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <regex>
#include <cstdint>
#include <cctype>
#include <algorithm>

#include "../core/except.hpp"
#include "../io/cli.hpp"
#include "termcolor2.hpp"

namespace poac::core::resolver::semver {
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
        std::uint64_t major = 0;
        // The minor version, to be incremented when functionality is added in a
        // backwards-compatible manner.
        std::uint64_t minor = 0;
        // The patch version, to be incremented when backwards-compatible bug
        // fixes are made.
        std::uint64_t patch = 0;
        // The pre-release version identifier, if one exists.
        std::vector<std::string> pre = {};
        // The build metadata, ignored when determining version precedence.
        std::vector<std::string> build = {};

        explicit Version(const std::string& version) {
            std::smatch match;
            if (std::regex_match(version, match, std::regex(FULL))) {
                apply_version(match);
                io::cli::debugln(*this);
            }
            else if (std::regex_match(version, match, std::regex(COERCE))) {
                apply_version(match);
                io::cli::debugln(*this);
            }
            else {
                throw except::error("Invalid version");
            }
        }
        Version(const char* version)
            : Version(std::string(version))
        {}

        template <typename CharT, std::size_t N>
        Version(const CharT(&version)[N])
                : Version(std::string(version, N))
        {}

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

    std::ostream& operator<<(std::ostream& os, const Version& v) {
        os << "SemVer {\n";
        os << "  full: " << termcolor2::green<> << "\"" << v.get_full() << "\"" << termcolor2::reset<> << ",\n";
        os << "  version: " << termcolor2::green<> << "\"" << v.get_version() << "\"" << termcolor2::reset<> << ",\n";
        os << "  major: " << termcolor2::yellow<> << v.major << termcolor2::reset<> << ",\n";
        os << "  minor: " << termcolor2::yellow<> << v.minor << termcolor2::reset<> << ",\n";
        os << "  patch: " << termcolor2::yellow<> << v.patch << termcolor2::reset<> << ",\n";
        os << "  prerelease: " << "[ ";
        for (const auto& s : v.pre) {
            os << termcolor2::green<> << "\"" << s << "\"" << termcolor2::reset<> << ", ";
        }
        os << "],\n";
        os << "  build: " << "[ ";
        for (const auto& s : v.build) {
            os << termcolor2::green<> << "\"" << s << "\"" << termcolor2::reset<> << ", ";
        }
        os << "],\n";
        os << "}";
        return os;
    }

    bool is_number(const std::string& s)
    {
        // https://stackoverflow.com/a/15039928
        int (*isdigit)(int) = std::isdigit;
        return !s.empty() && std::find_if(s.begin(), s.end(),
                [&](unsigned char c) { return !isdigit(c); }) == s.end();
    }

    bool gt_pre(const Version& lhs, const Version& rhs) {
        // NOT having a prerelease is > having one
        if (lhs.pre.empty() && !rhs.pre.empty()) { // gt
            return true;
        }

        const int lhs_pre_size = static_cast<int>(lhs.pre.size());
        const int rhs_pre_size = static_cast<int>(rhs.pre.size());
        const int max_size = std::max(lhs_pre_size, rhs_pre_size);
        for (int i = 0; i <= max_size; ++i) {
            if (i >= lhs_pre_size && i >= rhs_pre_size) {
                return false; // eq
            }
            else if (i >= lhs_pre_size) {
                return false; // lt
            }
            else if (i >= rhs_pre_size) {
                return true; // gt
            }
            else if (lhs.pre[i] == rhs.pre[i]) {
                continue;
            }
            else if (!is_number(lhs.pre[i]) && is_number(rhs.pre[i])) {
                return true;
            }
            else if (lhs.pre[i] > rhs.pre[i]) {
                return true;
            }
            else if (is_number(lhs.pre[i]) && is_number(rhs.pre[i])) {
                if (std::stoull(lhs.pre[i]) > std::stoull(rhs.pre[i])) {
                    return true;
                }
            }
            else {
                return false;
            }
        }
        return false;
    }
    bool lt_pre(const Version& lhs, const Version& rhs) {
        if (!lhs.pre.empty() && rhs.pre.empty()) { // lt
            return true;
        }

        const int lhs_pre_size = static_cast<int>(lhs.pre.size());
        const int rhs_pre_size = static_cast<int>(rhs.pre.size());
        const int max_size = std::max(lhs_pre_size, rhs_pre_size);
        for (int i = 0; i <= max_size; ++i) {
            if (i >= lhs_pre_size && i >= rhs_pre_size) {
                return false; // eq
            }
            else if (i >= lhs_pre_size) {
                return true; // lt
            }
            else if (i >= rhs_pre_size) {
                return false; // gt
            }
            else if (lhs.pre[i] == rhs.pre[i]) {
                continue;
            }
            else if (is_number(lhs.pre[i]) && !is_number(rhs.pre[i])) {
                return true;
            }
            else if (lhs.pre[i] < rhs.pre[i]) {
                return true;
            }
            else if (is_number(lhs.pre[i]) && is_number(rhs.pre[i])) {
                if (std::stoull(lhs.pre[i]) < std::stoull(rhs.pre[i])) {
                    return true;
                }
            }
            else {
                return false;
            }
        }
        return false;
    }
    bool eq_pre(const Version& lhs, const Version& rhs) {
        if (lhs.pre.empty() && rhs.pre.empty()) { // eq
            return true;
        }

        const int lhs_pre_size = static_cast<int>(lhs.pre.size());
        const int rhs_pre_size = static_cast<int>(rhs.pre.size());
        const int max_size = std::max(lhs_pre_size, rhs_pre_size);
        for (int i = 0; i <= max_size; ++i) {
            if (i >= lhs_pre_size && i >= rhs_pre_size) {
                return true; // eq
            }
            else if (i >= rhs_pre_size) {
                return false; // gt
            }
            else if (i >= lhs_pre_size) {
                return false; // lt
            }
            else if (lhs.pre[i] == rhs.pre[i]) {
                continue;
            }
            else {
                return false;
            }
        }
        return true;
    }


    bool operator>(const Version& lhs, const Version& rhs) { // gt
        if (lhs.major != rhs.major) {
            return lhs.major > rhs.major;
        }
        else if (lhs.minor != rhs.minor) {
            return lhs.minor > rhs.minor;
        }
        else if (lhs.patch != rhs.patch) {
            return lhs.patch > rhs.patch;
        }
        else {
            return gt_pre(lhs, rhs);
        }
    }
    bool operator>(const Version& lhs, const std::string& rhs) {
        return lhs > Version(rhs);
    }
    template <typename CharT>
    bool operator>(const Version& lhs, const CharT* rhs) { // lt
        return lhs > Version(rhs);
    }

    bool operator<(const Version& lhs, const Version& rhs) { // lt
        if (lhs.major != rhs.major) {
            return lhs.major < rhs.major;
        }
        else if (lhs.minor != rhs.minor) {
            return lhs.minor < rhs.minor;
        }
        else if (lhs.patch != rhs.patch) {
            return lhs.patch < rhs.patch;
        }
        else {
            return lt_pre(lhs, rhs);
        }
    }
    template <typename CharT>
    bool operator<(const Version& lhs, const CharT* rhs) { // lt
        return lhs < Version(rhs);
    }
    bool operator<(const Version& lhs, const std::string& rhs) {
        return lhs < Version(rhs);
    }

    bool operator==(const Version& lhs, const Version& rhs) { // eq
        return lhs.major == rhs.major
            && lhs.minor == rhs.minor
            && lhs.patch == rhs.patch
            && eq_pre(lhs, rhs);
    }
    bool operator==(const Version& lhs, const std::string& rhs) {
        return lhs == Version(rhs);
    }

    template <typename CharT>
    bool operator==(const Version& lhs, const CharT* rhs) {
        return lhs == Version(rhs);
    }
    template <typename CharT>
    bool operator==(const CharT* lhs, const Version& rhs) {
        return Version(lhs) == rhs;
    }

    bool operator!=(const Version& lhs, const Version& rhs) { // neq
        return lhs.major != rhs.major
            || lhs.minor != rhs.minor
            || lhs.patch != rhs.patch
            || !eq_pre(lhs, rhs);
    }
    bool operator!=(const Version& lhs, const std::string& rhs) {
        return lhs != Version(rhs);
    }

    bool operator>=(const Version& lhs, const Version& rhs) { // gte
        return lhs > rhs || lhs == rhs;
    }
    bool operator>=(const Version& lhs, const std::string& rhs) {
        return lhs >= Version(rhs);
    }
    template <typename CharT>
    bool operator>=(const Version& lhs, const CharT* rhs) { // lt
        return lhs >= Version(rhs);
    }

    bool operator<=(const Version& lhs, const Version& rhs) { // lte
        return lhs < rhs || lhs == rhs;
    }
    bool operator<=(const Version& lhs, const std::string& rhs) {
        return lhs <= Version(rhs);
    }
    template <typename CharT>
    bool operator<=(const Version& lhs, const CharT* rhs) { // lt
        return lhs <= Version(rhs);
    }


    class Interval {
    public:
        const std::string name;
        const std::string interval;

        std::string comp_op;
        std::string version_str;

        std::string first_comp_op;
        std::string second_comp_op;
        std::string first_version;
        std::string second_version;

        int mode;

        explicit Interval(const std::string& n, const std::string& i) : name(n), interval(i) {
            std::smatch match;
            if (std::regex_match(interval, match, std::regex(CLOSED_UNBOUNDED_INTERVAL))) {
                comp_op = match[2].str();
                version_str = match[3].str() + "." + match[4].str() + "." + match[5].str();

                // Check
                if (comp_op.empty()) { // equal
                    mode = 0;
                }
                else {
                    mode = 1;
                }
            }
            else if (std::regex_match(interval, match, std::regex(BOUNDED_INTERVAL))) {
                first_comp_op = match[2].str();
                second_comp_op = match[9].str();

                first_version = match[3].str() + "." + match[4].str() + "." + match[5].str();
                first_version += match[6].matched ? ("-" + match[6].str()) : "";
                first_version += match[7].matched ? ("+" + match[7].str()) : "";

                second_version = match[10].str() + "." + match[11].str() + "." + match[12].str();
                second_version += match[13].matched ? ("-" + match[13].str()) : "";
                second_version += match[14].matched ? ("+" + match[14].str()) : "";

                // Checks
                is_wasteful_comparison_operation();
                is_bounded_interval();
                mode = 2;
            }
            else {
                throw except::error(
                        "`", name, ": ", interval, "` is invalid expression.\n"
                        "Comparison operators:\n"
                        "  >, >=, <, <=\n"
                        "Logical operator:\n"
                        "  and\n"
                        "The following example is the meaning for equals:\n"
                        "  example: \"1.2.0\"");
            }
        }

        bool satisfies(const std::string& version) {
            switch (mode) {
                case 0:
                    return version == interval;
                case 1:
                    return satisfies_closed_unbounded_interval(version);
                case 2:
                    return satisfies_bounded_interval(version);
                default:
                    throw except::error("Unexcepted error");
            }
        }

    private:
        // >2.3.0, 1.0.0, <=1.2.3-alpha, ...
        bool satisfies_closed_unbounded_interval(const std::string& v) const {
            if (comp_op == ">") {
                return Version(v) > version_str;
            }
            else if (comp_op == ">=") {
                return Version(v) >= version_str;
            }
            else if (comp_op == "<") {
                return Version(v) < version_str;
            }
            else if (comp_op == "<=") {
                return Version(v) <= version_str;
            }
            return false;
        }


        // e.g. `>0.1.3 and >=0.3.2`, `<0.1.3 and <0.3.2`
        void is_wasteful_comparison_operation() {
            if ((first_comp_op == "<" || first_comp_op == "<=")
                && (second_comp_op == "<" || second_comp_op == "<="))
            {
                if (Version(first_version) > second_version) { // Prioritize the larger version
                    throw except::error(
                            "`", name, ": ", interval, "` is invalid expression.\n"
                            "Did you mean ", first_comp_op, first_version, " ?");
                }
                else {
                    throw except::error(
                            "`", name, ": ", interval, "` is invalid expression.\n"
                            "Did you mean ", second_comp_op, second_version, " ?");
                }
            }
            else if ((first_comp_op == ">" || first_comp_op == ">=")
                     && (second_comp_op == ">" || second_comp_op == ">="))
            {
                if (Version(first_version) < second_version) { // Prioritize the smaller version
                    throw except::error(
                            "`", name, ": ", interval, "` is invalid expression.\n"
                            "Did you mean ", first_comp_op, first_version, " ?");
                }
                else {
                    throw except::error(
                            "`", name, ": ", interval, "` is invalid expression.\n"
                            "Did you mean ", second_comp_op, second_version, " ?");
                }
            }
        }

        // Check if it is bounded interval
        //  (If it is unbounded, throw error)
        // (1, 6) => open bounded interval => OK!
        // [1, 6] => closed bounded interval => OK!
        // [a, ∞) => closed unbounded interval => one_exp
        // (-∞, ∞) => closed unbounded interval => ERR!
        // e.g. <0.1.1 and >=0.3.2
        void is_bounded_interval() {
            if (Version(first_version) < second_version) {
                if ((first_comp_op == "<" || first_comp_op == "<=")
                    && (second_comp_op == ">" || second_comp_op == ">="))
                {
                    throw except::error(
                            "`", name, ": ", interval, "` is strange.\n"
                            "In this case of interval specification using `and`,\n"
                            " it is necessary to be a bounded interval.\n"
                            "Please specify as in the following example:\n"
                            "e.g. `", second_comp_op, first_version, " and ",
                            first_comp_op, second_version, "`");
                }
            }
            else if (Version(first_version) > second_version) {
                if ((first_comp_op == ">" || first_comp_op == ">=")
                    && (second_comp_op == "<" || second_comp_op == "<="))
                {
                    throw except::error(
                            "`", name, ": ", interval, "` is strange.\n"
                            "In this case of interval specification using `and`,\n"
                            " it is necessary to be a bounded interval.\n"
                            "Please specify as in the following example:\n"
                            "e.g. `", first_comp_op, second_version, " and ",
                            second_comp_op, first_version, "`");
                }
            }
        }

        bool satisfies_bounded_interval(const std::string& v) const {
            if (first_comp_op == ">") {
                if (second_comp_op == "<") {
                    return (Version(v) > first_version) && (Version(v) < second_version);
                }
                else if (second_comp_op == "<=") {
                    return (Version(v) > first_version) && (Version(v) <= second_version);
                }
            }
            else if (first_comp_op == ">=") {
                if (second_comp_op == "<") {
                    return (Version(v) >= first_version) && (Version(v) < second_version);
                }
                else if (second_comp_op == "<=") {
                    return (Version(v) >= first_version) && (Version(v) <= second_version);
                }
            }
            else if (first_comp_op == "<") {
                if (second_comp_op == ">") {
                    return (Version(v) < first_version) && (Version(v) > second_version);
                }
                else if (second_comp_op == ">=") {
                    return (Version(v) < first_version) && (Version(v) >= second_version);
                }
            }
            else if (first_comp_op == "<=") {
                if (second_comp_op == ">") {
                    return (Version(v) <= first_version) && (Version(v) > second_version);
                }
                else if (second_comp_op == ">=") {
                    return (Version(v) <= first_version) && (Version(v) >= second_version);
                }
            }
            return false;
        }
    };

    bool satisfies(const std::string& name, const std::string& version, const std::string& interval) {
        return Interval(name, interval).satisfies(version);
    }
} // end namespace
#endif // !POAC_CORE_RESOLVER_SEMVER_HPP

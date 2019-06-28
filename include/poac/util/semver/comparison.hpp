#ifndef SEMVER_COMPARISON_HPP
#define SEMVER_COMPARISON_HPP

#include "version.hpp"

namespace semver {
    namespace detail {
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
    } // end namespace detail

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
            return detail::gt_pre(lhs, rhs);
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
            return detail::lt_pre(lhs, rhs);
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
               && detail::eq_pre(lhs, rhs);
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
               || !detail::eq_pre(lhs, rhs);
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
} // end namespace semver

#endif // !SEMVER_COMPARISON_HPP

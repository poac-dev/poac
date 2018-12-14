// Miscellaneous utility
#ifndef STROITE_UTILS_MISC_HPP
#define STROITE_UTILS_MISC_HPP

#include <iostream>
#include <string>
#include <vector>

#include <boost/algorithm/string.hpp>


namespace stroite::utils::misc {
    std::vector<std::string>
    split(const std::string& raw, const std::string& delim) {
        using boost::algorithm::token_compress_on;
        using boost::is_any_of;

        std::vector<std::string> ret;
        boost::split(ret, raw, is_any_of(delim), token_compress_on);
        return ret;
    }
} // end namespace
#endif // STROITE_UTILS_MISC_HPP

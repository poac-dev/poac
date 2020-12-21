// Miscellaneous utility
#ifndef POAC_UTIL_MISC_HPP
#define POAC_UTIL_MISC_HPP

#include <iostream>
#include <string>
#include <vector>

#include <boost/algorithm/string.hpp>

namespace poac::util::misc {
    std::vector<std::string>
    split(const std::string& raw, const std::string& delim) {
        using boost::algorithm::token_compress_on;
        using boost::is_any_of;

        std::vector<std::string> ret;
        boost::split(ret, raw, is_any_of(delim), token_compress_on);
        return ret;
    }
} // end namespace
#endif // POAC_UTIL_MISC_HPP

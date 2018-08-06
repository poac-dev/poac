#ifndef POAC_SOURCES_CACHE_HPP
#define POAC_SOURCES_CACHE_HPP

#include <iostream>
#include <vector>
#include <string>

#include <boost/filesystem.hpp>

#include "../io/file.hpp"
#include "../util/package.hpp"


namespace poac::sources::cache {
    bool resolve(const std::string& name, const std::string& tag) {
        namespace path = io::file::path;
        return path::validate_dir(path::poac_cache_dir / util::package::github_conv_pkgname(name, tag));
    }
} // end namespace
#endif // !POAC_SOURCES_CACHE_HPP

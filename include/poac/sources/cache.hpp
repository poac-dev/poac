#ifndef POAC_SOURCES_CACHE_HPP
#define POAC_SOURCES_CACHE_HPP

#include <iostream>
#include <vector>
#include <string>

#include <boost/filesystem.hpp>

#include "../io/file.hpp"
#include "../core/naming.hpp"


namespace poac::sources::cache {
    bool resolve(const std::string& pkgname) {
        namespace path = io::file::path;
        return path::validate_dir(path::poac_cache_dir / pkgname);
    }
} // end namespace
#endif // !POAC_SOURCES_CACHE_HPP

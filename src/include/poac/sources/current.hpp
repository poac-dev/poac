#ifndef POAC_SOURCES_CURRENT_HPP
#define POAC_SOURCES_CURRENT_HPP

#include <iostream>
#include <vector>
#include <string>

#include <boost/filesystem.hpp>

#include "../io/file.hpp"
#include "../util/package.hpp"


namespace poac::sources::current {
    bool resolve(const std::string& pkgname) {
        namespace path = io::file::path;
        return path::validate_dir(path::current_deps_dir / pkgname);
    }
} // end namespace
#endif // !POAC_SOURCES_CURRENT_HPP

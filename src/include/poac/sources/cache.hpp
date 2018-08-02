#ifndef POAC_SOURCES_CACHE_HPP
#define POAC_SOURCES_CACHE_HPP

#include <iostream>
#include <vector>
#include <string>

#include <boost/filesystem.hpp>

#include "../io/file.hpp"


/*
 * Installability:
 *   Exist poac.(yml|yaml) in project root. (poacによる依存関係解決アルゴリズムが使用可能)
 */
namespace poac::sources::cache {
    // username/repository -> repository
    static std::string get_name(const std::string& name) {
        if (name.find('/') != std::string::npos)
            return std::string(name, name.find('/')+1);
        else
            return name;
    }

    static std::string make_name(const std::string& name, const std::string& tag) {
        return (name + "-" + tag);
    }

    bool resolve(const std::string& name, const std::string& tag) {
        return io::file::validate_dir(io::file::connect_path(io::file::POAC_CACHE_DIR, make_name(get_name(name), tag)));
    }
} // end namespace
#endif // !POAC_SOURCES_CACHE_HPP

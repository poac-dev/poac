#ifndef POAC_UTIL_MANAGE_HASH_HPP
#define POAC_UTIL_MANAGE_HASH_HPP

#include <iostream>
#include <string>
#include <vector>

#include <boost/optional.hpp>

#include "./command.hpp"
#include "./build_deps.hpp"
#include "../io/file/path.hpp"


namespace poac::util::manage_hash {
    std::string to_cache_hash_path(const std::string& s) {
        namespace fs = boost::filesystem;
        namespace iopath = io::file::path;
        return (iopath::current_build_cache_hash_dir / fs::relative(s)).string() + ".hash";
    }

    boost::optional<std::map<std::string, std::string>>
    load(const std::string& src_cpp_hash) {
        std::ifstream ifs(src_cpp_hash);
        if(!ifs.is_open()){
            return boost::none;
        }

        std::string buff;
        std::map<std::string, std::string> hash;
        while (std::getline(ifs, buff)) {
            std::vector<std::string> list_string = io::file::path::split(buff, ": \n");
            hash[list_string[0]] = list_string[1];
        }
        return hash;
    }

    void insert_file(
            const std::string& filename,
            std::map<std::string, std::string>& hash )
    {
        if (const auto str = io::file::path::read_file(filename)) {
            hash.emplace(
                filename,
                std::to_string(
                     std::hash<std::string>{}(*str)
                )
            );
        }
    }

    // *.cpp -> hash
    boost::optional<std::map<std::string, std::string>> gen(
            const std::string& system,
            const std::string& version_prefix,
            const unsigned int& cpp_version,
            const std::vector<std::string>& include_search_path,
            const std::string& src_cpp )
    {
        // TODO: build_depsからの依存を無くし，引数で受け取るようにする
        if (const auto deps_headers = build_deps::gen(system, version_prefix, cpp_version, include_search_path, src_cpp)) {
            std::map<std::string, std::string> hash;
            for (const auto& name : *deps_headers) {
                insert_file(name, hash);
            }
            // sourceファイル自体のhashを計算
            insert_file(src_cpp, hash);
            return hash;
        }
        return boost::none;
    }
} // end namespace
#endif // !POAC_UTIL_MANAGE_HASH_HPP

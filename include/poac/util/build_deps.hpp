#ifndef POAC_UTIL_BUILD_DEPS_HPP
#define POAC_UTIL_BUILD_DEPS_HPP

#include <iostream>
#include <string>
#include <vector>

#include <boost/optional.hpp>
#include <boost/algorithm/string.hpp>

#include "./command.hpp"
#include "../io/file/path.hpp"


namespace poac::util::build_deps {
    boost::optional<std::string> calc(
            const std::string& system,
            const std::string& version_prefix,
            const unsigned int& cpp_version,
            const std::vector<std::string>& include_search_path,
            const std::string& src_cpp )
    {
        command cmd(system);
        cmd += version_prefix + std::to_string(cpp_version);
        for (const auto& isp : include_search_path)
            cmd += "-I" + isp;
        cmd += "-M " + src_cpp;
        return cmd.exec();
    }

    boost::optional<std::vector<std::string>> gen(
            const std::string& system,
            const std::string& version_prefix,
            const unsigned int& cpp_version,
            const std::vector<std::string>& include_search_path,
            const std::string& src_cpp )
    {
        if (const auto ret = calc(system, version_prefix, cpp_version, include_search_path, src_cpp)) {
            std::vector<std::string> deps_headers = io::file::path::split(*ret, " \n\\");
            deps_headers.erase(deps_headers.begin()); // main.o:
            deps_headers.erase(deps_headers.begin()); // main.cpp
            return deps_headers;
        }
        else {
            return boost::none;
        }
    }
} // end namespace
#endif // !POAC_UTIL_BUILD_DEPS_HPP

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
            // TODO: other_argとして，include search pathを指定する可能性がある．
            const std::vector<std::string>& compile_other_args,
            const std::string& src_cpp,
            const bool verbose )
    {
        command cmd(system);
        cmd += version_prefix + std::to_string(cpp_version);
        for (const auto& isp : include_search_path)
            cmd += "-I" + isp;
        for (const auto& cta : compile_other_args)
            cmd += cta;
        cmd += "-M " + src_cpp;

        if (verbose) std::cout << cmd << std::endl;

        return cmd.exec();
    }

    boost::optional<std::vector<std::string>> gen(
            const std::string& system,
            const std::string& version_prefix,
            const unsigned int& cpp_version,
            const std::vector<std::string>& include_search_path,
            const std::vector<std::string>& compile_other_args,
            const std::string& src_cpp,
            const bool verbose )
    {
        if (const auto ret = calc(
                system,
                version_prefix,
                cpp_version,
                include_search_path,
                compile_other_args,
                src_cpp,
                verbose ))
        {
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

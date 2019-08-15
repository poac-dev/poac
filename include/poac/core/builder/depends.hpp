#ifndef POAC_CORE_BUILDER_DEPENDS_HPP
#define POAC_CORE_BUILDER_DEPENDS_HPP

#include <iostream>
#include <string>
#include <vector>
#include <optional>

#include <poac/util/shell.hpp>
#include <poac/util/misc.hpp>

namespace poac::core::builder::depends {
    template <typename Opts>
    std::optional<std::string>
    calc(const Opts& opts, const std::string& src_cpp, const bool verbose)
    {
        util::shell cmd(opts.system);
        cmd += opts.std_version;
        for (const auto& isp : opts.include_search_path) {
            cmd += "-I" + isp.string();
        }
        for (const auto& cta : opts.other_args) {// TODO: other_argとして，include search pathを指定する可能性がある．
            cmd += cta;
        }
        // Like -M but do not mention header files that are found in system header directories,
        //  nor header files that are included, directly or indirectly, from such a header.
        // This implies that the choice of angle brackets or double quotes in an ‘#include’ directive
        //  does not in itself determine whether that header appears in -MM dependency output.
        // (https://gcc.gnu.org/onlinedocs/gcc/Preprocessor-Options.html#Preprocessor-Options)
        cmd += "-MM " + src_cpp;

        if (verbose) {
            std::cout << cmd << std::endl;
        }
        return cmd.exec();
    }

    template <typename Opts>
    std::optional<std::vector<std::string>>
    gen(const Opts& opts, const std::string& src_cpp, const bool verbose)
    {
        if (const auto ret = calc(opts, src_cpp, verbose)) {
            auto deps_headers = util::misc::split(*ret, " \n\\");
            deps_headers.erase(deps_headers.begin()); // main.o:
            deps_headers.erase(deps_headers.begin()); // main.cpp
            return deps_headers;
        }
        else {
            return std::nullopt;
        }
    }
} // end namespace
#endif // POAC_CORE_BUILDER_DEPENDS_HPP

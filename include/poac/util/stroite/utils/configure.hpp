#ifndef STROITE_UTILS_CONFIGURE_HPP
#define STROITE_UTILS_CONFIGURE_HPP

#include <iostream>
#include <string>
#include <vector>

#include <boost/algorithm/string.hpp>

#include "../../../core/exception.hpp"
#include "../../command.hpp"


namespace stroite::utils::configure {
    template <typename Opts>
    void enable_gnu(Opts& opts) {
        opts.version_prefix = "-std=gnu++";
    }
    std::string default_version_prefix() {
        return "-std=c++";
    }
    std::string make_macro_defn(const std::string& first, const std::string& second) {
        return "-D" + first + "=" + R"(\")" + second + R"(\")";
    }

    // Automatic selection of compiler
    auto auto_select_compiler() {
        using poac::core::exception::error;
        using poac::util::command;

        if (const char* cxx = std::getenv("CXX")) {
            return cxx;
        }
        else if (command("command -v g++ >/dev/null 2>&1").exec()) {
            return "g++";
        }
        else if (command("command -v clang++ >/dev/null 2>&1").exec()) {
            return "clang++";
        }
        else {
            throw error(
                    "Environment variable \"CXX\" was not found.\n"
                    "Select the compiler and export it.");
        }
    }
} // end namespace
#endif // STROITE_UTILS_CONFIGURE_HPP

#ifndef POAC_CORE_RESOLVER_HPP
#define POAC_CORE_RESOLVER_HPP

#include <string>
#include <utility>

#include <yaml-cpp/yaml.h>
#include <cvc4/cvc4.h>

#include "exception.hpp"
#include "../io/file.hpp"


namespace poac::core::resolver {
    namespace cache {
        bool resolve(const std::string& package_name) {
            namespace path = io::file::path;
            const auto package_path = path::poac_cache_dir / package_name;
            return path::validate_dir(package_path);
        }
    }

    namespace current {
        bool resolve(const std::string& current_package_name) {
            namespace path = io::file::path;
            const auto package_path = path::current_deps_dir / current_package_name;
            return path::validate_dir(package_path);
        }
    }

    namespace github {
        std::string resolve(const std::string& name, const std::string& tag) {
            return "https://github.com/" + name + "/archive/" + tag + ".tar.gz";
        }
        std::string resolve(const std::string& name) {
            return "https://github.com/" + name + ".git";
        }
    }

    namespace poac {
        void resolve() {
        }
    }
} // end namespace
#endif // !POAC_CORE_RESOLVER_HPP

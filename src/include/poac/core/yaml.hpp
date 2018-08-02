#ifndef POAC_CORE_YAML_HPP
#define POAC_CORE_YAML_HPP

#include <iostream>
#include <string>

#include <boost/filesystem.hpp>
#include <yaml-cpp/yaml.h>

#include "../io/cli.hpp"


// TODO: mv core::yaml to io::file::yaml???
namespace poac::core::yaml {
    namespace fs = boost::filesystem;


    bool _exists(const fs::path& dir) {
        // If both are present, select the former.
        return fs::exists(dir / fs::path("poac.yml")) ||
               fs::exists(dir / fs::path("poac.yaml"));
    }
    [[maybe_unused]] bool exists(const fs::path& dir) {
        return _exists(dir);
    }
    bool exists(/* current directory */) {
        return _exists(fs::path("."));
    }

    bool notfound_handle() {
        const bool ret = exists();
        if (!ret) {
            std::cerr << io::cli::red
                      << "ERROR: poac.yml is not found"
                      << io::cli::reset
                      << std::endl;
        }
        return ret;
    }

    YAML::Node get_node() {
        return YAML::LoadFile("./poac.yml");
    }
    YAML::Node get_node(const std::string& name) {
        return YAML::LoadFile("./poac.yml")[name];
    }
} // end namespace
#endif // !POAC_CORE_YAML_HPP

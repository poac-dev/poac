#ifndef POAC_IO_FILE_YAML_HPP
#define POAC_IO_FILE_YAML_HPP

#include <iostream>
#include <string>

#include <boost/filesystem.hpp>
#include <boost/optional.hpp>
#include <yaml-cpp/yaml.h>

#include "../../io/cli.hpp"


namespace poac::io::file::yaml {
    bool _exists(const boost::filesystem::path& dir) {
        namespace fs = boost::filesystem;
        // If both are present, select the former.
        return fs::exists(dir / fs::path("poac.yml")) ||
               fs::exists(dir / fs::path("poac.yaml"));
    }
    bool exists(const boost::filesystem::path& dir) {
        return _exists(dir);
    }
    bool exists(/* current directory */) {
        return _exists(boost::filesystem::current_path());
    }

    template <typename T>
    boost::optional<T> get(const YAML::Node& node, const std::string& key) {
        try { return node[key].as<T>(); }
        catch (...) { return boost::none; }
    }
    template <typename T>
    boost::optional<T> get2(const YAML::Node& node, const std::string& key1, const std::string& key2) {
        try { return node[key1][key2].as<T>(); }
        catch (...) { return boost::none; }
    }

    YAML::Node get_node() {
        return YAML::LoadFile("./poac.yml");
    }
    YAML::Node get_node(const std::string& name) {
        return YAML::LoadFile("./poac.yml")[name];
    }
} // end namespace
#endif // !POAC_IO_FILE_YAML_HPP

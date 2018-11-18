#ifndef POAC_IO_FILE_YAML_HPP
#define POAC_IO_FILE_YAML_HPP

#include <iostream>
#include <string>
#include <map>

#include <boost/filesystem.hpp>
#include <boost/optional.hpp>
#include <yaml-cpp/yaml.h>

#include "../../core/exception.hpp"


namespace poac::io::file::yaml {
    boost::optional<std::string> exists_setting_file(
            const boost::filesystem::path& basepath )
    {
        namespace fs = boost::filesystem;
        if (const auto yml = basepath / "poac.yml"; fs::exists(yml))
            return yml.string();
        else if (const auto yaml = basepath / "poac.yaml"; fs::exists(yaml))
            return yaml.string();
        else
            return boost::none;
    }
    boost::optional<std::string> exists_setting_file() {
        namespace fs = boost::filesystem;
        return exists_setting_file(fs::current_path());
    }

    boost::optional<YAML::Node> load(const std::string& filename) {
        try { return YAML::LoadFile(filename); }
        catch (...) { return boost::none; }
    }

    template <typename T>
    boost::optional<T> get(const YAML::Node& node) {
        try { return node.as<T>(); }
        catch (...) { return boost::none; }
    }
    template <typename T>
    boost::optional<T> get1(const YAML::Node& node, const std::string& key) {
        try { return node[key].as<T>(); }
        catch (...) { return boost::none; }
    }
    template <typename T>
    boost::optional<T> get2(const YAML::Node& node, const std::string& key1, const std::string& key2) {
        try { return node[key1][key2].as<T>(); }
        catch (...) { return boost::none; }
    }

    // TODO: keyが無くても無視して，許容されるfunctionがほしい．もしくは，optional指定子を付けるとか
    // {{"arg1", node["arg1"]}, ...}
    // get_by_widthは，そのkeyが存在することは保証するが，
    // そのnodeがasで，指定の型に変換可能であるかどうかは保証していない
    // そのため，asで変換できなかった時，exceptionが飛んでしまう．
    // 例えば，keyだけ書かれていた場合..
    template <typename... Args>
    static boost::optional<std::map<std::string, YAML::Node>>
    get_by_width(const YAML::Node &node, const Args&... args) {
        std::map<std::string, YAML::Node> mp;
        try {
            ((mp[args] = node[args]), ...);
            return mp;
        }
        catch (...) { return boost::none; }
    }
    // node[arg1][arg2]...
    // TODO: 多分，内部がポインタで実装されてて，書き換えると，どこから見ても書き換わってしまう．
    // TODO: YAML::Cloneを使用すると良い？？
    template <typename... Args>
    static boost::optional<YAML::Node>
    get_by_depth(YAML::Node node, const Args&... args) {
        try {
            ((node = node[args]), ...);
            return node;
        }
        catch (...) { return boost::none; }
    }

    // これには，必須のkeyを指定する．
    // optionalの場合なら，別のタイミングで抽出すること．
    template <typename... Args>
    static auto load_setting_file(const Args&... args) {
        namespace except = core::exception;

        // TODO: I want use Result type like rust-lang.
        if (const auto op_filename = exists_setting_file()) {
            if (const auto op_node = load(*op_filename)) {
                if (const auto op_select_node = get_by_width(*op_node, args...)) {
                    return *op_select_node;
                }
                else {
                    throw except::error("Required key does not exist in poac.yml");
                }
            }
            else {
                throw except::error("Could not load poac.yml");
            }
        }
        else {
            throw except::error("poac.yml does not exists");
        }
    }
//    template <typename T>
//    static auto load_setting_file(const std::initializer_list<T> args) {
//          TODO: variadic to initializer list
//    }

    // keyが無くても無視される
    template <typename... Args>
    static auto load_setting_file_opt(const Args&... args) {
        namespace except = core::exception;

        // TODO: I want use Result type like rust-lang.
        if (const auto op_filename = exists_setting_file()) {
            if (const auto op_node = load(*op_filename)) {
                return get_by_width(*op_node, args...);
            }
            else {
                throw except::error("Could not load poac.yml");
            }
        }
        else {
            throw except::error("poac.yml does not exists");
        }
    }
} // end namespace
#endif // !POAC_IO_FILE_YAML_HPP

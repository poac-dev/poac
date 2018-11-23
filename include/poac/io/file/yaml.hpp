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
    namespace detail {
        struct wrapper {
            YAML::Node node;
            wrapper(const YAML::Node& n) {
                node = YAML::Clone(n);
            }
            template <typename Key>
            wrapper& operator->*(const Key& key) {
                this->node = node[key];
                return *this;
            }
        };

        template <typename T>
        T get(const YAML::Node& node) {
            return node.as<T>();
        }
        template <typename T, typename ...Keys>
        T get(const YAML::Node& node, Keys&&... keys) {
            return (wrapper(node) ->* ... ->* keys).node.template as<T>();
        }
    }

    template <typename T, typename ...Args>
    boost::optional<T> get(const Args&... args) {
        try {
            return detail::get<T>(args...);
        }
        catch (const YAML::BadConversion& e) {
            return boost::none;
        }
    }


    boost::optional<std::string>
    exists_config_file(const boost::filesystem::path &basepath)
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
        return exists_config_file(fs::current_path());
    }

    boost::optional<YAML::Node> load(const std::string& filename) {
        try { return YAML::LoadFile(filename); }
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
        catch (const YAML::BadConversion& e) {
            return boost::none;
        }
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

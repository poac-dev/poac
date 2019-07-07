#ifndef POAC_IO_YAML_HPP
#define POAC_IO_YAML_HPP

#include <iostream>
#include <string>
#include <map>
#include <optional>
#include <fstream>

#include <boost/predef.h>
#include <boost/filesystem.hpp>
#include <yaml-cpp/yaml.h>

#include <poac/core/except.hpp>

namespace poac::io::yaml {
    namespace detail {
        struct wrapper {
            YAML::Node node;
            explicit wrapper(const YAML::Node& n) {
                // Argument-dependent lookup (ADL)
                node = Clone(n);
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
            return get<T>((wrapper(node) ->* ... ->* keys).node);
        }

        // Private member accessor
        using YAML_Node_t = bool YAML::Node::*;
#if BOOST_COMP_MSVC
        template <class T>
        struct accessor {
            static T m_isValid;
            static T get() noexcept {
                return m_isValid;
            }
        };
        template <class T>
        T accessor<T>::m_isValid;

        template <class T, T V>
        struct bastion {
            bastion() { accessor<T>::m_isValid = V; }
        };

        template struct bastion<YAML_Node_t, &YAML::Node::m_isValid>;
        using access = accessor<YAML_Node_t>;
#else
        template <class T, T V>
        struct accessor {
            static constexpr T m_isValid = V;
            static T get() noexcept {
                return m_isValid;
            }
        };
        template <typename T>
        using bastion = accessor<T, &YAML::Node::m_isValid>;
        using access = bastion<YAML_Node_t>;
#endif

        template <typename Head>
        std::optional<const char*>
        read(const YAML::Node& node, Head&& head) {
            if (!(node[head].*access::get())) {
                return head;
            }
            else {
                return std::nullopt;
            }
        }
        template <typename Head, typename ...Tail>
        std::optional<const char*>
        read(const YAML::Node& node, Head&& head, Tail&&... tail) {
            if (!(node[head].*access::get())) {
                return head;
            }
            else {
                return read(node, tail...);
            }
        }
    }

    template <typename T>
    std::optional<T>
    get(const YAML::Node& node) noexcept {
        try {
            return detail::get<T>(node);
        }
        catch (...) {
            return std::nullopt;
        }
    }

    template <typename T, typename ...Args>
    std::optional<T>
    get(const YAML::Node& node, Args&&... args) noexcept {
        try {
            return detail::get<T>(node, args...);
        }
        catch (...) {
            return std::nullopt;
        }
    }
    template <typename... Args>
    bool get(const YAML::Node& node, Args&&... args) noexcept {
        try {
            return detail::get<bool>(node, args...);
        }
        catch (...) {
            return false;
        }
    }

    template <typename... Args>
    bool contains(const YAML::Node& node, Args&&... args) {
        // has_value -> not contains
        return !static_cast<bool>(detail::read(node, args...));
    }

    template <typename T>
    T get_with_throw(const YAML::Node& node) {
        namespace except = core::except;
        try {
            return detail::get<T>(node);
        }
        catch (...) {
            throw except::error(
                    except::msg::key_does_not_exist(""), "\n",
                    except::msg::please_refer_docs(""));
        }
    }
    template <typename T>
    T get_with_throw(const YAML::Node& node, const std::string& arg) {
        namespace except = core::except;
        try {
            return detail::get<T>(node, arg);
        }
        catch (...) {
            throw except::error(
                    except::msg::key_does_not_exist(arg), "\n",
                    except::msg::please_refer_docs(""));
        }
    }

    template <typename... Args>
    std::map<std::string, YAML::Node>
    get_by_width(const YAML::Node& node, const Args&... args) {
        namespace except = core::except;
        if (const auto result = detail::read(node, args...)) {
            throw except::error(
                    except::msg::key_does_not_exist(std::string(*result)), "\n",
                    except::msg::please_refer_docs(""));
        }
        else {
            std::map<std::string, YAML::Node> mp;
            ((mp[args] = node[args]), ...);
            return mp;
        }
    }
    template <typename Arg>
    YAML::Node
    get_by_width(const YAML::Node& node, const Arg& args) {
        namespace except = core::except;
        if (const auto result = detail::read(node, args)) {
            throw except::error(
                    except::msg::key_does_not_exist(std::string(*result)), "\n",
                    except::msg::please_refer_docs(""));
        }
        else {
            return node[args];
        }
    }

    template <typename... Args>
    std::optional<std::map<std::string, YAML::Node>>
    get_by_width_opt(const YAML::Node& node, const Args&... args) {
        if (detail::read(node, args...)) {
            return std::nullopt;
        }
        else {
            std::map<std::string, YAML::Node> mp;
            ((mp[args] = node[args]), ...);
            return mp;
        }
    }

    std::optional<std::string>
    exists_config(const boost::filesystem::path& base)
    {
        namespace fs = boost::filesystem;
        if (const auto yml = base / "poac.yml"; fs::exists(yml)) {
            return yml.string();
        }
        else {
            return std::nullopt;
        }
    }
    std::optional<std::string>
    exists_config() {
        namespace fs = boost::filesystem;
        return exists_config(fs::current_path());
    }
    std::optional<YAML::Node>
    load(const std::string& filename) {
        try { return YAML::LoadFile(filename); }
        catch (...) { return std::nullopt; }
    }

    YAML::Node load_config() {
        namespace except = core::except;
        if (const auto op_filename = exists_config()) {
            if (const auto op_node = load(*op_filename)) {
                return *op_node;
            }
        }
        throw except::error(
                except::msg::does_not_exist("poac.yml"), "\n",
                except::msg::please_exec("`poac init` or `poac new $PROJNAME`"));
    }
    // TODO: この時点で，型情報を渡していて，これに則しない，keyを読むとエラー？
    //  load_config<CppVersion, Deps>()
    //  struct hog : hoge... {
    //     cpp_version: u64,
    //     deps: any
    //  }
    //  -> return
    // config.cpp_version() -> u64
    template <typename ...Args>
    auto load_config(const Args&... args) {
        return get_by_width(load_config(), args...);
    }

    std::optional<YAML::Node>
    load_config_by_dir(const boost::filesystem::path& base) {
        if (const auto op_filename = exists_config(base)) {
            if (const auto op_node = load(*op_filename)) {
                return op_node;
            }
        }
        return std::nullopt;
    }
    YAML::Node load_config_by_dir_with_throw(const boost::filesystem::path& base) {
        namespace except = core::except;
        if (const auto op_filename = exists_config(base)) {
            if (const auto op_node = load(*op_filename)) {
                return *op_node;
            }
        }
        throw except::error(
                except::msg::does_not_exist("poac.yml"), "\n",
                except::msg::please_exec("`poac init` or `poac new $PROJNAME`"));
    }

    std::string load_timestamp() {
        namespace fs = boost::filesystem;
        namespace except = core::except;

        if (const auto op_filename = exists_config()) {
            boost::system::error_code error;
            const std::time_t last_time = fs::last_write_time(*op_filename, error);
            return std::to_string(last_time);
        }
        else {
            throw except::error(
                    except::msg::does_not_exist("poac.yml"), "\n",
                    except::msg::please_exec("`poac init` or `poac new $PROJNAME`"));
        }
    }
} // end namespace
#endif // !POAC_IO_YAML_HPP

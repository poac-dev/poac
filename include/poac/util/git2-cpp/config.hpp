#ifndef GIT2_CONFIG_HPP
#define GIT2_CONFIG_HPP

#include <string>
#include <git2/config.h>
#include <poac/util/git2-cpp/global.hpp>
#include <poac/util/git2-cpp/exception.hpp>

namespace git2 {
    class config {
        git_config* cfg;

    public:
        config();
        ~config();

        explicit config(git_config* cfg) : cfg(cfg) {}

        config(const config&) = delete;
        config& operator=(const config&) = delete;
        config(config&&) = default;
        config& operator=(config&&) = default;

        std::string get_string(const std::string& name);
    };

    config::config() {
        git2::init();
        git2_throw(git_config_new(&this->cfg));
    }

    config::~config() {
        git_config_free(this->cfg);
    }

    /// Get the value of a string config variable as an owned string.
    std::string
    config::get_string(const std::string& name) {
        git_buf ret = { nullptr, 0, 0 };
        git2_throw(git_config_get_string_buf(&ret, this->cfg, name.c_str()));
        return std::string(ret.ptr, ret.size);
    }

} // end namespace git2

#endif	// !GIT2_CONFIG_HPP

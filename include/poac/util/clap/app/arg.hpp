#ifndef CLAP_APP_ARG_HPP
#define CLAP_APP_ARG_HPP

#include <string>
#include <vector>

namespace clap {
    struct arg {
        std::string m_name;
        std::string m_help;
        std::string m_short;
        std::string m_long;
        std::string m_value_name;
        std::vector<std::string> m_possible_values;
        bool m_required;
        bool m_multiple;
        bool m_global;

        arg() = delete;
        explicit arg(const std::string& name) : m_name(name) {}
        ~arg() = default;

        arg(const arg&) = default;
        arg& operator=(const arg&) = default;
        arg(arg&&) = default;
        arg& operator=(arg&&) = default;

        arg& help(const std::string&);
        arg& short_(const std::string&);
        arg& long_(const std::string&);
        arg& value_name(const std::string&);
        arg& possible_values(const std::vector<std::string>&);
        arg& required(const bool);
        arg& multiple(const bool);
        arg& global(const bool);
    };

    arg&
    arg::help(const std::string& help) {
        this->m_help = help;
        return *this;
    }

    arg&
    arg::short_(const std::string& short_) {
        this->m_short = short_;
        return *this;
    }

    arg&
    arg::long_(const std::string& long_) {
        this->m_long = long_;
        return *this;
    }

    arg&
    arg::value_name(const std::string& name) {
        this->m_value_name = name;
        return *this;
    }

    arg&
    arg::possible_values(const std::vector<std::string>& values) {
        this->m_possible_values = values;
        return *this;
    }

    arg&
    arg::required(const bool required) {
        this->m_required = required;
        return *this;
    }

    arg&
    arg::multiple(const bool multiple) {
        this->m_multiple = multiple;
        return *this;
    }

    arg&
    arg::global(const bool global) {
        this->m_global = global;
        return *this;
    }
} // end namespace clap

#endif	// !CLAP_APP_ARG_HPP

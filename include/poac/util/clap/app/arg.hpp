#ifndef CLAP_APP_ARG_HPP
#define CLAP_APP_ARG_HPP

#include <string>
#include <vector>

namespace clap {
    class arg {
        std::string m_name;
        std::string m_help;
        std::string m_short;
        std::string m_long;
        std::string m_value_name;
        std::vector<std::string> m_possible_values;
        bool m_required;

    public:
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
    arg::value_name(const std::string& value_name) {
        this->m_value_name = value_name;
        return *this;
    }

    arg&
    arg::possible_values(const std::vector<std::string>& possible_values) {
        this->m_possible_values = possible_values;
        return *this;
    }

    arg&
    arg::required(const bool required) {
        this->m_required = required;
        return *this;
    }
} // end namespace clap

#endif	// !CLAP_APP_ARG_HPP

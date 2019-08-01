#ifndef CLAP_APP_APP_HPP
#define CLAP_APP_APP_HPP

// std
#include <vector>
// internal
#include <poac/util/clap/app/subcommand.hpp>

namespace clap {
    class app {
        std::string m_name;
        std::string m_about;

    public:
        app() = delete;
        explicit app(const std::string& name) : m_name(name) {}
        ~app() = default;

        app(const app&) = default;
        app& operator=(const app&) = default;
        app(app&&) = default;
        app& operator=(app&&) = default;

        app& about(const std::string&);

//        app& arg(const clap::arg&);
//        app& arg(clap::arg&&);
    };
} // end namespace clap

#endif	// !CLAP_APP_APP_HPP

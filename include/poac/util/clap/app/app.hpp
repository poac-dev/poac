#ifndef CLAP_APP_APP_HPP
#define CLAP_APP_APP_HPP

// std
#include <vector>
// internal
#include <poac/util/clap/app/subcommand.hpp>

namespace clap {
    class app {
        std::string m_name;
        std::string m_version;
        std::string m_about;
        std::vector<clap::arg> m_args;
        std::vector<clap::subcommand> m_subcmds;

    public:
        app() = delete;
        explicit app(const std::string& name) : m_name(name) {}
        ~app() = default;

        app(const app&) = default;
        app& operator=(const app&) = default;
        app(app&&) = default;
        app& operator=(app&&) = default;

        app& version(const std::string&);

        app& about(const std::string&);

        app& arg(const clap::arg&);
        app& arg(clap::arg&&);

        app& subcommand(const clap::subcommand&);
        app& subcommand(clap::subcommand&&);

        app& subcommands(const std::vector<clap::subcommand>&);

        // poac, build, --verbose -> build, --verbose
        // poac, help, run -> {}
        std::vector<std::string> parse(const std::vector<std::string>&) const;
    };

    app&
    app::arg(const clap::arg& arg) {
        this->m_args.push_back(arg);
        return *this;
    }
    app&
    app::arg(clap::arg&& arg) {
        this->m_args.emplace_back(std::move(arg));
        return *this;
    }

    app&
    app::version(const std::string& version) {
        this->m_version = version;
        return *this;
    }

    app&
    app::subcommand(const clap::subcommand& subcmd) {
        this->m_subcmds.push_back(subcmd);
        return *this;
    }
    app&
    app::subcommand(clap::subcommand&& subcmd) {
        this->m_subcmds.emplace_back(std::move(subcmd));
        return *this;
    }

    app&
    app::subcommands(const std::vector<clap::subcommand>& subcmds) {
        this->m_subcmds = subcmds;
        return *this;
    }
} // end namespace clap

#endif	// !CLAP_APP_APP_HPP

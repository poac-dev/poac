#ifndef CLAP_APP_SUBCOMMAND_HPP
#define CLAP_APP_SUBCOMMAND_HPP

#include <iostream>
#include <string>
#include <vector>
#include <poac/util/clap/app/arg.hpp>

namespace clap {
    class subcommand {
        std::string m_name;
        std::string m_about;
        std::vector<clap::arg> m_args;

    public:
        subcommand() = delete;
        explicit subcommand(const std::string& name) : m_name(name) {}
        ~subcommand() = default;

        subcommand(const subcommand&) = default;
        subcommand& operator=(const subcommand&) = default;
        subcommand(subcommand&&) = default;
        subcommand& operator=(subcommand&&) = default;

        friend std::ostream& operator<<(std::ostream& os, const subcommand& s);

        subcommand& about(const std::string&);

        subcommand& arg(const clap::arg&);
        subcommand& arg(clap::arg&&);
    };

    subcommand&
    subcommand::about(const std::string& about) {
        this->m_about = about;
        return *this;
    }

    subcommand&
    subcommand::arg(const clap::arg& arg) {
        this->m_args.push_back(arg);
        return *this;
    }
    subcommand&
    subcommand::arg(clap::arg&& arg) {
        this->m_args.emplace_back(std::move(arg));
        return *this;
    }

    std::ostream& operator<<(std::ostream& os, const subcommand& s) {
        os << "poac-" << s.m_name << "\n"
           << s.m_about << "\n\n"
           << "USAGE:\n"
           << "    " << "poac " << s.m_name << " [OPTIONS] <path>" << "\n";

        std::vector<clap::arg> args;

        bool first = true;
        for (const auto& h : s.m_args) {
            if (h.m_short.empty() && h.m_long.empty()) {
                args.emplace_back(h);
            } else {
                if (first) {
                    first = false;
                    os << "\nOPTIONS:\n";
                }

                std::string option;
                if (h.m_short.empty()) {
                    option = "    --" + h.m_long;
                } else {
                    option = "-" + h.m_short + ", " + "--" + h.m_long;
                }
                os << "    " << std::left << std::setw(20) << option << h.m_help << "\n";
            }
        }

        if (!args.empty()) {
            os << "\nARGS:\n";
            for (const auto& a : args) {
                os << "    " << "<" << a.m_name << ">" << (a.m_multiple ? "..." : "") << "\n";
            }
        }

        return os;
    }
} // end namespace clap

#endif	// !CLAP_APP_SUBCOMMAND_HPP

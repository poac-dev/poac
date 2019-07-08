#ifndef POAC_OPTS_HELP_HPP
#define POAC_OPTS_HELP_HPP

#include <iostream>
#include <vector>
#include <string>
#include <utility>
#include <unordered_map>
#include <cstdlib>

#include <boost/predef.h>

#include <poac/opts/build.hpp>
#include <poac/opts/cache.hpp>
#include <poac/opts/cleanup.hpp>
#include <poac/opts/graph.hpp>
#include <poac/opts/init.hpp>
#include <poac/opts/install.hpp>
#include <poac/opts/new.hpp>
#include <poac/opts/publish.hpp>
#include <poac/opts/root.hpp>
#include <poac/opts/run.hpp>
#include <poac/opts/search.hpp>
#include <poac/opts/test.hpp>
#include <poac/opts/uninstall.hpp>
#include <poac/opts/update.hpp>
#include <poac/opts/version.hpp>
#include <poac/core/except.hpp>
#include <poac/util/termcolor2.hpp>

namespace poac::opts::help {
    constexpr auto summary = termcolor2::make_string("Display help for a command");
    constexpr auto options = termcolor2::make_string("<sub-command or option>");

    struct Options {
        enum class Type {
            Summary,
            Usage,
        };
        Type type;
        std::string cmd;
    };

    template <typename CharT, std::size_t N, typename Traits>
    constexpr auto
    decorate_summary(const termcolor2::basic_string<CharT, N, Traits>& str) {
        return termcolor2::yellow<> + str + termcolor2::reset<> + "\n";
    }
    template <typename CharT, std::size_t N, typename Traits>
    constexpr auto
    decorate_name(const termcolor2::basic_string<CharT, N, Traits>& str) {
        // TODO: padding function s -> "build" 9 -> "build    "
        return termcolor2::blue<> + termcolor2::bold<> + "   " + str + "   " + termcolor2::reset<>;
    }
    template <typename CharT, std::size_t N, typename Traits, std::size_t M>
    constexpr auto
    decorate(const CharT(&s1)[N], const termcolor2::basic_string<CharT, M, Traits>& s2) {
        return decorate_name(termcolor2::basic_string<CharT, N - 1>(s1)) + decorate_summary(s2);
    }

    constexpr auto
    construct_summary() {
        return decorate("build    ", opts::build::summary)
             + decorate("cache    ", opts::cache::summary)
             + decorate("cleanup  ", opts::cleanup::summary)
             + decorate("graph    ", opts::graph::summary)
             + decorate("help     ", opts::help::summary)
             + decorate("init     ", opts::init::summary)
             + decorate("install  ", opts::install::summary)
             + decorate("new      ", opts::_new::summary)
             + decorate("publish  ", opts::publish::summary)
             + decorate("root     ", opts::root::summary)
             + decorate("run      ", opts::run::summary)
             + decorate("search   ", opts::search::summary)
             + decorate("test     ", opts::test::summary)
             + decorate("uninstall", opts::uninstall::summary)
             + decorate("update   ", opts::update::summary)
             + decorate("version  ", opts::version::summary);
    }

#if BOOST_COMP_MSVC
    using termcolor2::color_literals::operator""_bold;
    const auto summary_string =
            "Usage: poac <command> [<args>]\n\n" +
            "Available commands:"_bold + '\n' +
            construct_summary().to_string() +
            "\nSee `poac <command> --help` for information on a specific command.\n"
            "For full documentation, see: https://github.com/poacpm/poac#readme";
#else
    constexpr auto summary_string =
            termcolor2::make_string("Usage: poac <command> [<args>]\n\n") +
            termcolor2::bold<> + "Available commands:" + termcolor2::reset<> + '\n' +
            construct_summary() +
            "\nSee `poac <command> --help` for information on a specific command.\n"
            "For full documentation, see: https://github.com/poacpm/poac#readme";
#endif

    // TODO: このmapを用意しているのが無駄．summaryの形成に使用していない
    const std::unordered_map<std::string, std::string>
    summaries_map{
        { "build",     opts::build::summary.to_string() },
        { "cache",     opts::cache::summary.to_string() },
        { "cleanup",   opts::cleanup::summary.to_string() },
        { "graph",     opts::graph::summary.to_string() },
        { "help",      opts::help::summary.to_string() },
        { "init",      opts::init::summary.to_string() },
        { "install",   opts::install::summary.to_string() },
        { "new",       opts::_new::summary.to_string() },
        { "publish",   opts::publish::summary.to_string() },
        { "root",      opts::root::summary.to_string() },
        { "run",       opts::run::summary.to_string() },
        { "search",    opts::search::summary.to_string() },
        { "test",      opts::test::summary.to_string() },
        { "uninstall", opts::uninstall::summary.to_string() },
        { "update",    opts::update::summary.to_string() },
        { "version",   opts::version::summary.to_string() }
    };

    const std::unordered_map<std::string, std::string>
    options_map{
        { "build",     opts::build::options.to_string() },
        { "cache",     opts::cache::options.to_string() },
        { "cleanup",   opts::cleanup::options.to_string() },
        { "graph",     opts::graph::options.to_string() },
        { "help",      opts::help::options.to_string() },
        { "init",      opts::init::options.to_string() },
        { "install",   opts::install::options.to_string() },
        { "new",       opts::_new::options.to_string() },
        { "publish",   opts::publish::options.to_string() },
        { "root",      opts::root::options.to_string() },
        { "run",       opts::run::options.to_string() },
        { "search",    opts::search::options.to_string() },
        { "test",      opts::test::options.to_string() },
        { "uninstall", opts::uninstall::options.to_string() },
        { "update",    opts::update::options.to_string() },
        { "version",   opts::version::options.to_string() }
    };

    void usage(const std::string& s) {
        const std::string opt = options_map.at(s);
        const std::string indent = "    ";

        std::cout << "poac-" << s << "\n"
                  << summaries_map.at(s) << "\n\n"
                  << "USAGE:\n"
                  << indent << "poac " << s << " [OPTIONS]\n\n"
                  << "OPTIONS:\n"
                  << indent << opt << std::endl; // TODO: optionの表示をもっとわかりやすくする．cargoのように
    }

    [[nodiscard]] std::optional<core::except::Error>
    help(help::Options&& opts) {
        switch (opts.type) {
            case help::Options::Type::Summary:
                std::cout << summary_string << std::endl;
                return std::nullopt;
            case help::Options::Type::Usage:
                usage(opts.cmd);
                return std::nullopt;
        }
    }

    [[nodiscard]] std::optional<core::except::Error>
    exec(std::optional<io::yaml::Config>&&, std::vector<std::string>&& args) {
        help::Options opts{};
        if (args.size() == 0) {
            opts.type = help::Options::Type::Summary;
        } else if (args.size() == 1) {
            opts.type = help::Options::Type::Usage;
            opts.cmd = args[0];
        } else {
            return core::except::Error::InvalidSecondArg::Help;
        }
        return help::help(std::move(opts));
    }
} // end namespace
#endif // !POAC_OPTS_HELP_HPP

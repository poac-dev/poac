#ifndef POAC_OPTS_CACHE_HPP
#define POAC_OPTS_CACHE_HPP

#include <iostream>
#include <string>
#include <regex>
#include <optional>

#include <boost/filesystem.hpp>
#include <boost/range/iterator_range.hpp>

#include <poac/core/except.hpp>
#include <poac/io/path.hpp>
#include <poac/io/term.hpp>
#include <poac/io/yaml.hpp>
#include <poac/util/argparse.hpp>
#include <poac/util/termcolor2.hpp>

namespace poac::opts::cache {
    constexpr auto summary = termcolor2::make_string("Manipulate cache files");
    constexpr auto options = termcolor2::make_string("[-a, --all | --pattern regex_pattern]");

    struct Options {
        enum class SubCmd {
            Root,
            List,
            Clean,
        };
        SubCmd subcmd;
        std::optional<std::regex> pattern;
        bool all;
        std::vector<std::string> files;
    };

    [[nodiscard]] std::optional<core::except::Error>
    clean(cache::Options&& opts) {
        namespace fs = boost::filesystem;
        if (opts.all) {
            fs::remove_all(io::path::poac_cache_dir);
        } else if (!opts.files.empty()) {
            for (const auto& f : opts.files) {
                const fs::path cache_package = io::path::poac_cache_dir / f;
                if (io::path::validate_dir(cache_package)) {
                    fs::remove_all(cache_package);
                    std::cout << cache_package << " is deleted" << std::endl;
                } else {
                    std::cout << termcolor2::red<> << cache_package << " not found"
                              << termcolor2::reset<> << std::endl;
                }
            }
        } else {
            return core::except::Error::InvalidSecondArg::Cache;
        }
        return std::nullopt;
    }

    [[nodiscard]] std::optional<core::except::Error>
    list(cache::Options&& opts) {
        namespace fs = boost::filesystem;
        if (opts.pattern) {
            for (const auto& e : boost::make_iterator_range(
                    fs::directory_iterator(io::path::poac_cache_dir), {})
            ) {
                const std::string cache_file = e.path().filename().string();
                if (std::regex_match(cache_file, opts.pattern.value()))
                    std::cout << cache_file << std::endl;
            }
        } else {
            for (const auto& e : boost::make_iterator_range(
                    fs::directory_iterator(io::path::poac_cache_dir), {})
            ) {
                std::cout << e.path().filename().string() << std::endl;
            }
        }
        return std::nullopt;
    }

    [[nodiscard]] std::optional<core::except::Error>
    root() {
        std::cout << io::path::poac_cache_dir.string() << std::endl;
        return std::nullopt;
    }

    [[nodiscard]] std::optional<core::except::Error>
    cache(cache::Options&& opts) {
        switch (opts.subcmd) {
            case cache::Options::SubCmd::Root:
                return root();
            case cache::Options::SubCmd::List:
                return list(std::move(opts));
            case cache::Options::SubCmd::Clean:
                return clean(std::move(opts));
        }
    }

    [[nodiscard]] std::optional<core::except::Error>
    exec(std::optional<io::yaml::Config>&&, std::vector<std::string>&& args) {
        if (args.empty()) {
            return core::except::Error::InvalidSecondArg::Cache;
        }

        cache::Options opts{};
        if (args[0] == "root" && args.size() == 1) {
            opts.subcmd = cache::Options::SubCmd::Root;
        } else if (args[0] == "list") {
            opts.subcmd = cache::Options::SubCmd::List;
            opts.pattern = util::argparse::use_get(args, "--pattern");
        } else if (args[0] == "clean") {
            opts.subcmd = cache::Options::SubCmd::Clean;
            opts.all = util::argparse::use(args, "-a", "--all");
            opts.files = std::vector<std::string>(args.begin() + 1, args.begin() + args.size());
        } else {
            return core::except::Error::InvalidSecondArg::Cache;
        }
        return cache::cache(std::move(opts));
    }
} // end namespace
#endif // !POAC_OPTS_CACHE_HPP

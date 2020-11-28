#ifndef POAC_IO_TERM_HPP
#define POAC_IO_TERM_HPP

#include <algorithm>
#include <filesystem>
#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include <string_view>
#include <optional>

#include <poac/core/except.hpp>
#include <poac/io/config.hpp>
#include <poac/io/path.hpp>
#include <poac/util/pretty.hpp>
#include <poac/util/termcolor2/termcolor2.hpp>

namespace poac::io::term {
    // Clear screen
    inline constexpr auto clr_screen = termcolor2::make_string("\x1b[2J");
    // Clear from the cursor position to the right end
    inline constexpr auto clr_right = termcolor2::make_string("\x1b[0K");
    // Clear from the cursor position to the left end
    inline constexpr auto clr_left = termcolor2::make_string("\x1b[1K");
    // Clear the line at the cursor position
    inline constexpr auto clr_line = termcolor2::make_string("\x1b[2K");

    inline void set_left(const int n) {
        std::cout << std::setw(n) << std::left;
    }

    inline constexpr auto bold_green = termcolor2::bold + termcolor2::green;

    inline constexpr auto status = bold_green + "==> " + termcolor2::reset;
    inline constexpr auto fetched = termcolor2::green + "  ●  " + termcolor2::reset;
    inline constexpr auto fetch_failed = termcolor2::red + "  ●  " + termcolor2::reset;

    inline constexpr auto warning = termcolor2::yellow + "WARN: " + termcolor2::reset;
    inline constexpr auto error = termcolor2::bold + termcolor2::red + "error: " + termcolor2::reset;
    inline constexpr auto info = termcolor2::blue + "info: " + termcolor2::reset;
    inline constexpr auto debug_m = termcolor2::gray + "[debug] " + termcolor2::reset;

    inline void status_done() {
        std::cout << status << "Done." << std::endl;
    }

    [[nodiscard]] std::optional<core::except::Error>
    yes_or_no(std::string_view question) {
        std::cout << question << " [Y/n] ";
        std::string y_n;
        std::cin >> y_n;
        std::transform(y_n.begin(), y_n.end(), y_n.begin(), ::tolower);
        if (y_n == "yes" || y_n == "y") {
            return std::nullopt;
        } else {
            return core::except::Error::InterruptedByUser;
        }
    }

    template <typename CharT>
    inline std::basic_ostream<CharT>&
    status_opt(std::basic_ostream<CharT>& os) {
        int indent_size = 9;
        indent_size += termcolor2::bold_v<CharT>().size();
        indent_size += termcolor2::green_v<CharT>().size();
        indent_size += termcolor2::reset_v<CharT>().size();
        return (os << std::right << std::setw(indent_size));
    }

    inline std::ostream&
    compiling(std::ostream& os) {
        return (os << status_opt << bold_green + "Compiling " << termcolor2::reset);
    }
    inline void
    echo_compiling(const std::optional<io::config::Config>& config) {
        std::cout << compiling
                  << config->package.name
                  << " v" << config->package.version
                  << std::endl;
    }
    inline void
    echo_compiling(const std::optional<io::config::Config>& config, const std::filesystem::path& path) {
        std::cout << compiling
                  << config->package.name
                  << " v" << config->package.version
                  << " (" << std::filesystem::absolute(path).string() << ")"
                  << std::endl;
    }

    inline std::ostream&
    echo_status(std::ostream& os, const std::string& msg) {
        return (os << status_opt << bold_green.to_string() + msg << termcolor2::reset);
    }

    inline std::ostream&
    finished(std::ostream& os) {
        return echo_status(os, "Finished ");
    }

    inline std::ostream&
    running(std::ostream& os) {
        return echo_status(os, "Running ");
    }

    // Create progress bar, [====>   ]
    std::string to_progress(const int& max_count, int now_count) {
        if (now_count > max_count) {
            now_count = max_count;
        }

        const int bar_size = 50;
        const int percent = (now_count * 100) / max_count;
        const int bar_pos = percent / 2;

        std::string bar = "[";
        if (now_count == max_count) {
            for (int i = 0; i < (bar_size - 1); ++i) {
                bar += "=";
            }
            bar += ">]";
        } else if ((bar_pos - 1) > 0) {
            for (int i = 0; i < (bar_pos - 1); ++i) {
                bar += "=";
            }
            bar += ">";
            for (int i = 0; i < (bar_size - bar_pos); ++i) {
                bar += " ";
            }
            bar += "]";
        } else if (bar_pos == 1) {
            bar += ">";
            for (int i = 0; i < (bar_size - 1); ++i) {
                bar += " ";
            }
            bar += "]";
        } else {
            for (int i = 0; i < bar_size; ++i) {
                bar += " ";
            }
            bar += "]";
        }
        return bar;
    }
    // Print byte progress bar, [====>   ] 10.21B/21.28KB
    void echo_byte_progress(const int& max_count, const int& now_count) {
        const auto [ parsed_max_byte, max_byte_unit ] = util::pretty::to_byte(max_count);
        const auto [ parsed_now_byte, now_byte_unit ] = util::pretty::to_byte(now_count);
        std::cout << term::to_progress(max_count, now_count) << " ";
        std::cout << std::fixed;
        std::cout << std::setprecision(2) << parsed_now_byte << now_byte_unit << "/";
        std::cout << std::setprecision(2) << parsed_max_byte << max_byte_unit << std::flush;
    }

    template <typename... T>
    inline void debugln(const T&...
#ifdef DEBUG
    s) {
        ((std::cout << debug_m) << ... << s) << std::endl;
#else
    ) {
#endif
    }
    template <typename... T>
    inline void debug(const T &...
#ifdef DEBUG
    s) {
        ((std::cout << debug_m) << ... << s);
#else
    ) {
#endif
    }
} // end namespace
#endif // !POAC_IO_TERM_HPP

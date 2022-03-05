#ifndef SEMVER_INTERVAL_HPP
#define SEMVER_INTERVAL_HPP

// std
#include <string>
#include <string_view>
#include <regex>
#include <stdexcept>
#include <optional>
#include <variant>

// external
#include <fmt/core.h>

// internal
#include <poac/util/semver/version.hpp>
#include <poac/util/semver/comparison.hpp>
#include <poac/util/semver/exception.hpp>

namespace semver {
    inline constexpr std::string_view base_version_format{ "{}.{}.{}" }; // 1.2.3
    inline constexpr std::string_view version_format{ "{}.{}.{}{}{}" }; // 1.2.3-beta+11.22.33

    inline std::string make_pre_or_build(const char& prefix, std::smatch::const_reference match_item) {
        return match_item.matched ? (prefix + match_item.str()) : "";
    }

    template <std::size_t Start, std::size_t ... Is>
    inline constexpr std::index_sequence<(Start + Is)...>
    add_offset(std::index_sequence<Is...>) {
        return {};
    }
    template <std::size_t Start, std::size_t End>
    inline constexpr auto make_range() {
        return add_offset<Start>(std::make_index_sequence<End - Start + 1>{});
    }

    template <std::size_t Pre, std::size_t Build, std::size_t... Is>
    std::string make_version_impl(const std::smatch& match, std::index_sequence<Is...>) {
        if constexpr (Pre == 0 || Build == 0) {
            return fmt::format(base_version_format, match[Is].str()...);
        } else {
            return fmt::format(
                version_format, match[Is].str()...,
                make_pre_or_build('-', match[Pre]),
                make_pre_or_build('+', match[Build])
            );
        }
    }

    template <std::size_t Start, std::size_t End, std::size_t Pre, std::size_t Build>
    inline std::string make_version(const std::smatch& match) {
        static_assert(End - Start == 2, "make_version: range diff should be 2");
        return make_version_impl<Pre, Build>(match, make_range<Start, End>());
    }

    template <std::size_t Start, std::size_t End>
    std::string make_version(const std::smatch& match) {
        if constexpr (End - Start == 2) { // not include pre & build
            return make_version<Start, End, 0, 0>(match);
        } else if constexpr (End - Start == 4) {
            // pop back pre & build
            return make_version<Start, End - 2, End - 1, End>(match);
        } else {
            static_assert([]{ return false; }(), "make_version: range diff should be 2 or 4");
        }
    }

    class ExactVersion {
    private:
        const std::string version;

    public:
        explicit ExactVersion(std::string_view version) noexcept : version{ version } {}

        inline bool satisfies(std::string_view interval) const noexcept {
            return version == interval;
        }
    };

    class BoundedInterval {
    private:
        const std::string left_comp_op;
        const std::string right_comp_op;
        const std::string left_version;
        const std::string right_version;

        bool satisfies_impl(std::string_view v) const {
            if (left_comp_op == ">") {
                if (right_comp_op == "<") {
                    return (Version(v) > left_version) && (Version(v) < right_version);
                } else if (right_comp_op == "<=") {
                    return (Version(v) > left_version) && (Version(v) <= right_version);
                }
            } else if (left_comp_op == ">=") {
                if (right_comp_op == "<") {
                    return (Version(v) >= left_version) && (Version(v) < right_version);
                } else if (right_comp_op == "<=") {
                    return (Version(v) >= left_version) && (Version(v) <= right_version);
                }
            } else if (left_comp_op == "<") {
                if (right_comp_op == ">") {
                    return (Version(v) < left_version) && (Version(v) > right_version);
                } else if (right_comp_op == ">=") {
                    return (Version(v) < left_version) && (Version(v) >= right_version);
                }
            } else if (left_comp_op == "<=") {
                if (right_comp_op == ">") {
                    return (Version(v) <= left_version) && (Version(v) > right_version);
                } else if (right_comp_op == ">=") {
                    return (Version(v) <= left_version) && (Version(v) >= right_version);
                }
            }
            return false;
        }

        // e.g. `>0.1.3 and >=0.3.2`, `<0.1.3 and <0.3.2`
        std::optional<std::string>
        is_wasteful_comparison_operation() const { // TODO: noexcept
            if ((left_comp_op == "<" || left_comp_op == "<=")
                && (right_comp_op == "<" || right_comp_op == "<="))
            {
                // Prioritize the larger version
                if (Version(left_version) > right_version) {
                    return "Did you mean " + left_comp_op + left_version + " ?";
                } else {
                    return "Did you mean " + right_comp_op + right_version + " ?";
                }
            } else if ((left_comp_op == ">" || left_comp_op == ">=")
                       && (right_comp_op == ">" || right_comp_op == ">="))
            {
                // Prioritize the smaller version
                if (Version(left_version) < right_version) {
                    return "Did you mean " + left_comp_op + left_version + " ?";
                } else {
                    return "Did you mean " + right_comp_op + right_version + " ?";
                }
            }
            return std::nullopt;
        }

        // Check if it is bounded interval
        //  (If it is unbounded, throw error)
        // (1, 6) => open bounded interval => OK!
        // [1, 6] => closed bounded interval => OK!
        // [a, ∞) => closed unbounded interval => one_exp
        // (-∞, ∞) => closed unbounded interval => ERR!
        // e.g. <0.1.1 and >=0.3.2
        std::optional<std::string>
        is_bounded_interval() const { // TODO: noexcept
            if (Version(left_version) < right_version) {
                if ((left_comp_op == "<" || left_comp_op == "<=")
                    && (right_comp_op == ">" || right_comp_op == ">="))
                {
                    return "In this case of interval specification using `and` +\n"
                           " it is necessary to be a bounded interval.\n"
                           "Please specify as in the following example:\n"
                           "e.g. `" + right_comp_op + left_version + " and " +
                           left_comp_op + right_version + "`";
                }
            } else if (Version(left_version) > right_version) {
                if ((left_comp_op == ">" || left_comp_op == ">=")
                    && (right_comp_op == "<" || right_comp_op == "<="))
                {
                    return "In this case of interval specification using `and` +\n"
                           " it is necessary to be a bounded interval.\n"
                           "Please specify as in the following example:\n"
                           "e.g. `" + left_comp_op + right_version + " and " +
                           right_comp_op + left_version + "`";
                }
            }
            return std::nullopt;
        }

    public:
        explicit BoundedInterval(const std::smatch& match, std::string_view interval)
            : left_comp_op{ match[2].str() }
            , right_comp_op{ match[9].str() }
            , left_version{ make_version<3, 7>(match) }
            , right_version{ make_version<10, 14>(match) }
        {
            if (const auto error = is_wasteful_comparison_operation()) {
                throw redundant_interval_error(std::string(interval), error.value());
            }
            if (const auto error = is_bounded_interval()) {
                throw strange_interval_error(std::string(interval), error.value());
            }
        }

        bool satisfies(std::string_view version) const {
            return satisfies_impl(version);
        }
    };

    class ClosedUnboundedInterval {
    private:
        const std::string comp_op;
        const std::string version_str;

        // >2.3.0, 1.0.0, <=1.2.3-alpha, ...
        bool satisfies_impl(std::string_view v) const {
            if (comp_op == ">") {
                return Version(v) > version_str;
            } else if (comp_op == ">=") {
                return Version(v) >= version_str;
            } else if (comp_op == "<") {
                return Version(v) < version_str;
            } else if (comp_op == "<=") {
                return Version(v) <= version_str;
            }
            return false;
        }

    public:
        explicit ClosedUnboundedInterval(
            std::string_view comp_op, std::string_view version_str
        )
            : comp_op{ comp_op }, version_str{ version_str }
        {}

        bool satisfies(std::string_view version) const {
            return satisfies_impl(version);
        }
    };

    class Interval {
    private:
        using IntervalClass = std::variant<ExactVersion, BoundedInterval, ClosedUnboundedInterval>;

        const std::string interval;
        const IntervalClass interval_class;

        template <typename T, typename... U>
        IntervalClass make_interval_class(U&&... args) const {
            return IntervalClass{ std::in_place_type<T>, std::forward<U>(args)... };
        }

        bool interval_match(std::smatch& match, const std::string& re) const {
            return std::regex_match(interval, match, std::regex(re));
        }

        IntervalClass get_interval_class() const {
            std::smatch match;
            if (interval_match(match, CLOSED_UNBOUNDED_INTERVAL)) {
                const std::string comp_op = match[2].str();
                const std::string version_str = make_version<3, 5>(match);
                return comp_op.empty()
                     ? make_interval_class<ExactVersion>(version_str)
                     : make_interval_class<ClosedUnboundedInterval>(comp_op, version_str);
            } else if (interval_match(match, BOUNDED_INTERVAL)) {
                return BoundedInterval{ match, interval };
            }
            throw invalid_interval_error(
                interval,
                "Comparison operators:\n"
                "  >, >=, <, <=\n"
                "Logical operator:\n"
                "  and\n"
                "The following example is the meaning for equals:\n"
                "  example: \"1.2.0\""
            );
        }

    public:
        explicit Interval(std::string_view i)
            : interval(i), interval_class{ get_interval_class() }
        {}

        bool satisfies(std::string_view version) const {
            return std::visit([version = std::move(version)](const auto& i) {
                return i.satisfies(version);
            }, interval_class);
        }
    };
} // end namespace semver

#endif // !SEMVER_INTERVAL_HPP

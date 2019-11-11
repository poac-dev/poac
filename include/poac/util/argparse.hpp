#ifndef POAC_UTIL_ARGPARSE_HPP
#define POAC_UTIL_ARGPARSE_HPP

#include <type_traits>
#include <algorithm>
#include <utility>
#include <array>
#include <tuple>
#include <optional>

#include <poac/util/types.hpp>

namespace poac::util::argparse {
    template <typename SinglePassRange, typename... T>
    bool use(SinglePassRange& rng, T... args) {
        const auto first = std::cbegin(rng);
        const auto last = std::cend(rng);
        return ((std::find(first, last, args) != last) || ...);
    }

    // { arg1, arg2 }, arg2 -> { arg1 }
    template <typename SinglePassRange, typename... T>
    bool use_rm(SinglePassRange& rng, T... args) {
        const auto first = std::begin(rng);
        auto last = std::end(rng);
        bool found = false;
        for (const auto& a : types::tuple_to_array(std::tuple<T...>{ args... })) {
            if (const auto itr = std::find(first, last, a); itr != last) {
                rng.erase(itr);
                last = std::end(rng);
                found = true;
            }
        }
        return found;
    }

    // -o filename -> return filename
    template <typename SinglePassRange, typename T>
    std::optional<std::string>
    use_get(SinglePassRange& rng, T arg) {
        const auto first = std::cbegin(rng);
        const auto last = std::cend(rng);
        if (const auto result = std::find(first, last, arg); result != last) {
            return *(result + 1);
        }
        else {
            return std::nullopt;
        }
    }
    // -o filename OR --output filename -> return filename
    template <typename SinglePassRange, typename T>
    std::optional<std::string>
    use_get(SinglePassRange& rng, T arg1, T arg2) {
        const auto first = std::cbegin(rng);
        const auto last = std::cend(rng);
        if (const auto result1 = std::find(first, last, arg1); result1 != last) {
            return *(result1 + 1);
        }
        else if (const auto result2 = std::find(first, last, arg2); result2 != last) {
            return *(result2 + 1);
        }
        else {
            return std::nullopt;
        }
    }
} // end namespace
#endif // !POAC_UTIL_ARGPARSE_HPP

#ifndef POAC_UTIL_ARGPARSE_HPP
#define POAC_UTIL_ARGPARSE_HPP

#include <type_traits>
#include <algorithm>
#include <utility>
#include <array>
#include <tuple>

#include "./types.hpp"


namespace poac::util::argparse {
    template <class SinglePassRange, class... T>
    bool use(SinglePassRange& rng, T... args)
    {
        const auto first = std::begin(rng);
        const auto last = std::end(rng);
        return ((std::find(first, last, args) != last) || ...);
    }

    template <class SinglePassRange, class... T>
    bool use_rm(SinglePassRange& rng, T... args)
    {
        const auto first = std::begin(rng);
        const auto last = std::end(rng);
        bool found = false;
        for (const auto& a : types::tuple_to_array(std::tuple<T...>{ args... })) {
            if (const auto itr = std::find(first, last, a); itr != last) {
                rng.erase(itr);
                found = true;
            }
        }
        return found;
    }
} // end namespace
#endif // !POAC_UTIL_ARGPARSE_HPP

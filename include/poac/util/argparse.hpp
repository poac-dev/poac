#ifndef POAC_UTIL_ARGPARSE_HPP
#define POAC_UTIL_ARGPARSE_HPP

#include <type_traits>
#include <algorithm>


namespace poac::util::argparse {
    template <class SinglePassRange, class... T>
    bool use(SinglePassRange& rng, T... args)
    {
        const auto last = std::end(rng);
        return ((std::find(std::begin(rng), last, args) != last) || ...);
    }
} // end namespace
#endif // !POAC_UTIL_ARGPARSE_HPP

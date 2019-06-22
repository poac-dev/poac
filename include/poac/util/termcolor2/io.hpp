#ifndef TERMCOLOR2_IO_HPP
#define TERMCOLOR2_IO_HPP

#include <cstddef> // std::size_t
#include <ostream> // std::basic_ostream
#include <iterator> // std::ostreambuf_iterator
#include <ios> // std::ios_base

#include "./string.hpp"

namespace termcolor2 {
    template <typename CharT, std::size_t N, typename Traits, typename StreamTraits>
    inline std::basic_ostream<CharT, StreamTraits>&
    operator<<(std::basic_ostream<CharT, StreamTraits>& os,
               const basic_string<CharT, N, Traits>& str)
    {
#ifdef __APPLE__
        return std::__put_character_sequence(os, str.data(), str.size());
#elif defined(_MSC_VER)
        // MSVC doesn't define __ostream_insert
        using streamsize = decltype(os.precision());
        return os.write(str.data(), static_cast<streamsize>(str.size()));
#else
        return std::__ostream_insert(os, str.data(), str.size());
#endif
    }
} // end namespace termcolor2

#endif	// !TERMCOLOR2_IO_HPP

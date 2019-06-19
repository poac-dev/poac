#ifndef TERMCOLOR2_IO_HPP
#define TERMCOLOR2_IO_HPP

#include <cstddef> // std::size_t
#include <ostream> // std::basic_ostream
#include <iterator> // std::ostreambuf_iterator
#include <ios> // std::ios_base

#include "./string.hpp"

namespace termcolor2 {
    template<class CharT, std::size_t N, class Traits>
    inline std::basic_ostream<CharT, Traits>&
    operator<<(std::basic_ostream<CharT, Traits>& os,
               const basic_string<CharT, N, Traits>& str)
    {
#ifdef __APPLE__
        return _VSTD::__put_character_sequence(os, str.data(), str.size());
#else
        return std::__ostream_insert(os, str.data(), str.size());
#endif
    }
} // end namespace termcolor2

#endif	// !TERMCOLOR2_IO_HPP

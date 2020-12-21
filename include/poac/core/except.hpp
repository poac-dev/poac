#ifndef POAC_CORE_EXCEPT_HPP
#define POAC_CORE_EXCEPT_HPP

#include <string>
#include <string_view>
#include <stdexcept>

namespace poac::core::except {
    namespace detail {
        template <typename Arg>
        std::string to_string(const Arg& str) {
            return std::to_string(str);
        }
        template <>
        std::string to_string(const std::string& str) {
            return str;
        }
        std::string to_string(std::string_view str) {
            return std::string(str);
        }
        template <typename CharT, std::size_t N>
        std::string to_string(const CharT(&str)[N]) {
            return str;
        }
    }

    class error : public std::invalid_argument {
    public:
        explicit error(const std::string_view __s)
            : invalid_argument(std::string(__s)) {}
        template <typename... Args>
        explicit error(const Args&... __s)
            : invalid_argument(
                    (... + detail::to_string(__s))
              )
        {}

        error() = delete;
        error(const error&) = default;
        error& operator=(const error&) = default;
        error(error&&) = default;
        error& operator=(error&&) = default;
        ~error() noexcept override = default;
    };
} // end namespace
#endif // !POAC_CORE_EXCEPT_HPP

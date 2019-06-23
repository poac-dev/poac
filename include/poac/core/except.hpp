#ifndef POAC_CORE_EXCEPT_HPP
#define POAC_CORE_EXCEPT_HPP

#include <string>
#include <string_view>
#include <stdexcept>

namespace poac::core::except {
    namespace msg {
        std::string put_period(const std::string& str) {
            if (*(str.end()) != '.') {
                return str + ".";
            }
            return str;
        }

        std::string not_found(const std::string& str) {
            return put_period(str + " not found");
        }
        std::string does_not_exist(const std::string& str) {
            return put_period(str + " does not exist");
        }
        std::string key_does_not_exist(const std::string& str) {
            return put_period("Required key `" + str + "` does not exist in poac.yml");
        }

        std::string already_exist(const std::string& str) {
            return put_period(str + " already exist");
        }

        std::string could_not(const std::string& str) {
            return put_period("Could not " + str);
        }
        std::string could_not_load(const std::string& str) {
            return put_period(could_not("load " + str));
        }
        std::string could_not_read(const std::string& str) {
            return put_period(could_not("read " + str));
        }

        std::string please(const std::string& str) {
            return put_period("Please " + str);
        }
        std::string please_refer_docs(const std::string& str) {
            // str <- /en/getting_started.html
            return put_period(please("refer to https://doc.poac.pm" + str));
        }
        std::string please_exec(const std::string& str) {
            return put_period(please("Please execute " + str));
        }
    }

    class invalid_first_arg : public std::invalid_argument
    {
    public:
        explicit invalid_first_arg(const std::string& __s) : invalid_argument(__s) {}
        explicit invalid_first_arg(const char* __s)        : invalid_argument(__s) {}

        virtual ~invalid_first_arg() = default;
    };

    class invalid_second_arg : public std::invalid_argument
    {
    public:
        explicit invalid_second_arg(const std::string& __s) : invalid_argument(__s) {}
        explicit invalid_second_arg(const char* __s)        : invalid_argument(__s) {}

        virtual ~invalid_second_arg() = default;
    };

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

    class error : public std::invalid_argument
    {
    public:
        explicit error(const std::string& __s)
            : invalid_argument(__s) {}

        explicit error(const char* __s)
            : invalid_argument(__s) {}

        template <typename... Args>
        error(const Args&... __s)
            : invalid_argument(
                    (... + except::to_string(__s))
              )
        {}

        virtual ~error() = default;
    };
} // end namespace
#endif // !POAC_CORE_EXCEPT_HPP

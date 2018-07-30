#ifndef POAC_CORE_EXCEPT_HPP
#define POAC_CORE_EXCEPT_HPP

#include <string>
#include <stdexcept>


namespace poac::core::except {
    class invalid_first_arg : public std::invalid_argument
    {
    public:
        explicit invalid_first_arg(const std::string& __s) : invalid_argument(__s) {}
        explicit invalid_first_arg(const char* __s)        : invalid_argument(__s) {}

        virtual ~invalid_first_arg() {};
    };
    class invalid_second_arg : public std::invalid_argument
    {
    public:
        explicit invalid_second_arg(const std::string& __s) : invalid_argument(__s) {}
        explicit invalid_second_arg(const char* __s)        : invalid_argument(__s) {}

        virtual ~invalid_second_arg() {};
    };
} // end namespace
#endif // !POAC_CORE_EXCEPT_HPP
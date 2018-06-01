#ifndef POAC_CORE_EXCEPT_HPP
#define POAC_CORE_EXCEPT_HPP

#include <string>
#include <stdexcept>


namespace poac::core {
    class invalid_first_argument : public std::invalid_argument
    {
    public:
        explicit invalid_first_argument(const std::string& __s) : invalid_argument(__s) {}
        explicit invalid_first_argument(const char* __s)        : invalid_argument(__s) {}

        virtual ~invalid_first_argument() {};
    };
    class invalid_second_argument : public std::invalid_argument
    {
    public:
        explicit invalid_second_argument(const std::string& __s) : invalid_argument(__s) {}
        explicit invalid_second_argument(const char* __s)        : invalid_argument(__s) {}

        virtual ~invalid_second_argument() {};
    };
} // end namespace
#endif // !POAC_CORE_EXCEPT_HPP
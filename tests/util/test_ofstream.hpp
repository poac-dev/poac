#ifndef POAC_TESTS_SUPPORT_TEST_OFSTREAM_HPP
#define POAC_TESTS_SUPPORT_TEST_OFSTREAM_HPP

#include <fstream>
#include <string>
#include <filesystem>

namespace support {
    // A RAII wrapper for basic_ofstream
    template <typename CharT, typename Traits = std::char_traits<CharT>>
    class basic_test_ofstream : public std::basic_ofstream<CharT, Traits> {
    public:
        using char_type = CharT;
        using traits_type = Traits;
        using int_type = typename traits_type::int_type;
        using pos_type = typename traits_type::pos_type;
        using off_type = typename traits_type::off_type;

        explicit
        basic_test_ofstream(const char* s, std::ios_base::openmode mode = std::ios_base::out)
            : std::basic_ofstream<char_type, traits_type>(s, mode), filepath(s)
        {}
        explicit
        basic_test_ofstream(const std::string& s, std::ios_base::openmode mode = std::ios_base::out)
            : std::basic_ofstream<char_type, traits_type>(s, mode), filepath(s)
        {}
        explicit
        basic_test_ofstream(const std::filesystem::path& s, std::ios_base::openmode mode = std::ios_base::out)
            : std::basic_ofstream<char_type, traits_type>(s.string(), mode), filepath(s)
        {}

        ~basic_test_ofstream() {
            std::filesystem::remove(this->filepath);
        }

        basic_test_ofstream(const basic_test_ofstream&) = delete;
        basic_test_ofstream& operator=(const basic_test_ofstream&) = delete;

        basic_test_ofstream(basic_test_ofstream&& rhs) noexcept
            : std::basic_ofstream<char_type, traits_type>(std::move(rhs)) {
            this->__sb_(std::move(rhs.__sb_));
            this->set_rdbuf(&this->__sb_);
        }
        inline basic_test_ofstream&
        operator=(basic_test_ofstream&& rhs) noexcept {
            std::basic_ofstream<char_type, traits_type>::operator=(std::move(rhs));
            this->__sb_ = std::move(rhs.__sb_);
            return *this;
        }

    private:
        std::filesystem::path filepath;
    };

    // Aliasies
    using test_ofstream = basic_test_ofstream<char>;
    using test_wofstream = basic_test_ofstream<wchar_t>;
} // end namespace support

#endif // !POAC_TESTS_SUPPORT_TEST_OFSTREAM_HPP

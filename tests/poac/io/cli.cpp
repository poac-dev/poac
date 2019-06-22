#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN
#include <boost/test/unit_test.hpp>

#include <poac/io/cli.hpp>


// std::string basename(const std::string& name)
BOOST_AUTO_TEST_CASE( poac_io_cli_basename_test )
{
    namespace io = poac::io;
    using namespace poac::io::cli::color_literals;

    static_assert(io::cli::hoge == "hoge");
    std::cout << "hoge"_red << std::endl;

    static_assert("hoge"_red == "\x1b[31mhoge\x1b[0m");
    static_assert(L"hoge"_red == L"\x1b[31mhoge\x1b[0m");
    static_assert(u"hoge"_red == u"\x1b[31mhoge\x1b[0m");
    static_assert(U"hoge"_red == U"\x1b[31mhoge\x1b[0m");

//    static_assert("hoge"_red != L"\x1b[31mhoge\x1b[0m");
//    static_assert("hoge"_red != u"\x1b[31mhoge\x1b[0m");
//    static_assert("hoge"_red != U"\x1b[31mhoge\x1b[0m");
//
//    static_assert(L"hoge"_red != u"\x1b[31mhoge\x1b[0m");
//    static_assert(L"hoge"_red != U"\x1b[31mhoge\x1b[0m");
//
//    static_assert(u"hoge"_red != U"\x1b[31mhoge\x1b[0m");

    std::cout << "hoge" << std::endl;
//    static_assert((io::cli::hoge + "hh") == "hogehh");
//    BOOST_TEST( name::basename("user/repo") == "repo" );
//    BOOST_TEST( name::basename("repo") == "repo" );
}

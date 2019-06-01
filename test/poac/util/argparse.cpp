#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN
#include <boost/test/unit_test.hpp>

#include <string>
#include <vector>

#include <poac/util/argparse.hpp>


// bool use(SinglePassRange& rng, T... args)
BOOST_AUTO_TEST_CASE( poac_util_argparse_use_test )
{
    using poac::util::argparse::use;

    std::vector<std::string> temp{ "-h", "--help", "--flag" };
    BOOST_TEST( use(temp, "-h") );
    BOOST_TEST( use(temp, "-h", "--help") );
    BOOST_TEST( use(temp, "-h", "--help", "--no") );
    BOOST_TEST( !use(temp, "-n", "--no") );
}

// bool use_rm(SinglePassRange& rng, T... args)
BOOST_AUTO_TEST_CASE( poac_util_argparse_use_rm_test )
{
    using poac::util::argparse::use;
    using poac::util::argparse::use_rm;

    std::vector<std::string> temp{ "-h", "--help", "--flag" };
    BOOST_TEST( use_rm(temp, "-h") );
    BOOST_TEST( !use(temp, "-h") );
    BOOST_TEST( use(temp, "--help") );
    BOOST_TEST( use(temp, "--flag") );

    temp = { "-h", "--help", "--flag" };
    BOOST_TEST( use_rm(temp, "-h", "--help") );
    BOOST_TEST( !use(temp, "-h", "--help") );
    BOOST_TEST( use(temp, "--flag") );

    temp = { "-h", "--help", "--flag" };
    BOOST_TEST( !use_rm(temp, "-n", "--no") );
    BOOST_TEST( use(temp, "-h", "--help", "--flag") );
}

// 1. std::optional<std::string> use_get(SinglePassRange& rng, T arg)
// 2. std::optional<std::string> use_get(SinglePassRange& rng, T arg1, T arg2)
BOOST_AUTO_TEST_CASE( poac_util_argparse_use_get_test )
{
    using poac::util::argparse::use_get;

    std::vector<std::string> temp{ "-o", "output.o", "--flag" };

    auto res = use_get(temp, "-o"); // 1
    BOOST_TEST( static_cast<bool>(res) );
    BOOST_TEST( *res == "output.o" );

    res = use_get(temp, "-o", "--output"); // 2
    BOOST_TEST( static_cast<bool>(res) );
    BOOST_TEST( *res == "output.o" );

    temp = { "-h", "--help", "--flag" };
    res = use_get(temp, "-o", "--output");
    BOOST_TEST( !static_cast<bool>(res) );
}

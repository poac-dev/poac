#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN
#include <boost/test/unit_test.hpp>

#include <list>
#include <string>
#include <iostream>

#include <boost/algorithm/string.hpp>

#include <poac/util/command.hpp>
#include <poac/util/compiler.hpp>


// std::string to(const std::string& s)
BOOST_AUTO_TEST_CASE( poac_util_compiler_test1 )
{
    using namespace poac::util;

    BOOST_TEST( true );
}

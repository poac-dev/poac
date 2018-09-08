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

    command cmd("clang++ -std=c++17 -I/Users/matken/Dropbox/Documents/project/poacpm/poac/deps/boost/include -I/Users/matken/Dropbox/Documents/project/poacpm/poac/deps/curl/include -I/Users/matken/Dropbox/Documents/project/poacpm/poac/deps/yaml-cpp/include -M main.cpp");
    if (const auto ret = cmd.exec()) {
        std::string delim(" \n\\");
        std::list<std::string> list_string;
        boost::split(list_string, *ret, boost::is_any_of(delim), boost::algorithm::token_compress_on);

        list_string.erase(list_string.begin()); // main.o:
        list_string.erase(list_string.begin()); // main.cpp
        int count = 0;
        for (std::string s : list_string) {
            std::cout << s << std::endl;
            if (count++ > 10) break;
        }
    }

    BOOST_TEST( true );
}


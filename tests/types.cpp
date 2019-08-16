#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN
#include <boost/test/unit_test.hpp>

#include <array>
#include <string>
#include <vector>
#include <tuple>

#include <boost/property_tree/ptree.hpp>

#include <poac/util/types.hpp>


// 1. std::optional<std::size_t> index_of(const SinglePassRange& rng, const T& t)
// 2. inline auto index_of(InputIterator first, InputIterator last, const T& value)
BOOST_AUTO_TEST_CASE( poac_util_types_index_of_test )
{
    using poac::util::types::index_of;

    std::vector<std::string> test_case{"0", "1", "2"};
    auto res = index_of(test_case, "1"); // 1
    BOOST_CHECK( res.has_value() );
    BOOST_CHECK( res.value() == 1 );

    res = index_of(test_case.cbegin(), test_case.cend(), "0"); // 2
    BOOST_CHECK( res.has_value() );
    BOOST_CHECK( res.value() == 0 );
}

// bool duplicate(const SinglePassRange& rng)
BOOST_AUTO_TEST_CASE( poac_util_types_duplicate_test )
{
    using poac::util::types::duplicate;

    std::vector<std::string> test_case{"0", "0", "2"};
    BOOST_CHECK( duplicate(test_case) );

    test_case = {"0", "1", "2"};
    BOOST_CHECK( !duplicate(test_case) );
}

// 1. std::vector<T> ptree_to_vector(const U& pt, const K& key)
// 2. std::vector<T> ptree_to_vector(const U &pt)
BOOST_AUTO_TEST_CASE( poac_util_types_ptree_to_vector_test )
{
    using poac::util::types::ptree_to_vector;

    boost::property_tree::ptree pt;
    std::vector<std::string> test_case{ "0", "1", "2" };

    boost::property_tree::ptree children;
    {
        boost::property_tree::ptree child;
        child.put("", "0");
        children.push_back(std::make_pair("", child));
    }
    {
        boost::property_tree::ptree child;
        child.put("", "1");
        children.push_back(std::make_pair("", child));
    }
    {
        boost::property_tree::ptree child;
        child.put("", "2");
        children.push_back(std::make_pair("", child));
    }
    pt.add_child("data", children);

    BOOST_CHECK( ptree_to_vector<std::string>(pt, "data") == test_case ); // 1
    BOOST_CHECK( ptree_to_vector<std::string>(children) == test_case ); // 2
}

// auto tuple_to_array(Tuple&& tuple)
BOOST_AUTO_TEST_CASE( poac_util_types_tuple_to_array_test )
{
    using poac::util::types::tuple_to_array;
    using namespace std::literals::string_literals;

    std::array<std::string, 3> test_case{ "0", "1", "2" };
    const auto res = tuple_to_array(std::make_tuple("0"s, "1"s, "2"s));
    BOOST_CHECK( res == test_case );
}

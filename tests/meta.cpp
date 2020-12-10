#define BOOST_TEST_MAIN
#include <array>
#include <boost/property_tree/ptree.hpp>
#include <boost/test/included/unit_test.hpp>
#include <poac/util/meta.hpp>
#include <string>
#include <tuple>
#include <vector>

// 1. std::optional<std::size_t> index_of(const SinglePassRange& rng, const T& t)
// 2. inline auto index_of(InputIterator first, InputIterator last, const T& value)
BOOST_AUTO_TEST_CASE( poac_util_types_index_of_test )
{
    using poac::util::meta::index_of;

    std::vector<std::string> test_case{"0", "1", "2"};
    {
        const auto res = index_of(test_case, "1"); // 1
        BOOST_CHECK( res.has_value() );
        BOOST_CHECK( res.value() == 1 );
    }
    {
        const auto res = index_of(test_case, "10"); // 1
        BOOST_CHECK( !res.has_value() );
    }
    BOOST_CHECK( index_of(test_case.cbegin(), test_case.cend(), "0") == 0 ); // 2
}

// inline auto index_of_if(InputIterator first, InputIterator last, Predicate pred)
BOOST_AUTO_TEST_CASE( poac_util_types_index_of_if_test )
{
    using poac::util::meta::index_of_if;

    std::vector<std::string> test_case{"0", "1", "2"};
    BOOST_CHECK(
        index_of_if(
            test_case.cbegin(),
            test_case.cend(),
            [](auto& x){ return x == "0"; }
        ) == 0
    );
}

// bool duplicate(const SinglePassRange& rng)
BOOST_AUTO_TEST_CASE( poac_util_types_duplicate_test )
{
    using poac::util::meta::duplicate;

    std::vector<std::string> test_case{"0", "0", "2"};
    BOOST_CHECK( duplicate(test_case) );

    test_case = {"0", "1", "2"};
    BOOST_CHECK( !duplicate(test_case) );
}

// 1. std::vector<T> ptree_to_vector(const U& pt, const K& key)
// 2. std::vector<T> ptree_to_vector(const U &pt)
BOOST_AUTO_TEST_CASE( poac_util_types_ptree_to_vector_test )
{
    using poac::util::meta::to_vector;

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

    BOOST_CHECK(to_vector<std::string>(pt, "data") == test_case ); // 1
    BOOST_CHECK(to_vector<std::string>(children) == test_case ); // 2
}

BOOST_AUTO_TEST_CASE( poac_util_meta_are_all_same_test )
{
    using poac::util::meta::are_all_same;
    using poac::util::meta::are_all_same_v;

    static_assert(are_all_same<int, int, int>::value);
    static_assert(are_all_same_v<int, int, int>);
    static_assert(std::negation_v<are_all_same<int, std::string, int>>);
    static_assert(std::negation_v<are_all_same<std::string, int, int>>);
    static_assert(std::negation_v<are_all_same<int, int, std::string>>);
}

BOOST_AUTO_TEST_CASE( poac_util_meta_is_specialization_test )
{
    using poac::util::meta::is_specialization;

    static_assert(is_specialization<std::vector<int>, std::vector>::value);
    static_assert(is_specialization<std::map<int, int>, std::map>::value);
    static_assert(is_specialization<std::map<int, std::vector<int>>, std::map>::value);
    static_assert(std::negation_v<is_specialization<std::map<int, std::vector<int>>, std::vector>>);
}

BOOST_AUTO_TEST_CASE( poac_util_meta_is_tuple_test )
{
    using poac::util::meta::is_tuple;
    using poac::util::meta::is_tuple_v;

    static_assert(is_tuple_v<std::tuple<int>>);
    static_assert(is_tuple_v<std::tuple<int, std::string>>);
    static_assert(std::negation_v<is_tuple<std::vector<int>>>);
}

BOOST_AUTO_TEST_CASE( poac_util_meta_to_array_test )
{
    using poac::util::meta::to_array;

    constexpr std::array<int, 3> test_case{
        0, 1, 2
    };
    constexpr std::tuple<int, int, int> res1 = std::make_tuple(0, 1, 2);
    constexpr std::array<int, 3> res = to_array(res1);
    BOOST_CHECK( res == test_case );
}

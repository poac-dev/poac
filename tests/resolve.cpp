#define BOOST_TEST_MAIN
#include <boost/test/included/unit_test.hpp>
#include <poac/core/resolver/resolve.hpp>

BOOST_AUTO_TEST_CASE( poac_core_resolver_resolve_to_binary_numbers_test )
{
    using poac::core::resolver::resolve::to_binary_numbers;

    BOOST_TEST( to_binary_numbers(0, 5)  == "00000" );
    BOOST_TEST( to_binary_numbers(1, 5)  == "00001" );
    BOOST_TEST( to_binary_numbers(2, 5)  == "00010" );
    BOOST_TEST( to_binary_numbers(3, 5)  == "00011" );
    BOOST_TEST( to_binary_numbers(4, 5)  == "00100" );
    BOOST_TEST( to_binary_numbers(5, 5)  == "00101" );
    BOOST_TEST( to_binary_numbers(6, 5)  == "00110" );
    BOOST_TEST( to_binary_numbers(7, 5)  == "00111" );
    BOOST_TEST( to_binary_numbers(8, 5)  == "01000" );
    BOOST_TEST( to_binary_numbers(9, 5)  == "01001" );
    BOOST_TEST( to_binary_numbers(10, 5) == "01010" );
    BOOST_TEST( to_binary_numbers(11, 5) == "01011" );
    BOOST_TEST( to_binary_numbers(12, 5) == "01100" );
    BOOST_TEST( to_binary_numbers(13, 5) == "01101" );
    BOOST_TEST( to_binary_numbers(14, 5) == "01110" );
    BOOST_TEST( to_binary_numbers(15, 5) == "01111" );
    BOOST_TEST( to_binary_numbers(16, 5) == "10000" );
    BOOST_TEST( to_binary_numbers(0, 10)  == "0000000000" );
    BOOST_TEST( to_binary_numbers(1, 10)  == "0000000001" );
    BOOST_TEST( to_binary_numbers(2, 10)  == "0000000010" );
    BOOST_TEST( to_binary_numbers(3, 10)  == "0000000011" );
    BOOST_TEST( to_binary_numbers(4, 10)  == "0000000100" );
    BOOST_TEST( to_binary_numbers(5, 10)  == "0000000101" );
    BOOST_TEST( to_binary_numbers(6, 10)  == "0000000110" );
    BOOST_TEST( to_binary_numbers(7, 10)  == "0000000111" );
    BOOST_TEST( to_binary_numbers(8, 10)  == "0000001000" );
    BOOST_TEST( to_binary_numbers(9, 10)  == "0000001001" );
    BOOST_TEST( to_binary_numbers(10, 10) == "0000001010" );
    BOOST_TEST( to_binary_numbers(11, 10) == "0000001011" );
    BOOST_TEST( to_binary_numbers(12, 10) == "0000001100" );
    BOOST_TEST( to_binary_numbers(13, 10) == "0000001101" );
    BOOST_TEST( to_binary_numbers(14, 10) == "0000001110" );
    BOOST_TEST( to_binary_numbers(15, 10) == "0000001111" );
    BOOST_TEST( to_binary_numbers(16, 10) == "0000010000" );
}

BOOST_AUTO_TEST_CASE( poac_core_resolver_test1 )
{ // TODO
//    using namespace poac::core::resolver::resolve;
//
//    DuplicateDeps test{};
//    test.emplace_back("D", poac::io::lockfile::Lockfile::Package{"1.0.0" });
//    test.emplace_back("D", poac::io::lockfile::Lockfile::Package{"1.1.0" });
//    test.emplace_back("E", poac::io::lockfile::Lockfile::Package{"1.0.0" });
//    test.emplace_back("A", poac::io::lockfile::Lockfile::Package{"1.0.0" });
//    test.emplace_back(
//            { {"B"}, {"1.0.0"}, {""}, {{
//            { {"D"}, {"1.0.0"}, {""}, {} },
//            { {"D"}, {"1.1.0"}, {""}, {} },
//            { {"E"}, {"1.0.0"}, {""}, {} }
//    }} });
//    test.emplace_back({ {"C"}, {"1.0.0"}, {""}, {{
//            { {"D"}, {"1.1.0"}, {""}, {} }
//    }} });
//
//    NoDuplicateDeps backtracked;
//    backtracked["A"] = { "1.0.0", "" };
//    backtracked["B"] = { "1.0.0", "" };
//    backtracked["C"] = { "1.0.0", "" };
//    backtracked["D"] = { "1.1.0", "" };
//    backtracked["E"] = { "1.0.0", "" };
//
//    const ResolvedDeps result = backtrack_loop(test);
//    BOOST_CHECK( result.backtracked == backtracked );
}

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN
#include <boost/test/unit_test.hpp>
#include <poac/core/resolver/resolve.hpp>

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

#ifndef POAC_TESTS_SUPPORT_MACROS_HPP
#define POAC_TESTS_SUPPORT_MACROS_HPP

#include <boost/test/tools/old/interface.hpp>

// In macros below following argument abbreviations are used:
// M - message
// S - statement
// E - exception
#define BOOST_CHECK_THROW_MSG( S, E, M )               \
do {                                                   \
    BOOST_CHECK_THROW( S, E );                         \
    try {                                              \
        S;                                             \
    } catch (const E& e) {                             \
        BOOST_CHECK( std::string(e.what()) == M );     \
    }                                                  \
} while ( boost::test_tools::tt_detail::dummy_cond() ) \

#endif // !POAC_TESTS_SUPPORT_MACROS_HPP

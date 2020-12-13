#define BOOST_TEST_MAIN
#include <boost/test/included/unit_test.hpp>

#include <poac/core/validator.hpp>

BOOST_AUTO_TEST_CASE( poac_core_validator_use_valid_characters_test )
{
    using poac::core::validator::use_valid_characters;

    BOOST_CHECK( use_valid_characters("na$me").is_err() );
    BOOST_CHECK( use_valid_characters("nam()e").is_err() );
    BOOST_CHECK( use_valid_characters("namße").is_err() );

    BOOST_CHECK( !use_valid_characters("poacpm/poac-api").is_err() );
    BOOST_CHECK( !use_valid_characters("poacpm/poac_api").is_err() );
    BOOST_CHECK( !use_valid_characters("poacpm/poac").is_err() );
}

#define BOOST_TEST_MAIN
#include <boost/test/included/unit_test.hpp>

#include <poac/core/validator.hpp>

BOOST_AUTO_TEST_CASE( poac_core_validator_use_valid_characters_test )
{
    using poac::core::validator::invalid_characters;

    BOOST_CHECK( invalid_characters("na$me").is_err() );
    BOOST_CHECK( invalid_characters("nam()e").is_err() );
    BOOST_CHECK( invalid_characters("namße").is_err() );

    BOOST_CHECK( !invalid_characters("poacpm/poac-api").is_err() );
    BOOST_CHECK( !invalid_characters("poacpm/poac_api").is_err() );
    BOOST_CHECK( !invalid_characters("poacpm/poac").is_err() );

    BOOST_CHECK( invalid_characters("double//slashes").is_err() );
    BOOST_CHECK( invalid_characters("double--hyphens").is_ok() );
    BOOST_CHECK( invalid_characters("double__underscores").is_ok() );

    BOOST_CHECK( invalid_characters("many////////////slashes").is_err() );
    BOOST_CHECK( invalid_characters("many------------hyphens").is_ok() );
    BOOST_CHECK( invalid_characters("many________underscores").is_ok() );

    BOOST_CHECK( invalid_characters("/startWithSlash").is_err() );
    BOOST_CHECK( invalid_characters("-startWithHyphen").is_err() );
    BOOST_CHECK( invalid_characters("_startWithUnderscore").is_err() );

    BOOST_CHECK( invalid_characters("endWithSlash/").is_err() );
    BOOST_CHECK( invalid_characters("endWithHyphen-").is_err() );
    BOOST_CHECK( invalid_characters("endWithUnderscore_").is_err() );
}

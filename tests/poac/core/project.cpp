#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN
#include <boost/test/unit_test.hpp>

#include <poac/core/project.hpp>

// std::optional<std::string_view> extract_str(std::string_view target, std::string_view prefix, std::string_view suffix)
BOOST_AUTO_TEST_CASE( poac_core_project_extract_str_test )
{
    using poac::core::project::extract_str;

    {
        std::string_view target = "https://github.com/poacpm/poac.git";
        BOOST_CHECK( extract_str(target, "https://github.com/", ".git").value() == "poacpm/poac" );
    }
    {
        std::string_view target = "git@github.com:poacpm/poac.git";
        BOOST_CHECK( !extract_str(target, "https://github.com/", ".git").has_value() );
    }
    {
        std::string_view target = "git@github.com:poacpm/poac.git";
        BOOST_CHECK( extract_str(target, "git@github.com:", ".git").value() == "poacpm/poac" );
    }
}

// std::string_view extract_full_name(std::string_view repository)
BOOST_AUTO_TEST_CASE( poac_core_project_extract_full_name_test )
{
    using poac::core::project::extract_full_name;

    {
        std::string_view repo = "https://github.com/poacpm/poac.git";
        BOOST_CHECK( extract_full_name(repo) == "poacpm/poac" );
    }
    {
        std::string_view repo = "github.com/poacpm/poac.git";
        BOOST_CHECK_THROW(
                extract_full_name(repo),
                poac::core::except::error
        );
    }
    {
        std::string_view repo = "git@github.com:poacpm/poac.git";
        BOOST_CHECK( extract_full_name(repo) == "poacpm/poac" );
    }
}

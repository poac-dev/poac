#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN
#include <boost/test/unit_test.hpp>
#include <boost/test/output_test_stream.hpp>
#include <poac/util/command.hpp>


// command operator&&(const command& rhs)
BOOST_AUTO_TEST_CASE( poac_util_command_test1 )
{
    using namespace poac::util;

    command cmd("mkdir test");
    command cmd2 = (cmd && command("cd test"));

    BOOST_TEST( cmd2.string() == "mkdir test && cd test" );
}

// command operator&&(const std::string& rhs)
BOOST_AUTO_TEST_CASE( poac_util_command_test2 )
{
    using namespace poac::util;

    const command cmd("mkdir test");
    const command cmd2 = (cmd && "cd test");

    BOOST_TEST( cmd2.string() == "mkdir test && cd test" );
}

// command operator&=(const command& rhs)
BOOST_AUTO_TEST_CASE( poac_util_command_test3 )
{
    using namespace poac::util;

    command cmd("mkdir test");
    cmd &= command("cd test");

    BOOST_TEST( cmd.string() == "mkdir test && cd test" );
}

// command operator&=(const std::string& rhs)
BOOST_AUTO_TEST_CASE( poac_util_command_test4 )
{
    using namespace poac::util;

    command cmd("mkdir test");
    cmd &= "cd test";

    BOOST_TEST( cmd.string() == "mkdir test && cd test" );
}

// bool operator==(const command& rhs)
BOOST_AUTO_TEST_CASE( poac_util_command_test5 )
{
    using namespace poac::util;

    command cmd("mkdir test");
    cmd &= "cd test";

    BOOST_TEST( cmd == command("mkdir test && cd test") );
}

// bool operator==(const std::string& rhs)
BOOST_AUTO_TEST_CASE( poac_util_command_test6 )
{
    using namespace poac::util;

    command cmd("mkdir test");
    cmd &= "cd test";

    BOOST_TEST( cmd == "mkdir test && cd test" );
}

// friend std::ostream& operator<<(std::ostream& stream, const command& c)
BOOST_AUTO_TEST_CASE( poac_util_command_test7 )
{
    using namespace poac::util;

    command cmd("mkdir test");
    cmd &= "cd test";

    boost::test_tools::output_test_stream output;
    output << cmd;

    BOOST_TEST( !output.is_empty( false ) );
    BOOST_TEST( output.is_equal( "mkdir test && cd test" ) );
}

// command env(const std::string& name, const std::string& val)
BOOST_AUTO_TEST_CASE( poac_util_command_test8 )
{
    using namespace poac::util;

    command cmd("cmake ..");
    cmd = cmd.env("OPENSSL_ROOT_DIR", "/usr/local/opt/openssl/");
    cmd = cmd.env("MACOSX_RPATH", "1");

    BOOST_TEST( cmd.string() == "MACOSX_RPATH=1 OPENSSL_ROOT_DIR=/usr/local/opt/openssl/ cmake .." );
}

// command stderr_to_stdout()
BOOST_AUTO_TEST_CASE( poac_util_command_test9 )
{
    using namespace poac::util;

    command cmd("cmake ..");
    cmd = cmd.env("OPENSSL_ROOT_DIR", "/usr/local/opt/openssl/");
    cmd = cmd.env("MACOSX_RPATH", "1");
    cmd = cmd.stderr_to_stdout();

    BOOST_TEST( cmd.string() == "MACOSX_RPATH=1 OPENSSL_ROOT_DIR=/usr/local/opt/openssl/ cmake .. 2>&1" );
}

// boost::optional<std::string> exec()
BOOST_AUTO_TEST_CASE( poac_util_command_test10 )
{
    using namespace poac::util;
    command cmd("echo test");
    BOOST_TEST( *(cmd.exec()) == "test\n" );
}

// boost::optional<std::string> exec()
BOOST_AUTO_TEST_CASE( util_command_test11 )
{
    using namespace poac::util;
    command cmd("nocmd");
    BOOST_TEST( !static_cast<bool>(cmd.exec()) );
}

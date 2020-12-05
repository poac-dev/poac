#define BOOST_TEST_MAIN
#include <boost/test/included/unit_test.hpp>
#include <boost/test/tools/output_test_stream.hpp>
#include <poac/util/shell.hpp>

// 1. shell::shell()
// 2. shell::shell(const std::string& c)
BOOST_AUTO_TEST_CASE( poac_util_shell_shell_test )
{
    using poac::util::cmd;
    BOOST_CHECK(cmd().string() == "" ); // 1
    BOOST_CHECK(cmd("cd").string() == "cd" ); // 2
    BOOST_CHECK(cmd("cd").string() == "cd" ); // 3
}

// shell env(const std::string& name, const std::string& val)
BOOST_AUTO_TEST_CASE( poac_util_shell_env_test )
{
    using poac::util::cmd;

    cmd c("cmake ..");
    c = c.env("OPENSSL_ROOT_DIR", "/usr/local/opt/openssl/");
    c = c.env("MACOSX_RPATH", "1");

    BOOST_CHECK( c.string() == "MACOSX_RPATH=1 OPENSSL_ROOT_DIR=/usr/local/opt/openssl/ cmake .." );
}

// shell stderr_to_stdout()
BOOST_AUTO_TEST_CASE( poac_util_shell_stderr_to_stdout_test )
{
    using poac::util::cmd;

    cmd c("cmake ..");
    c = c.stderr_to_stdout();
    BOOST_CHECK( c.string() == "cmake .. 2>&1" );
}

// shell to_dev_null()
BOOST_AUTO_TEST_CASE( poac_util_shell_to_dev_null_test )
{
    using poac::util::cmd;

    cmd c("cmake ..");
    c = c.to_dev_null();
    BOOST_CHECK( c.string() == "cmake .. >/dev/null" );
}

// boost::optional<std::string> exec()
BOOST_AUTO_TEST_CASE( poac_util_shell_exec_test )
{
    using poac::util::cmd;
    {
        cmd c("echo test");
        BOOST_CHECK( c.exec().value() == "test\n" );
    }
    {
        cmd c("nocmd");
        BOOST_CHECK( !c.exec().has_value() );
    }
}

// bool exec_incontinent()
BOOST_AUTO_TEST_CASE( poac_util_shell_exec_ignore_test )
{
    using poac::util::cmd;
    cmd c("cd");
    BOOST_CHECK( c.exec_ignore() );
}

// friend std::ostream& operator<<(std::ostream& stream, const shell& c)
BOOST_AUTO_TEST_CASE( poac_util_shell_op_test1 )
{
    using poac::util::cmd;

    cmd c("mkdir test");
    c &= "cd test";

    boost::test_tools::output_test_stream output;
    output << c;

    BOOST_CHECK( !output.is_empty(false) );
    BOOST_CHECK( output.is_equal("mkdir test && cd test") );
}

// bool operator==(const shell& rhs)
BOOST_AUTO_TEST_CASE( poac_util_shell_op_test2 )
{
    using poac::util::cmd;

    cmd c("mkdir test");
    c &= "cd test";

    BOOST_CHECK( c == cmd("mkdir test && cd test") );
}
// bool operator==(const std::string& rhs)
BOOST_AUTO_TEST_CASE( poac_util_shell_op_test3 )
{
    using poac::util::cmd;

    cmd c("mkdir test");
    c &= "cd test";

    BOOST_CHECK( c == "mkdir test && cd test" );
}

// shell operator&&(const shell& rhs)
BOOST_AUTO_TEST_CASE( poac_util_shell_op_test4 )
{
    using poac::util::cmd;

    cmd c("mkdir test");
    cmd c2 = (c && cmd("cd test"));

    BOOST_CHECK( c2.string() == "mkdir test && cd test" );
}
// shell operator&&(const std::string& rhs)
BOOST_AUTO_TEST_CASE( poac_util_shell_op_test5 )
{
    using poac::util::cmd;

    const cmd c("mkdir test");
    const cmd c2 = (c && "cd test");

    BOOST_CHECK( c2.string() == "mkdir test && cd test" );
}

// shell operator&=(const shell& rhs)
BOOST_AUTO_TEST_CASE( poac_util_shell_op_test6 )
{
    using poac::util::cmd;

    cmd c("mkdir test");
    c &= cmd("cd test");

    BOOST_CHECK( c.string() == "mkdir test && cd test" );
}
// shell operator&=(const std::string& rhs)
BOOST_AUTO_TEST_CASE( poac_util_shell_op_test7 )
{
    using poac::util::cmd;

    cmd c("mkdir test");
    c &= "cd test";

    BOOST_CHECK( c.string() == "mkdir test && cd test" );
}

// shell operator||(const shell& rhs)
BOOST_AUTO_TEST_CASE( poac_util_shell_op_test8 )
{
    using poac::util::cmd;

    cmd c("mkdir test");
    cmd c2 = (c || cmd("cd test"));

    BOOST_CHECK( c2.string() == "mkdir test || cd test" );
}
// shell operator||(const std::string& rhs)
BOOST_AUTO_TEST_CASE( poac_util_shell_op_test9 )
{
    using poac::util::cmd;

    const cmd c("mkdir test");
    const cmd c2 = (c || "cd test");

    BOOST_CHECK( c2.string() == "mkdir test || cd test" );
}

// shell operator|=(const shell& rhs)
BOOST_AUTO_TEST_CASE( poac_util_shell_op_test10 )
{
    using poac::util::cmd;

    cmd c("mkdir test");
    c |= cmd("cd test");

    BOOST_CHECK( c.string() == "mkdir test || cd test" );
}
// shell operator|=(const std::string& rhs)
BOOST_AUTO_TEST_CASE( poac_util_shell_op_test11 )
{
    using poac::util::cmd;

    cmd c("mkdir test");
    c |= "cd test";

    BOOST_CHECK( c.string() == "mkdir test || cd test" );
}

// shell operator+(const shell& rhs)
BOOST_AUTO_TEST_CASE( poac_util_shell_op_test12 )
{
    using poac::util::cmd;

    cmd c("mkdir test");
    cmd c2 = (c + cmd("cd test"));

    BOOST_CHECK( c2.string() == "mkdir test cd test" );
}
// shell operator+(const std::string& rhs)
BOOST_AUTO_TEST_CASE( poac_util_shell_op_test13 )
{
    using poac::util::cmd;

    const cmd c("mkdir test");
    const cmd c2 = (c + "cd test");

    BOOST_CHECK( c2.string() == "mkdir test cd test" );
}

// shell operator+=(const shell& rhs)
BOOST_AUTO_TEST_CASE( poac_util_shell_op_test14 )
{
    using poac::util::cmd;

    cmd c("mkdir test");
    c += cmd("cd test");

    BOOST_CHECK( c.string() == "mkdir test cd test" );
}
// shell operator+=(const std::string& rhs)
BOOST_AUTO_TEST_CASE( poac_util_shell_op_test15 )
{
    using poac::util::cmd;

    cmd c("mkdir test");
    c += "cd test";

    BOOST_CHECK( c.string() == "mkdir test cd test" );
}

// bool has_shell(const std::string& c)
BOOST_AUTO_TEST_CASE( poac_util_shell_has_command_test )
{
    using poac::util::_shell::has_command;
    BOOST_CHECK( has_command("cd") );
}

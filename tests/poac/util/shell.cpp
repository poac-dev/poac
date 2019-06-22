#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN
#include <boost/test/unit_test.hpp>
#include <boost/test/output_test_stream.hpp>

#include <cstdlib>

#include <poac/util/shell.hpp>


// 1. shell::shell()
// 2. shell::shell(const std::string& c)
BOOST_AUTO_TEST_CASE( poac_util_shell_shell_test )
{
    using poac::util::shell;
    BOOST_TEST( shell().string() == "" ); // 1
    BOOST_TEST( shell("cd").string() == "cd" ); // 2
    BOOST_TEST( shell("cd").string() == "cd" ); // 3
}

// shell env(const std::string& name, const std::string& val)
BOOST_AUTO_TEST_CASE( poac_util_shell_env_test )
{
    using poac::util::shell;

    shell cmd("cmake ..");
    cmd = cmd.env("OPENSSL_ROOT_DIR", "/usr/local/opt/openssl/");
    cmd = cmd.env("MACOSX_RPATH", "1");

    BOOST_TEST( cmd.string() == "MACOSX_RPATH=1 OPENSSL_ROOT_DIR=/usr/local/opt/openssl/ cmake .." );
}

// shell stderr_to_stdout()
BOOST_AUTO_TEST_CASE( poac_util_shell_stderr_to_stdout_test )
{
    using poac::util::shell;

    shell cmd("cmake ..");
    cmd = cmd.stderr_to_stdout();
    BOOST_TEST( cmd.string() == "cmake .. 2>&1" );
}

// shell to_dev_null()
BOOST_AUTO_TEST_CASE( poac_util_shell_to_dev_null_test )
{
    using poac::util::shell;

    shell cmd("cmake ..");
    cmd = cmd.to_dev_null();
    BOOST_TEST( cmd.string() == "cmake .. >/dev/null" );
}

// boost::optional<std::string> exec()
BOOST_AUTO_TEST_CASE( poac_util_shell_exec_test )
{
    using poac::util::shell;
    {
        shell cmd("echo test");
        BOOST_TEST(*(cmd.exec()) == "test\n");
    }
    {
        shell cmd("nocmd");
        BOOST_TEST(!static_cast<bool>(cmd.exec()));
    }
}

// bool exec_incontinent()
BOOST_AUTO_TEST_CASE( poac_util_shell_exec_incontinent_test )
{
    using poac::util::shell;
    shell cmd("cd");
    BOOST_TEST( cmd.exec_incontinent() == EXIT_SUCCESS );
}

// friend std::ostream& operator<<(std::ostream& stream, const shell& c)
BOOST_AUTO_TEST_CASE( poac_util_shell_op_test1 )
{
    using poac::util::shell;

    shell cmd("mkdir test");
    cmd &= "cd test";

    boost::test_tools::output_test_stream output;
    output << cmd;

    BOOST_TEST( !output.is_empty( false ) );
    BOOST_TEST( output.is_equal( "mkdir test && cd test" ) );
}

// bool operator==(const shell& rhs)
BOOST_AUTO_TEST_CASE( poac_util_shell_op_test2 )
{
    using poac::util::shell;

    shell cmd("mkdir test");
    cmd &= "cd test";

    BOOST_TEST( cmd == shell("mkdir test && cd test") );
}
// bool operator==(const std::string& rhs)
BOOST_AUTO_TEST_CASE( poac_util_shell_op_test3 )
{
    using poac::util::shell;

    shell cmd("mkdir test");
    cmd &= "cd test";

    BOOST_TEST( cmd == "mkdir test && cd test" );
}

// shell operator&&(const shell& rhs)
BOOST_AUTO_TEST_CASE( poac_util_shell_op_test4 )
{
    using poac::util::shell;

    shell cmd("mkdir test");
    shell cmd2 = (cmd && shell("cd test"));

    BOOST_TEST( cmd2.string() == "mkdir test && cd test" );
}
// shell operator&&(const std::string& rhs)
BOOST_AUTO_TEST_CASE( poac_util_shell_op_test5 )
{
    using poac::util::shell;

    const shell cmd("mkdir test");
    const shell cmd2 = (cmd && "cd test");

    BOOST_TEST( cmd2.string() == "mkdir test && cd test" );
}

// shell operator&=(const shell& rhs)
BOOST_AUTO_TEST_CASE( poac_util_shell_op_test6 )
{
    using poac::util::shell;

    shell cmd("mkdir test");
    cmd &= shell("cd test");

    BOOST_TEST( cmd.string() == "mkdir test && cd test" );
}
// shell operator&=(const std::string& rhs)
BOOST_AUTO_TEST_CASE( poac_util_shell_op_test7 )
{
    using poac::util::shell;

    shell cmd("mkdir test");
    cmd &= "cd test";

    BOOST_TEST( cmd.string() == "mkdir test && cd test" );
}

// shell operator||(const shell& rhs)
BOOST_AUTO_TEST_CASE( poac_util_shell_op_test8 )
{
    using poac::util::shell;

    shell cmd("mkdir test");
    shell cmd2 = (cmd || shell("cd test"));

    BOOST_TEST( cmd2.string() == "mkdir test || cd test" );
}
// shell operator||(const std::string& rhs)
BOOST_AUTO_TEST_CASE( poac_util_shell_op_test9 )
{
    using poac::util::shell;

    const shell cmd("mkdir test");
    const shell cmd2 = (cmd || "cd test");

    BOOST_TEST( cmd2.string() == "mkdir test || cd test" );
}

// shell operator|=(const shell& rhs)
BOOST_AUTO_TEST_CASE( poac_util_shell_op_test10 )
{
    using poac::util::shell;

    shell cmd("mkdir test");
    cmd |= shell("cd test");

    BOOST_TEST( cmd.string() == "mkdir test || cd test" );
}
// shell operator|=(const std::string& rhs)
BOOST_AUTO_TEST_CASE( poac_util_shell_op_test11 )
{
    using poac::util::shell;

    shell cmd("mkdir test");
    cmd |= "cd test";

    BOOST_TEST( cmd.string() == "mkdir test || cd test" );
}

// shell operator+(const shell& rhs)
BOOST_AUTO_TEST_CASE( poac_util_shell_op_test12 )
{
    using poac::util::shell;

    shell cmd("mkdir test");
    shell cmd2 = (cmd + shell("cd test"));

    BOOST_TEST( cmd2.string() == "mkdir test cd test" );
}
// shell operator+(const std::string& rhs)
BOOST_AUTO_TEST_CASE( poac_util_shell_op_test13 )
{
    using poac::util::shell;

    const shell cmd("mkdir test");
    const shell cmd2 = (cmd + "cd test");

    BOOST_TEST( cmd2.string() == "mkdir test cd test" );
}

// shell operator+=(const shell& rhs)
BOOST_AUTO_TEST_CASE( poac_util_shell_op_test14 )
{
    using poac::util::shell;

    shell cmd("mkdir test");
    cmd += shell("cd test");

    BOOST_TEST( cmd.string() == "mkdir test cd test" );
}
// shell operator+=(const std::string& rhs)
BOOST_AUTO_TEST_CASE( poac_util_shell_op_test15 )
{
    using poac::util::shell;

    shell cmd("mkdir test");
    cmd += "cd test";

    BOOST_TEST( cmd.string() == "mkdir test cd test" );
}

// bool has_shell(const std::string& c)
BOOST_AUTO_TEST_CASE( poac_util_shell_has_command_test )
{
    using poac::util::_shell::has_command;
    BOOST_TEST( has_command("cd") );
}

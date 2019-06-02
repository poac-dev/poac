#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN
#include <boost/test/unit_test.hpp>
#include <boost/test/output_test_stream.hpp>

#include <cstdlib>

#include <poac/util/command.hpp>


// 1. command::command()
// 2. command::command(const std::string& c)
BOOST_AUTO_TEST_CASE( poac_util_command_command_test )
{
    using poac::util::command;
    BOOST_TEST( command().string() == "" ); // 1
    BOOST_TEST( command("cd").string() == "cd" ); // 2
    BOOST_TEST( command("cd").string() == "cd" ); // 3
}

// command env(const std::string& name, const std::string& val)
BOOST_AUTO_TEST_CASE( poac_util_command_env_test )
{
    using poac::util::command;

    command cmd("cmake ..");
    cmd = cmd.env("OPENSSL_ROOT_DIR", "/usr/local/opt/openssl/");
    cmd = cmd.env("MACOSX_RPATH", "1");

    BOOST_TEST( cmd.string() == "MACOSX_RPATH=1 OPENSSL_ROOT_DIR=/usr/local/opt/openssl/ cmake .." );
}

// command stderr_to_stdout()
BOOST_AUTO_TEST_CASE( poac_util_command_stderr_to_stdout_test )
{
    using poac::util::command;

    command cmd("cmake ..");
    cmd = cmd.stderr_to_stdout();
    BOOST_TEST( cmd.string() == "cmake .. 2>&1" );
}

// command to_dev_null()
BOOST_AUTO_TEST_CASE( poac_util_command_to_dev_null_test )
{
    using poac::util::command;

    command cmd("cmake ..");
    cmd = cmd.to_dev_null();
    BOOST_TEST( cmd.string() == "cmake .. >/dev/null" );
}

// boost::optional<std::string> exec()
BOOST_AUTO_TEST_CASE( poac_util_command_exec_test )
{
    using poac::util::command;
    {
        command cmd("echo test");
        BOOST_TEST(*(cmd.exec()) == "test\n");
    }
    {
        command cmd("nocmd");
        BOOST_TEST(!static_cast<bool>(cmd.exec()));
    }
}

// bool exec_incontinent()
BOOST_AUTO_TEST_CASE( poac_util_command_exec_incontinent_test )
{
    using poac::util::command;
    command cmd("cd");
    BOOST_TEST( cmd.exec_incontinent() == EXIT_SUCCESS );
}

// friend std::ostream& operator<<(std::ostream& stream, const command& c)
BOOST_AUTO_TEST_CASE( poac_util_command_op_test1 )
{
    using poac::util::command;

    command cmd("mkdir test");
    cmd &= "cd test";

    boost::test_tools::output_test_stream output;
    output << cmd;

    BOOST_TEST( !output.is_empty( false ) );
    BOOST_TEST( output.is_equal( "mkdir test && cd test" ) );
}

// bool operator==(const command& rhs)
BOOST_AUTO_TEST_CASE( poac_util_command_op_test2 )
{
    using poac::util::command;

    command cmd("mkdir test");
    cmd &= "cd test";

    BOOST_TEST( cmd == command("mkdir test && cd test") );
}
// bool operator==(const std::string& rhs)
BOOST_AUTO_TEST_CASE( poac_util_command_op_test3 )
{
    using poac::util::command;

    command cmd("mkdir test");
    cmd &= "cd test";

    BOOST_TEST( cmd == "mkdir test && cd test" );
}

// command operator&&(const command& rhs)
BOOST_AUTO_TEST_CASE( poac_util_command_op_test4 )
{
    using poac::util::command;

    command cmd("mkdir test");
    command cmd2 = (cmd && command("cd test"));

    BOOST_TEST( cmd2.string() == "mkdir test && cd test" );
}
// command operator&&(const std::string& rhs)
BOOST_AUTO_TEST_CASE( poac_util_command_op_test5 )
{
    using poac::util::command;

    const command cmd("mkdir test");
    const command cmd2 = (cmd && "cd test");

    BOOST_TEST( cmd2.string() == "mkdir test && cd test" );
}

// command operator&=(const command& rhs)
BOOST_AUTO_TEST_CASE( poac_util_command_op_test6 )
{
    using poac::util::command;

    command cmd("mkdir test");
    cmd &= command("cd test");

    BOOST_TEST( cmd.string() == "mkdir test && cd test" );
}
// command operator&=(const std::string& rhs)
BOOST_AUTO_TEST_CASE( poac_util_command_op_test7 )
{
    using poac::util::command;

    command cmd("mkdir test");
    cmd &= "cd test";

    BOOST_TEST( cmd.string() == "mkdir test && cd test" );
}

// command operator||(const command& rhs)
BOOST_AUTO_TEST_CASE( poac_util_command_op_test8 )
{
    using poac::util::command;

    command cmd("mkdir test");
    command cmd2 = (cmd || command("cd test"));

    BOOST_TEST( cmd2.string() == "mkdir test || cd test" );
}
// command operator||(const std::string& rhs)
BOOST_AUTO_TEST_CASE( poac_util_command_op_test9 )
{
    using poac::util::command;

    const command cmd("mkdir test");
    const command cmd2 = (cmd || "cd test");

    BOOST_TEST( cmd2.string() == "mkdir test || cd test" );
}

// command operator|=(const command& rhs)
BOOST_AUTO_TEST_CASE( poac_util_command_op_test10 )
{
    using poac::util::command;

    command cmd("mkdir test");
    cmd |= command("cd test");

    BOOST_TEST( cmd.string() == "mkdir test || cd test" );
}
// command operator|=(const std::string& rhs)
BOOST_AUTO_TEST_CASE( poac_util_command_op_test11 )
{
    using poac::util::command;

    command cmd("mkdir test");
    cmd |= "cd test";

    BOOST_TEST( cmd.string() == "mkdir test || cd test" );
}

// command operator+(const command& rhs)
BOOST_AUTO_TEST_CASE( poac_util_command_op_test12 )
{
    using poac::util::command;

    command cmd("mkdir test");
    command cmd2 = (cmd + command("cd test"));

    BOOST_TEST( cmd2.string() == "mkdir test cd test" );
}
// command operator+(const std::string& rhs)
BOOST_AUTO_TEST_CASE( poac_util_command_op_test13 )
{
    using poac::util::command;

    const command cmd("mkdir test");
    const command cmd2 = (cmd + "cd test");

    BOOST_TEST( cmd2.string() == "mkdir test cd test" );
}

// command operator+=(const command& rhs)
BOOST_AUTO_TEST_CASE( poac_util_command_op_test14 )
{
    using poac::util::command;

    command cmd("mkdir test");
    cmd += command("cd test");

    BOOST_TEST( cmd.string() == "mkdir test cd test" );
}
// command operator+=(const std::string& rhs)
BOOST_AUTO_TEST_CASE( poac_util_command_op_test15 )
{
    using poac::util::command;

    command cmd("mkdir test");
    cmd += "cd test";

    BOOST_TEST( cmd.string() == "mkdir test cd test" );
}

// bool has_command(const std::string& c)
BOOST_AUTO_TEST_CASE( poac_util_command_has_command_test )
{
    using poac::util::_command::has_command;
    BOOST_TEST( has_command("cd") );
}

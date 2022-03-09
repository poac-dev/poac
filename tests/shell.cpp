#include <boost/ut.hpp>

#include <sstream>

#include <poac/util/shell.hpp>

int main() {
    using namespace std::literals::string_literals;
    using namespace boost::ut;
    using poac::util::shell::cmd;

    // 1. shell::shell()
    // 2. shell::shell(const std::string& c)
    "test constructor"_test = [] {
        expect(eq(cmd().string(), ""s)); // 1
        expect(eq(cmd("cd").string(), "cd"s)); // 2
        expect(eq(cmd("cd").string(), "cd"s)); // 3
    };

    // shell env(const std::string& name, const std::string& val)
    "test env"_test = [] {
        cmd c("cmake ..");
        c = c.env("OPENSSL_ROOT_DIR", "/usr/local/opt/openssl/");
        c = c.env("MACOSX_RPATH", "1");

        expect(eq(c.string(), "MACOSX_RPATH=1 OPENSSL_ROOT_DIR=/usr/local/opt/openssl/ cmake .."s));
    };

    // shell stderr_to_stdout()
    "test stderr_to_stdout"_test = [] {
        cmd c("cmake ..");
        c = c.stderr_to_stdout();
        expect(eq(c.string(), "cmake .. 2>&1"s));
    };

    // shell to_dev_null()
    "test to_dev_null"_test = [] {
        cmd c("cmake ..");
        c = c.to_dev_null();
        expect(eq(c.string(), "cmake .. >/dev/null"s));
    };

    // boost::optional<std::string> exec()
    "test exec"_test = [] {
        expect(eq(cmd("echo test").exec().value(), "test\n"s));
        expect(!cmd("nocmd").exec().has_value());
    };

    // bool exec_ignore()
    "test exec_ignore"_test = [] {
        cmd c("cd");
        expect(c.exec_ignore());
    };

    // friend std::ostream& operator<<(std::ostream& stream, const shell& c)
    "test operator<<"_test = [] {
        cmd c("mkdir test");
        c &= "cd test";

        std::ostringstream output;
        output << c;

        expect(eq(output.str(), "mkdir test && cd test"s));
    };

    "test operator==(const shell& rhs)"_test = [] {
        cmd c("mkdir test");
        c &= "cd test";

        expect(c == cmd("mkdir test && cd test"));
    };
    "test operator==(const std::string& rhs)"_test = [] {
        cmd c("mkdir test");
        c &= "cd test";

        expect(c == "mkdir test && cd test");
    };

    "test operator&&(const shell& rhs)"_test = [] {
        cmd c("mkdir test");
        cmd c2 = (c && cmd("cd test"));

        expect(eq(c2.string(), "mkdir test && cd test"s));
    };
    "test operator&&(const std::string& rhs)"_test = [] {
        const cmd c("mkdir test");
        const cmd c2 = (c && "cd test");

        expect(eq(c2.string(), "mkdir test && cd test"s));
    };

    "test operator&=(const shell& rhs)"_test = [] {
        cmd c("mkdir test");
        c &= cmd("cd test");

        expect(eq(c.string(), "mkdir test && cd test"s));
    };
    "test operator&=(const std::string& rhs)"_test = [] {
        cmd c("mkdir test");
        c &= "cd test";

        expect(eq(c.string(), "mkdir test && cd test"s));
    };

    "test operator||(const shell& rhs)"_test = [] {
        cmd c("mkdir test");
        cmd c2 = (c || cmd("cd test"));

        expect(eq(c2.string(), "mkdir test || cd test"s));
    };
    "test operator||(const std::string& rhs)"_test = [] {
        const cmd c("mkdir test");
        const cmd c2 = (c || "cd test");

        expect(eq(c2.string(), "mkdir test || cd test"s));
    };

    "test operator|=(const shell& rhs)"_test = [] {
        cmd c("mkdir test");
        c |= cmd("cd test");

        expect(eq(c.string(), "mkdir test || cd test"s));
    };
    "test operator|=(const std::string& rhs)"_test = [] {
        cmd c("mkdir test");
        c |= "cd test";

        expect(eq(c.string(), "mkdir test || cd test"s));
    };

    "test operator+(const shell& rhs)"_test = [] {
        cmd c("mkdir test");
        cmd c2 = (c + cmd("cd test"));

        expect(eq(c2.string(), "mkdir test cd test"s));
    };
    "test operator+(const std::string& rhs)"_test = [] {
        const cmd c("mkdir test");
        const cmd c2 = (c + "cd test");

        expect(eq(c2.string(), "mkdir test cd test"s));
    };

    "test operator+=(const shell& rhs)"_test = [] {
        cmd c("mkdir test");
        c += cmd("cd test");

        expect(eq(c.string(), "mkdir test cd test"s));
    };
    "test operator+=(const std::string& rhs)"_test = [] {
        cmd c("mkdir test");
        c += "cd test";

        expect(eq(c.string(), "mkdir test cd test"s));
    };

    "test has_command"_test = [] {
        using poac::util::shell::has_command;
        expect(has_command("cd"));
    };
}

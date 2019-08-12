#ifndef POAC_UTIL_SHELL_HPP
#define POAC_UTIL_SHELL_HPP

#include <iostream>
#include <string>
#include <array>
#include <vector>
#include <optional>
#include <cstdio>
#include <cstdlib>

namespace poac::util {
    class shell {
    public:
        std::string string() const {
            return cmd;
        }

        shell() : cmd() {}
        explicit shell(const std::string& c) : cmd(c) {}

        shell& env(const std::string& name, const std::string& value) {
            cmd.insert(0, name + "=" + value + " ");
            return *this;
        }
        shell& stderr_to_stdout() {
            cmd += " 2>&1";
            return *this;
        }
        shell& to_dev_null() {
            cmd += " >/dev/null";
            return *this;
        }
        shell& dump_stdout() {
            cmd += " 1>/dev/null";
            return *this;
        }
        shell& dump_stderr() {
            cmd += " 2>/dev/null";
            return *this;
        }

        // TODO: 全てのstderrをstdoutにパイプし，吸収した上で，resultとして返却？？？
        // TODO: errorと，その内容を同時に捕捉できない．
        std::optional<std::string>
        exec() const {
            std::array<char, 128> buffer{};
            std::string result;

#ifdef _WIN32
            if (FILE* pipe = _popen(cmd.c_str(), "r")) {
#else
            if (FILE* pipe = popen(cmd.c_str(), "r")) {
#endif
                while (std::fgets(buffer.data(), 128, pipe) != nullptr)
                    result += buffer.data();
#ifdef _WIN32
                if (_pclose(pipe) != 0) {
#else
                if (pclose(pipe) != 0) {
#endif
                    std::cout << result; // TODO: error時も，errorをstdoutにパイプしていれば，resultに格納されるため，これを返したい．
                    return std::nullopt;
                }
            }
            else {
                return std::nullopt;
            }
            return result;
        }

        bool exec_ignore() const {
            // EXIT_SUCCESS -> 0 -> false -> true
            // EXIT_FAILURE -> 1 -> true -> false
            return !static_cast<bool>(std::system(cmd.c_str()));
        }

        friend std::ostream&
        operator<<(std::ostream& os, const shell& c) {
            return (os << c.cmd);
        }

        bool operator==(const shell& rhs) const {
            return this->cmd == rhs.cmd;
        }
        bool operator==(const std::string& rhs) const {
            return this->cmd == rhs;
        }

        shell operator&&(const shell& rhs) const {
            return shell(this->cmd + " && " + rhs.cmd);
        }
        shell operator&&(const std::string& rhs) const {
            return shell(this->cmd + " && " + rhs);
        }

        shell& operator&=(const shell& rhs) {
            this->cmd += " && " + rhs.cmd;
            return *this;
        }
        shell& operator&=(const std::string& rhs) {
            this->cmd += " && " + rhs;
            return *this;
        }

        shell operator||(const shell& rhs) const {
            return shell(this->cmd + " || " + rhs.cmd);
        }
        shell operator||(const std::string& rhs) const {
            return shell(this->cmd + " || " + rhs);
        }

        shell& operator|=(const shell& rhs) {
            this->cmd += " || " + rhs.cmd;
            return *this;
        }
        shell& operator|=(const std::string& rhs) {
            this->cmd += " || " + rhs;
            return *this;
        }

        shell operator+(const shell& rhs) const { // TODO: "; "でなくても良いのか
            return shell(this->cmd + " " + rhs.cmd);
        }
        shell operator+(const std::string& rhs) const {
            return shell(this->cmd + " " + rhs);
        }

        shell& operator+=(const shell& rhs) {
            this->cmd += " " + rhs.cmd;
            return *this;
        }
        shell& operator+=(const std::string& rhs) {
            this->cmd += " " + rhs;
            return *this;
        }

    private:
        std::string cmd;
    };

    namespace _shell {
        bool has_command(const std::string& c) {
            return shell("type " + c + " >/dev/null 2>&1").exec().has_value();
        }
    }
} // end namespace
#endif // !POAC_UTIL_SHELL_HPP

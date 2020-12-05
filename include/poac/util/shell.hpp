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
    class cmd {
    public:
        std::string string() const {
            return cmd_;
        }

        cmd() : cmd_() {}
        explicit cmd(const std::string& c) : cmd_(c) {}

        cmd& env(const std::string& name, const std::string& value) {
            cmd_.insert(0, name + "=" + value + " ");
            return *this;
        }
        cmd& stderr_to_stdout() {
            cmd_ += " 2>&1";
            return *this;
        }
        cmd& to_dev_null() {
            cmd_ += " >/dev/null";
            return *this;
        }
        cmd& dump_stdout() {
            cmd_ += " 1>/dev/null";
            return *this;
        }
        cmd& dump_stderr() {
            cmd_ += " 2>/dev/null";
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
            if (FILE* pipe = popen(cmd_.c_str(), "r")) {
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
            return !static_cast<bool>(std::system(cmd_.c_str()));
        }

        friend std::ostream&
        operator<<(std::ostream& os, const cmd& c) {
            return (os << c.cmd_);
        }

        bool operator==(const cmd& rhs) const {
            return this->cmd_ == rhs.cmd_;
        }
        bool operator==(const std::string& rhs) const {
            return this->cmd_ == rhs;
        }

        cmd
        operator&&(const cmd& rhs) const {
            return cmd(this->cmd_ + " && " + rhs.cmd_);
        }
        cmd
        operator&&(const std::string& rhs) const {
            return cmd(this->cmd_ + " && " + rhs);
        }

        cmd& operator&=(const cmd& rhs) {
            this->cmd_ += " && " + rhs.cmd_;
            return *this;
        }
        cmd& operator&=(const std::string& rhs) {
            this->cmd_ += " && " + rhs;
            return *this;
        }

        cmd
        operator||(const cmd& rhs) const {
            return cmd(this->cmd_ + " || " + rhs.cmd_);
        }
        cmd
        operator||(const std::string& rhs) const {
            return cmd(this->cmd_ + " || " + rhs);
        }

        cmd& operator|=(const cmd& rhs) {
            this->cmd_ += " || " + rhs.cmd_;
            return *this;
        }
        cmd& operator|=(const std::string& rhs) {
            this->cmd_ += " || " + rhs;
            return *this;
        }

        cmd
        operator+(const cmd& rhs) const { // TODO: "; "でなくても良いのか
            return cmd(this->cmd_ + " " + rhs.cmd_);
        }
        cmd
        operator+(const std::string& rhs) const {
            return cmd(this->cmd_ + " " + rhs);
        }

        cmd& operator+=(const cmd& rhs) {
            this->cmd_ += " " + rhs.cmd_;
            return *this;
        }
        cmd& operator+=(const std::string& rhs) {
            this->cmd_ += " " + rhs;
            return *this;
        }

    private:
        std::string cmd_;
    };

    namespace _shell {
        bool has_command(const std::string& c) {
            return cmd("type " + c + " >/dev/null 2>&1").exec().has_value();
        }
    }
} // end namespace
#endif // !POAC_UTIL_SHELL_HPP

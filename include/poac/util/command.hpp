#ifndef POAC_UTIL_COMMAND_HPP
#define POAC_UTIL_COMMAND_HPP

#include <iostream>
#include <string>
#include <vector>
#include <optional>
#include <cstdio>
#include <cstdlib>


namespace poac::util {
    class command {
    public:
        std::string string() const { return cmd; }

        command() { cmd = ""; }
        command(const std::string& c) { cmd = c; }

        command env(const std::string& name, const std::string& val) {
            return cmd.insert(0, name + "=" + val + " ");
        }
        command stderr_to_stdout() {
            return cmd + " 2>&1";
        }
        command to_dev_null() {
            return cmd + " >/dev/null";
        }

        // TODO: 全てのstderrをstdoutにパイプし，吸収した上で，resultとして返却？？？
        // TODO: errorと，その内容を同時に捕捉できない．
        std::optional<std::string> exec() const {
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

        bool exec_incontinent() const {
            return static_cast<bool>(std::system(cmd.c_str()));
        }

        friend std::ostream& operator<<(std::ostream& stream, const command& c) {
            stream << c.cmd;
            return stream;
        }

        bool operator==(const command& rhs) const {
            return this->cmd == rhs.cmd;
        }
        bool operator==(const std::string& rhs) const {
            return this->cmd == rhs;
        }

        command operator&&(const command& rhs) const {
            return command(this->cmd + " && " + rhs.cmd);
        }
        command operator&&(const std::string& rhs) const {
            return command(this->cmd + " && " + rhs);
        }

        command operator&=(const command& rhs) {
            return this->cmd += (" && " + rhs.cmd);
        }
        command operator&=(const std::string& rhs) {
            return this->cmd += (" && " + rhs);
        }

        command operator||(const command& rhs) const {
            return command(this->cmd + " || " + rhs.cmd);
        }
        command operator||(const std::string& rhs) const {
            return command(this->cmd + " || " + rhs);
        }

        command operator|=(const command& rhs) {
            return this->cmd += (" || " + rhs.cmd);
        }
        command operator|=(const std::string& rhs) {
            return this->cmd += (" || " + rhs);
        }

        command operator+(const command& rhs) const { // TODO: "; "でなくても良いのか
            return command(this->cmd + " " + rhs.cmd);
        }
        command operator+(const std::string& rhs) const {
            return command(this->cmd + " " + rhs);
        }

        command operator+=(const command& rhs) {
            return this->cmd += " " + rhs.cmd;
        }
        command operator+=(const std::string& rhs) {
            return this->cmd += " " + rhs;
        }

    private:
        std::string cmd;
    };

    namespace _command {
        bool has_command(const std::string& c) {
            return static_cast<bool>(command("type " + c + " >/dev/null 2>&1").exec());
        }
    }
} // end namespace
#endif // !POAC_UTIL_COMMAND_HPP

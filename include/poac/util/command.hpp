#ifndef POAC_UTIL_COMMAND_HPP
#define POAC_UTIL_COMMAND_HPP

#include <iostream>
#include <string>
#include <vector>
#include <optional>


namespace poac::util {
    class command {
    public:
        std::string data() const { return cmd; }

        command() { cmd = ""; }
        command(const std::string& c) { cmd = c; }
        command(const std::vector<std::string>& cs) {
            util::command cmd2;
            int count = 0;
            for (const auto &s : cs) {
                if (count++ == 0) cmd2 = util::command(s).stderr_to_stdout(); // TODO: std_err
                else cmd2 &= util::command(s).stderr_to_stdout();
            }
            cmd = cmd2.data();
        }

        command env(const std::string& name, const std::string& val) {
            return cmd.insert(0, name + "=" + val + " ");
        }
        command stderr_to_stdout() {
            return cmd + " 2>&1";
        }

        // TODO: 全てのstderrをstdoutにパイプし，吸収した上で，resultとして返却？？？
        // TODO: errorと，その内容を同時に捕捉できない．
        std::optional<std::string> exec() const {
            std::array<char, 128> buffer;
            std::string result;

            if (FILE* pipe = popen(cmd.c_str(), "r")) {
                while (std::fgets(buffer.data(), 128, pipe) != nullptr)
                    result += buffer.data();
                if (pclose(pipe) != 0) {
                    std::cout << result; // TODO: error時も，errorをstdoutにパイプしていれば，resultに格納されるため，これを返したい．
                    return std::nullopt;
                }
            }
            else {
                return std::nullopt;
            }
            return result;
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

        command operator+(const command& rhs) const {
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
} // end namespace
#endif // !POAC_UTIL_COMMAND_HPP

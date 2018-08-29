#ifndef POAC_UTIL_COMMAND_HPP
#define POAC_UTIL_COMMAND_HPP

#include <iostream>
#include <string>
#include <vector>

#include <boost/optional.hpp>


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
                if (count++ == 0) cmd2 = util::command(s).std_err(); // TODO: std_err
                else cmd2 &= util::command(s).std_err();
            }
            cmd = cmd2.data();
        }

        command env(const std::string& name, const std::string& val) {
            return cmd.insert(0, name + "=" + val + " ");
        }
        command std_err() {
            return cmd + " 2>&1";
        }

        boost::optional<std::string> run() const {
            std::array<char, 128> buffer;
            std::string result;

            if (FILE* pipe = popen(cmd.c_str(), "r")) {
                while (std::fgets(buffer.data(), 128, pipe) != nullptr)
                    result += buffer.data();
//                std::cout << result; // TODO: error時も，errorをstdoutにパイプしていれば，resultに格納されるため，これを返したい．
                if (pclose(pipe) != 0)
                    return boost::none;
            }
            else {
                return boost::none;
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

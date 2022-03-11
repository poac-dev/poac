#include <boost/ut.hpp>

#include <poac/core/builder/ninja_syntax.hpp>

#include <string>
#include <sstream>

//static const std::string LONG_WORD = std::string(10, 'a');
//static const std::string LONGWORDWITHSPACES = std::string(5, 'a') + "$ " + std::string(5, 'a');
//static const std::string INDENT = "    ";

int main() {
    using namespace std::literals::string_literals;
    using namespace boost::ut;
    using namespace boost::ut::spec;
    namespace ninja_syntax = poac::core::builder::ninja_syntax;

    describe("test build") = [] {
        it("test variables dict") = [] {
            ninja_syntax::writer writer{ std::ostringstream() };
            writer.build(
                std::vector<std::filesystem::path>{"out"},
                "cc",
                ninja_syntax::build_set_t{
                    .inputs = "in",
                    .variables = std::unordered_map<std::string, std::string>{
                        {"name", "value"}
                    }
                }
            );

            expect(eq(
                "build out: cc in\n"
                "  name = value\n"s,
                writer.get_value()
            ));
        };

        it("test implicit outputs") = [] {
            ninja_syntax::writer writer{ std::ostringstream() };
            writer.build(
                std::vector<std::filesystem::path>{"o"},
                "cc",
                ninja_syntax::build_set_t{
                    .inputs = "i",
                    .implicit_outputs = "io",
                }
            );

            expect(eq(
                "build o | io: cc i\n"s,
                writer.get_value()
            ));
        };
    };
}

// std
#include <memory>
#include <sstream>
#include <string>
#include <vector>

// external
// clang-format off
// to avoid reporting errors with inline namespace on only the dry-run mode. (IDK why)
#include <boost/ut.hpp>
// clang-format on
#include <boost/predef.h> // NOLINT: found C system header after other header

// internal
#include "../../util/ut_helpers/throws_with_msg.hpp"

#include <poac/util/cfg.hpp>

auto
main() -> int {
  using namespace std::literals::string_literals;
  using namespace boost::ut;
  using namespace boost::ut::spec;
  using poac::test::ut_helpers::throws_with_msg;

  describe("test parse") = [] {
    using poac::util::cfg::parse;

    it("test good cases") = [] {
      using poac::util::cfg::IdentError;
      using poac::util::cfg::SyntaxError;

      throws_with_msg<IdentError>(
          [] { parse("foo"); },
          "cfg expected parenthesis, comma, identifier, or string\n"
          "foo\n"
          "^-- unknown identify"
      );
      throws_with_msg<IdentError>(
          [] { parse("_bar"); },
          "cfg expected parenthesis, comma, identifier, or string\n"
          "_bar\n"
          "^--- unknown identify"
      );
      throws_with_msg<IdentError>(
          [] { parse(" foo"); },
          "cfg expected parenthesis, comma, identifier, or string\n"
          " foo\n"
          " ^-- unknown identify"
      );
      throws_with_msg<IdentError>(
          [] { parse(" foo  "); },
          "cfg expected parenthesis, comma, identifier, or string\n"
          " foo  \n"
          " ^-- unknown identify"
      );
      throws_with_msg<IdentError>(
          [] { parse(" foo  = \"bar\""); },
          "cfg expected parenthesis, comma, identifier, or string\n"
          " foo  = \"bar\"\n"
          " ^-- unknown identify"
      );
      throws_with_msg<IdentError>(
          [] { parse("foo=\"\""); },
          "cfg expected parenthesis, comma, identifier, or string\n"
          "foo=\"\"\n"
          "^-- unknown identify"
      );
      throws_with_msg<IdentError>(
          [] { parse(" foo=\"3\"      "); },
          "cfg expected parenthesis, comma, identifier, or string\n"
          " foo=\"3\"      \n"
          " ^-- unknown identify"
      );
      throws_with_msg<IdentError>(
          [] { parse("foo = \"3 e\""); },
          "cfg expected parenthesis, comma, identifier, or string\n"
          "foo = \"3 e\"\n"
          "^-- unknown identify"
      );

      throws_with_msg<SyntaxError>(
          [] { parse("os"); },
          "cfg syntax error\n"
          "os\n"
          "   ^ expected operator"
      );
      throws_with_msg<IdentError>(
          [] { parse("_os"); },
          "cfg expected parenthesis, comma, identifier, or string\n"
          "_os\n"
          "^-- unknown identify"
      );
      throws_with_msg<SyntaxError>(
          [] { parse(" os"); },
          "cfg syntax error\n"
          " os\n"
          "    ^ expected operator"
      );
      throws_with_msg<SyntaxError>(
          [] { parse(" os  "); },
          "cfg syntax error\n"
          " os  \n"
          "    ^ expected operator"
      );
      expect(nothrow([] { parse(" os  = \"bar\""); }));
      expect(nothrow([] { parse("os=\"\""); }));
      expect(nothrow([] { parse(" os=\"3\"      "); }));
      expect(nothrow([] { parse("os = \"3 e\""); }));

      throws_with_msg<SyntaxError>(
          [] { parse("all()"); },
          "cfg syntax error\n"
          "all()\n"
          "      ^ expected operator"
      );
      throws_with_msg<IdentError>(
          [] { parse("all(a)"); },
          "cfg expected parenthesis, comma, identifier, or string\n"
          "all(a)\n"
          "    ^ unknown identify"
      );
      throws_with_msg<IdentError>(
          [] { parse("all(a, b)"); },
          "cfg expected parenthesis, comma, identifier, or string\n"
          "all(a, b)\n"
          "    ^ unknown identify"
      );
      throws_with_msg<IdentError>(
          [] { parse("all(a, )"); },
          "cfg expected parenthesis, comma, identifier, or string\n"
          "all(a, )\n"
          "    ^ unknown identify"
      );
      throws_with_msg<IdentError>(
          [] { parse("not(a = \"b\")"); },
          "cfg expected parenthesis, comma, identifier, or string\n"
          "not(a = \"b\")\n"
          "    ^ unknown identify"
      );
      throws_with_msg<IdentError>(
          [] { parse("not(all(a))"); },
          "cfg expected parenthesis, comma, identifier, or string\n"
          "not(all(a))\n"
          "        ^ unknown identify"
      );

      throws_with_msg<SyntaxError>(
          [] { parse("all()"); },
          "cfg syntax error\n"
          "all()\n"
          "      ^ expected operator"
      );
      throws_with_msg<SyntaxError>(
          [] { parse("all(os)"); },
          "cfg syntax error\n"
          "all(os)\n"
          "       ^ expected operator"
      );
      throws_with_msg<SyntaxError>(
          [] { parse("all(os, compiler)"); },
          "cfg syntax error\n"
          "all(os, compiler)\n"
          "       ^ expected operator"
      );
      throws_with_msg<SyntaxError>(
          [] { parse("all(os, )"); },
          "cfg syntax error\n"
          "all(os, )\n"
          "       ^ expected operator"
      );
      expect(nothrow([] { parse("not(os = \"b\")"); }));
      throws_with_msg<SyntaxError>(
          [] { parse("not(all(os))"); },
          "cfg syntax error\n"
          "not(all(os))\n"
          "           ^ expected operator"
      );

      expect(nothrow([] { parse("cfg(not(os = \"foo\"))"); }));
      expect(nothrow([] { parse(R"(all(os = "foo", os = "bar"))"); }));
      expect(nothrow([] { parse(R"(any(os = "foo", os = "bar"))"); }));
    };

    it("test bad cases") = [] {
      using poac::util::cfg::StringError;
      using poac::util::cfg::IdentError;
      using poac::util::cfg::OperatorError;
      using poac::util::cfg::ExpressionError;
      using poac::util::cfg::SyntaxError;

      throws_with_msg<ExpressionError>(
          [] { parse(" "); }, "expected start of a cfg expression"
      );
      throws_with_msg<SyntaxError>(
          [] { parse(" all"); },
          "cfg syntax error\n"
          " all\n"
          "    ^ expected '(', but cfg expression ended"
      );
      throws_with_msg<SyntaxError>(
          [] { parse("all(os"); },
          "cfg syntax error\n"
          "all(os\n"
          "       ^ expected operator"
      );
      throws_with_msg<SyntaxError>(
          [] { parse("not"); },
          "cfg syntax error\n"
          "not\n"
          "   ^ expected '(', but cfg expression ended"
      );
      throws_with_msg<SyntaxError>(
          [] { parse("not(os"); },
          "cfg syntax error\n"
          "not(os\n"
          "       ^ expected operator"
      );
      throws_with_msg<SyntaxError>(
          [] { parse("os "); },
          "cfg syntax error\n"
          "os \n"
          "   ^ expected operator"
      );
      throws_with_msg<SyntaxError>(
          [] { parse("os = "); },
          "cfg syntax error\n"
          "os = \n"
          "    ^ expected a string, but cfg expression ended"
      );
      throws_with_msg<SyntaxError>(
          [] { parse("all(not())"); },
          "cfg syntax error\n"
          "all(not())\n"
          "          ^ expected operator"
      );
      throws_with_msg<IdentError>(
          [] { parse("foo(a)"); },
          "cfg expected parenthesis, comma, identifier, or string\n"
          "foo(a)\n"
          "^-- unknown identify"
      );

      throws_with_msg<StringError>(
          [] { parse("os = \"foo"); },
          "missing terminating '\"' character\n"
          "os = \"foo\n"
          "     ^--- unterminated string"
      );
      throws_with_msg<OperatorError>(
          [] { parse("os < \"foo\""); },
          "cfg operator error\n"
          "os < \"foo\"\n"
          "   ^ cannot be specified except os_version"
      );
      throws_with_msg<OperatorError>(
          [] { parse("os <= \"foo\""); },
          "cfg operator error\n"
          "os <= \"foo\"\n"
          "   ^- cannot be specified except os_version"
      );
      throws_with_msg<OperatorError>(
          [] { parse("os > \"foo\""); },
          "cfg operator error\n"
          "os > \"foo\"\n"
          "   ^ cannot be specified except os_version"
      );
      throws_with_msg<OperatorError>(
          [] { parse("os >= \"foo\""); },
          "cfg operator error\n"
          "os >= \"foo\"\n"
          "   ^- cannot be specified except os_version"
      );
      expect(nothrow([] { parse("os_version < \"foo\""); }));
      expect(nothrow([] { parse("os_version <= \"foo\""); }));
      expect(nothrow([] { parse("os_version > \"foo\""); }));
      expect(nothrow([] { parse("os_version >= \"foo\""); }));
      expect(nothrow([] { parse("os_version = \"foo\""); }));

      throws_with_msg<SyntaxError>(
          [] { parse("all(os = \"foo\""); },
          "cfg syntax error\n"
          "all(os = \"foo\"\n"
          "              ^ expected ')', but cfg expression ended"
      );
      throws_with_msg<IdentError>(
          [] { parse("and()"); },
          "cfg expected parenthesis, comma, identifier, or string\n"
          "and()\n"
          "^-- unknown identify"
      );
      throws_with_msg<IdentError>(
          [] { parse("or()"); },
          "cfg expected parenthesis, comma, identifier, or string\n"
          "or()\n"
          "^- unknown identify"
      );
      expect(nothrow([] { parse("compiler = \"foo\""); }));
      expect(nothrow([] { parse("arch = \"foo\""); }));
      expect(nothrow([] { parse("feature = \"foo\""); }));
      expect(nothrow([] { parse("os = \"foo\""); }));
      expect(nothrow([] { parse("platform = \"foo\""); }));
      throws_with_msg<IdentError>(
          [] { parse("foo = \"bar\""); },
          "cfg expected parenthesis, comma, identifier, or string\n"
          "foo = \"bar\"\n"
          "^-- unknown identify"
      );
      throws_with_msg<IdentError>(
          [] { parse("3compiler = \"bar\""); },
          "cfg expected parenthesis, comma, identifier, or string\n"
          "3compiler = \"bar\"\n"
          "^ unexpected character"
      );
      throws_with_msg<SyntaxError>(
          [] { parse("all(compiler = os)"); },
          "cfg syntax error\n"
          "all(compiler = os)\n"
          "               ^- expected a string"
      );
      throws_with_msg<SyntaxError>(
          [] { parse("all compiler = \"bar\")"); },
          "cfg syntax error\n"
          "all compiler = \"bar\")\n"
          "   ^ excepted '(' after `all`"
      );
      throws_with_msg<SyntaxError>(
          [] { parse(R"(all(not(compiler = "foo", os = "bar"))"); },
          "cfg syntax error\n"
          "all(not(compiler = \"foo\", os = \"bar\")\n"
          "                        ^ excepted ')'"
      );
    };
  };

  "test struct Token"_test = [] {
    using poac::util::cfg::Token;

    expect(throws<std::invalid_argument>([] { Token{ Token::std::string }; }));
    expect(throws<std::invalid_argument>([] { Token{ Token::Ident }; }));
    expect(throws<std::invalid_argument>([] { Token(Token::Comma, "foo"); }));
    expect(throws<std::invalid_argument>([] {
      Token(Token::GtEq, Token::ident::os);
    }));
  };

  "test to_string(Token::ident ident)"_test = [] {
    using poac::util::cfg::Token;
    using poac::util::cfg::to_string;

    expect(eq(to_string(Token::ident::cfg), "cfg"s));
    expect(eq(to_string(Token::ident::not_), "not"s));
    expect(eq(to_string(Token::ident::all), "all"s));
    expect(eq(to_string(Token::ident::any), "any"s));
    expect(eq(to_string(Token::ident::compiler), "compiler"s));
    expect(eq(to_string(Token::ident::arch), "arch"s));
    expect(eq(to_string(Token::ident::feature), "feature"s));
    expect(eq(to_string(Token::ident::os), "os"s));
    expect(eq(to_string(Token::ident::os_version), "os_version"s));
    expect(eq(to_string(Token::ident::platform), "platform"s));
  };

  "test operator<<(std::ostream& os, const Token& token)"_test = [] {
    using poac::util::cfg::Token;

    {
      std::ostringstream output;
      output << Token{ Token::LeftParen };
      expect(eq(output.str(), "left_paren: ("s));
    }
    {
      std::ostringstream output;
      output << Token{ Token::RightParen };
      expect(eq(output.str(), "right_paren: )"s));
    }
    {
      std::ostringstream output;
      output << Token{ Token::Comma };
      expect(eq(output.str(), "comma: ,"s));
    }
    {
      std::ostringstream output;
      output << Token{ Token::Equals };
      expect(eq(output.str(), "equals: ="s));
    }
    {
      std::ostringstream output;
      output << Token{ Token::Gt };
      expect(eq(output.str(), "gt: >"s));
    }
    {
      std::ostringstream output;
      output << Token{ Token::GtEq };
      expect(eq(output.str(), "gteq: >="s));
    }
    {
      std::ostringstream output;
      output << Token{ Token::Lt };
      expect(eq(output.str(), "lt: <"s));
    }
    {
      std::ostringstream output;
      output << Token{ Token::LtEq };
      expect(eq(output.str(), "lteq: <="s));
    }
    {
      std::ostringstream output;
      const std::string s = "foo";
      output << Token{ Token::std::string, s };
      expect(eq(output.str(), "string: " + s));
    }
    {
      std::ostringstream output;
      output << Token{ Token::Ident, Token::ident::os };
      expect(eq(output.str(), "ident: " + to_string(Token::ident::os)));
    }
  };

  "test from_token_ident(Token::ident ident)"_test = [] {
    using poac::util::cfg::Cfg;
    using poac::util::cfg::Token;

    expect(throws<std::invalid_argument>([] {
      Cfg(Token::ident::all, Cfg::Op::Lt, "foo");
    }));
  };

  "test struct CfgExpr"_test = [] {
    using poac::util::cfg::CfgExpr;
    using poac::util::cfg::Cfg;
    using poac::util::cfg::Token;

    const auto c = Cfg(Token::ident::compiler, Cfg::Op::Equals, "foo");
    expect(throws<std::invalid_argument>([&] { CfgExpr(CfgExpr::cfg, c); }));

    std::vector<CfgExpr> e;
    e.emplace_back(CfgExpr::value, c);
    expect(throws<std::invalid_argument>([&] {
      CfgExpr(CfgExpr::not_, std::move(e));
    }));
    expect(throws<std::invalid_argument>([&] {
      CfgExpr(
          CfgExpr::all, std::make_unique<CfgExpr>(CfgExpr(CfgExpr::value, c))
      );
    }));
  };

#if !(BOOST_OS_LINUX || BOOST_OS_MACOS)
  skip /
#endif
      describe("test cfg_cfgexpr_match") = [] {
    using poac::util::cfg::parse;

    it("test1") = [] {
      expect(!parse("cfg(not(os = \"unix\"))").match());
      expect(parse("cfg(not(not(os = \"unix\")))").match());
      expect(parse(R"(cfg(any(os = "linux", os = "macos"))))").match());
      expect(!parse(R"(cfg(all(os = "linux", os = "macos"))))").match());
      expect(parse(R"(cfg(all(os = "unix", os = "unix"))))").match());

      // Always return false (unimplemented)
      expect(!parse("cfg(compiler = \"gcc\")").match());
      expect(!parse("cfg(os_version > \"14\")").match());
    };

#if !BOOST_ARCH_X86
    skip /
#endif
        it("test x86") = [] { expect(parse("cfg(arch = \"x86\")").match()); };

#if !BOOST_ARCH_X86_64
    skip /
#endif
        it("test x86_64") = [] {
      expect(parse("cfg(arch = \"x86_64\")").match());
    };

    // #ifdef __SSE__
    //     BOOST_CHECK( parse("cfg(feature = \"sse\")").match() );
    // #endif

#if BOOST_PLAT_MINGW
    skip /
#endif
        it("test mingw") = [] {
      expect(parse("cfg(not(platform = \"mingw\"))").match());
    };

    // #if BOOST_ARCH_X86_64 && defined(__SSE__) && !BOOST_PLAT_MINGW
    //     BOOST_CHECK(
    //         parse("cfg(all(os = \"unix\", "
    //                       "arch = \"x86_64\", "
    //                       "feature = \"x86_64\", "
    //                       "not(platform = \"mingw\")"
    //                    "))").match()
    //     );
    // #endif
  };

  "test to_kind(std::string_view kind)"_test = [] {
    using poac::util::cfg::Token;
    using poac::util::cfg::to_kind;

    expect(constant<to_kind("(") == Token::LeftParen>);
    expect(constant<to_kind(")") == Token::RightParen>);
    expect(constant<to_kind(",") == Token::Comma>);
    expect(constant<to_kind("=") == Token::Equals>);
    expect(constant<to_kind(">") == Token::Gt>);
    expect(constant<to_kind(">=") == Token::GtEq>);
    expect(constant<to_kind("<") == Token::Lt>);
    expect(constant<to_kind("<=") == Token::LtEq>);
    expect(throws<poac::util::cfg::Exception>([] { to_kind("unknown"); }));
  };

  "test to_string(Token::ident ident)"_test = [] {
    using poac::util::cfg::Token;
    using poac::util::cfg::to_string;

    expect(eq(to_string(Token::ident::cfg), "cfg"s));
    expect(eq(to_string(Token::ident::not_), "not"s));
    expect(eq(to_string(Token::ident::all), "all"s));
    expect(eq(to_string(Token::ident::any), "any"s));
    expect(eq(to_string(Token::ident::compiler), "compiler"s));
    expect(eq(to_string(Token::ident::arch), "arch"s));
    expect(eq(to_string(Token::ident::feature), "feature"s));
    expect(eq(to_string(Token::ident::os), "os"s));
    expect(eq(to_string(Token::ident::os_version), "os_version"s));
    expect(eq(to_string(Token::ident::platform), "platform"s));
  };

  "test operator<<(std::ostream& os, const Token& token)"_test = [] {
    using poac::util::cfg::Token;
    {
      std::ostringstream output;
      output << Token{ Token::LeftParen };
      expect(eq(output.str(), "left_paren: ("s));
    }
    {
      std::ostringstream output;
      output << Token{ Token::RightParen };
      expect(eq(output.str(), "right_paren: )"s));
    }
    {
      std::ostringstream output;
      output << Token{ Token::Comma };
      expect(eq(output.str(), "comma: ,"s));
    }
    {
      std::ostringstream output;
      output << Token{ Token::Equals };
      expect(eq(output.str(), "equals: ="s));
    }
    {
      std::ostringstream output;
      output << Token{ Token::Gt };
      expect(eq(output.str(), "gt: >"s));
    }
    {
      std::ostringstream output;
      output << Token{ Token::GtEq };
      expect(eq(output.str(), "gteq: >="s));
    }
    {
      std::ostringstream output;
      output << Token{ Token::Lt };
      expect(eq(output.str(), "lt: <"s));
    }
    {
      std::ostringstream output;
      output << Token{ Token::LtEq };
      expect(eq(output.str(), "lteq: <="s));
    }
    {
      std::ostringstream output;
      output << Token{ Token::std::string, "test_string" };
      expect(eq(output.str(), "string: test_string"s));
    }
    {
      std::ostringstream output;
      output << Token{ Token::Ident, Token::ident::cfg };
      expect(eq(output.str(), "ident: cfg"s));
    }
  };
} // NOLINT: too long fn main

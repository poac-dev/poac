// std
#include <iostream>

// external
#include <boost/ut.hpp>

// internal
import semver.lexer;
import semver.token;

auto main() -> int {
  using namespace std::literals::string_literals;
  using namespace boost::ut;
  using namespace boost::ut::spec;

  using semver::Lexer;
  using semver::Token;

  describe("test simple tokens") = [] {
    Lexer lexer{"=><<=>=^~*.,-+||"};

    it("Eq") = [&] {
      const Token token = lexer.next();
      const bool res = token.kind == Token::Eq;
      expect(res);
    };
    it("Gt") = [&] {
      const Token token = lexer.next();
      const bool res = token.kind == Token::Gt;
      expect(res);
    };
    it("Lt") = [&] {
      const Token token = lexer.next();
      const bool res = token.kind == Token::Lt;
      expect(res);
    };
    it("LtEq") = [&] {
      const Token token = lexer.next();
      const bool res = token.kind == Token::LtEq;
      expect(res);
    };
    it("GtEq") = [&] {
      const Token token = lexer.next();
      const bool res = token.kind == Token::GtEq;
      expect(res);
    };
    it("Caret") = [&] {
      const Token token = lexer.next();
      const bool res = token.kind == Token::Caret;
      expect(res);
    };
    it("Tilde") = [&] {
      const Token token = lexer.next();
      const bool res = token.kind == Token::Tilde;
      expect(res);
    };
    it("Star") = [&] {
      const Token token = lexer.next();
      const bool res = token.kind == Token::Star;
      expect(res);
    };
    it("Dot") = [&] {
      const Token token = lexer.next();
      const bool res = token.kind == Token::Dot;
      expect(res);
    };
    it("Comma") = [&] {
      const Token token = lexer.next();
      const bool res = token.kind == Token::Comma;
      expect(res);
    };
    it("Hyphen") = [&] {
      const Token token = lexer.next();
      const bool res = token.kind == Token::Hyphen;
      expect(res);
    };
    it("Plus") = [&] {
      const Token token = lexer.next();
      const bool res = token.kind == Token::Plus;
      expect(res);
    };
    it("Or") = [&] {
      const Token token = lexer.next();
      const bool res = token.kind == Token::Or;
      expect(res);
    };
  };

  describe("test whitespace") = [] {
    Lexer lexer{"  foo \t\n\rbar"};

    it("Whitespace") = [&] {
      const Token token = lexer.next();
      const bool test1 = token.kind == Token::Whitespace;
      expect(test1);

      expect(std::holds_alternative<Token::WhitespaceType>(token.component));
      const bool test2 = std::get<Token::WhitespaceType>(token.component)
                         == Token::WhitespaceType(0, 2);
      expect(test2);
    };
    it("AlphaNumeric") = [&] {
      const Token token = lexer.next();
      const bool test1 = token.kind == Token::AlphaNumeric;
      expect(test1);

      expect(std::holds_alternative<Token::AlphanumericType>(token.component));
      const bool test2 =
          std::get<Token::AlphanumericType>(token.component) == "foo";
      expect(test2);
    };
    it("Whitespace 2") = [&] {
      const Token token = lexer.next();
      const bool test1 = token.kind == Token::Whitespace;
      expect(test1);

      expect(std::holds_alternative<Token::WhitespaceType>(token.component));
      const bool test2 = std::get<Token::WhitespaceType>(token.component)
                         == Token::WhitespaceType(5, 9);
      expect(test2);
    };
    it("AlphaNumeric 2") = [&] {
      const Token token = lexer.next();
      const bool test1 = token.kind == Token::AlphaNumeric;
      expect(test1);

      expect(std::holds_alternative<Token::AlphanumericType>(token.component));
      const bool test2 =
          std::get<Token::AlphanumericType>(token.component) == "bar";
      expect(test2);
    };
  };

  describe("test components") = [] {
    it("test1") = [] {
      Lexer lexer{"42"};
      const Token token = lexer.next();
      const bool test1 = token.kind == Token::Numeric;
      expect(test1);

      expect(std::holds_alternative<Token::NumericType>(token.component));
      const bool test2 = std::get<Token::NumericType>(token.component) == 42;
      expect(test2);
    };

    it("test2") = [] {
      Lexer lexer{"0"};
      const Token token = lexer.next();
      const bool test1 = token.kind == Token::Numeric;
      expect(test1);

      expect(std::holds_alternative<Token::NumericType>(token.component));
      const bool test2 = std::get<Token::NumericType>(token.component) == 0;
      expect(test2);
    };

    it("test3") = [] {
      Lexer lexer{"01"};
      const Token token = lexer.next();
      const bool test1 = token.kind == Token::AlphaNumeric;
      expect(test1);

      expect(std::holds_alternative<Token::AlphanumericType>(token.component));
      const bool test2 =
          std::get<Token::AlphanumericType>(token.component) == "01";
      expect(test2);
    };

    it("test4") = [] {
      Lexer lexer{"5885644aa"};
      const Token token = lexer.next();
      const bool test1 = token.kind == Token::AlphaNumeric;
      expect(test1);

      expect(std::holds_alternative<Token::AlphanumericType>(token.component));
      const bool test2 =
          std::get<Token::AlphanumericType>(token.component) == "5885644aa";
      expect(test2);
    };

    it("test5") = [] {
      Lexer lexer{"beta2"};
      const Token token = lexer.next();
      const bool test1 = token.kind == Token::AlphaNumeric;
      expect(test1);

      expect(std::holds_alternative<Token::AlphanumericType>(token.component));
      const bool test2 =
          std::get<Token::AlphanumericType>(token.component) == "beta2";
      expect(test2);
    };

    it("test6") = [] {
      Lexer lexer{"beta.2"};
      {
        const Token token = lexer.next();
        const bool test1 = token.kind == Token::AlphaNumeric;
        expect(test1);

        expect(std::holds_alternative<Token::AlphanumericType>(token.component)
        );
        const bool test2 =
            std::get<Token::AlphanumericType>(token.component) == "beta";
        expect(test2);
      }
      {
        const Token token = lexer.next();
        const bool res = token.kind == Token::Dot;
        expect(res);
      }
      {
        const Token token = lexer.next();
        const bool test1 = token.kind == Token::Numeric;
        expect(test1);

        expect(std::holds_alternative<Token::NumericType>(token.component));
        const bool test2 = std::get<Token::NumericType>(token.component) == 2;
        expect(test2);
      }
    };
  };

  "test is_wildcard"_test = [] {
    expect(constant<Token{Token::Star}.is_wildcard()>);
    expect(constant<Token(Token::AlphaNumeric, "x").is_wildcard()>);
    expect(constant<Token(Token::AlphaNumeric, "X").is_wildcard()>);
    expect(constant<!Token(Token::AlphaNumeric, "other").is_wildcard()>);
  };

  // TODO: not exported
  //  "test str_to_uint"_test = [] {
  //    using semver::str_to_uint;
  //    expect(constant<str_to_uint("123").value() == 123>);
  //    expect(constant<!str_to_uint("abc").has_value()>);
  //    expect(constant<!str_to_uint("12a").has_value()>);
  //  };
}

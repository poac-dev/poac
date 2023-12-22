module;

#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>
#include <variant>
#include <vector>

export module semver.token;

export namespace semver {

struct Token {
  enum Kind {
    /// `=`
    Eq,
    /// `>`
    Gt,
    /// `<`
    Lt,
    /// `<=`
    LtEq,
    /// `>=`
    GtEq,
    /// '^`
    Caret,
    /// '~`
    Tilde,
    /// '*`
    Star,
    /// `.`
    Dot,
    /// `,`
    Comma,
    /// `-`
    Hyphen,
    /// `+`
    Plus,
    /// '||'
    Or,
    /// any number of whitespace (`\t\r\n `) and its span.
    Whitespace,
    /// Numeric component, like `0` or `42`.
    Numeric,
    /// Alphanumeric component, like `alpha1` or `79deadbe`.
    AlphaNumeric,
    /// UnexpectedChar
    Unexpected
  };

  using NullType = std::monostate;
  using WhitespaceType = std::pair<std::size_t, std::size_t>;
  using NumericType = std::uint_fast64_t;
  using AlphanumericType = std::string_view;
  using VariantType =
      std::variant<NullType, WhitespaceType, NumericType, AlphanumericType>;

  Kind kind;
  VariantType component;

  constexpr Token() noexcept : Token(Kind::Unexpected) {} // delegation

  constexpr explicit Token(Kind k) noexcept : kind(k) {}

  constexpr Token(Kind k, const std::size_t& s1, const std::size_t& s2)
      : kind(
          k != Kind::Whitespace ? throw std::invalid_argument("semver::Token")
                                : Kind::Whitespace
      ),
        component(std::make_pair(s1, s2)) {}

  constexpr Token(Kind k, const NumericType& n)
      : kind(
          k != Kind::Numeric ? throw std::invalid_argument("semver::Token")
                             : Kind::Numeric
      ),
        component(n) {}

  constexpr Token(Kind k, AlphanumericType c)
      : kind(
          k != Kind::AlphaNumeric ? throw std::invalid_argument("semver::Token")
                                  : Kind::AlphaNumeric
      ),
        component(c) {}

  Token(const Token&) = default;
  auto operator=(const Token&) -> Token& = default;
  Token(Token&&) noexcept = default;
  auto operator=(Token&&) noexcept -> Token& = default;
  ~Token() = default;

  [[nodiscard]] constexpr auto is_whitespace() const noexcept -> bool {
    return kind == Kind::Whitespace;
  }

  [[nodiscard]] constexpr auto is_simple_token() const noexcept -> bool {
    return std::holds_alternative<NullType>(component);
  }

  [[nodiscard]] constexpr auto is_wildcard() const noexcept -> bool {
    return kind == Kind::Star
           || (std::holds_alternative<AlphanumericType>(component)
               && (std::get<AlphanumericType>(component) == "X"
                   || std::get<AlphanumericType>(component) == "x"));
  }
};

constexpr auto operator==(const Token& lhs, const Token& rhs) -> bool {
  if (lhs.is_simple_token() && rhs.is_simple_token()) {
    return lhs.kind == rhs.kind;
  }
  return (lhs.kind == rhs.kind) && (lhs.component == rhs.component);
}
constexpr auto operator==(const Token& lhs, const Token::Kind& rhs) -> bool {
  return lhs.is_simple_token() && (lhs.kind == rhs);
}
constexpr auto operator==(const Token::Kind& lhs, const Token& rhs) -> bool {
  return rhs.is_simple_token() && (lhs == rhs.kind);
}

constexpr auto operator!=(const Token& lhs, const Token& rhs) -> bool {
  return !(lhs == rhs);
}
constexpr auto operator!=(const Token& lhs, const Token::Kind& rhs) -> bool {
  return !(lhs == rhs);
}
constexpr auto operator!=(const Token::Kind& lhs, const Token& rhs) -> bool {
  return !(lhs == rhs);
}

struct Identifier {
  enum Kind {
    /// An identifier that's solely numbers.
    Numeric,
    /// An identifier with letters and numbers.
    AlphaNumeric
  };

  using NumericType = std::uint_fast64_t;
  using AlphanumericType = std::string_view;
  using VariantType = std::variant<NumericType, AlphanumericType>;

  Kind kind;
  VariantType component;

  Identifier() = delete;
  Identifier(const Identifier&) = default;
  auto operator=(const Identifier&) -> Identifier& = default;
  Identifier(Identifier&&) noexcept = default;
  auto operator=(Identifier&&) noexcept -> Identifier& = default;
  ~Identifier() = default;

  constexpr Identifier(Kind k, const NumericType& n)
      : kind(
          k != Kind::Numeric ? throw std::invalid_argument("semver::Identifier")
                             : Kind::Numeric
      ),
        component(n) {}

  constexpr Identifier(Kind k, AlphanumericType c)
      : kind(
          k != Kind::AlphaNumeric
              ? throw std::invalid_argument("semver::Identifier")
              : Kind::AlphaNumeric
      ),
        component(c) {}

  [[nodiscard]] constexpr auto is_numeric() const noexcept -> bool {
    return kind == Kind::Numeric;
  }

  [[nodiscard]] constexpr auto is_alpha_numeric() const noexcept -> bool {
    return kind == Kind::AlphaNumeric;
  }

  [[nodiscard]] inline auto get_numeric() const -> NumericType {
    return std::get<Identifier::NumericType>(component);
  }

  [[nodiscard]] inline auto get_alpha_numeric() const -> AlphanumericType {
    return std::get<Identifier::AlphanumericType>(component);
  }
};

constexpr auto operator==(const Identifier& lhs, const Identifier& rhs)
    -> bool {
  return (lhs.kind == rhs.kind) && (lhs.component == rhs.component);
}
constexpr auto operator!=(const Identifier& lhs, const Identifier& rhs)
    -> bool {
  return !(lhs == rhs);
}

auto to_string(const Identifier& id) -> std::string {
  if (std::holds_alternative<Identifier::NumericType>(id.component)) {
    return std::to_string(id.get_numeric());
  } else if (std::holds_alternative<Identifier::AlphanumericType>(id.component
             )) {
    return std::string(id.get_alpha_numeric());
  }
  __builtin_unreachable();
}

inline auto operator<<(std::ostream& os, const Identifier& id)
    -> std::ostream& {
  return (os << to_string(id));
}

struct Version {
  /// Major version as number (`0` in `"0.1.2"`).
  std::uint_fast64_t major;
  /// Minor version as number (`1` in `"0.1.2"`).
  std::uint_fast64_t minor;
  /// Patch version as number (`2` in `"0.1.2"`).
  std::uint_fast64_t patch;
  /// Pre-release metadata as a vector of `Identifier` (`"alpha1"` in
  /// `"0.1.2-alpha1"` or `7` (numeric) in `"0.1.2-7"`, `"pre"` and `0`
  /// (numeric) in `"0.1.2-pre.0"`).
  std::vector<Identifier> pre;
  /// Build metadata as a vector of `Identifier` (`"build1"` in `"0.1.2+build1"`
  /// or `7` (numeric) in `"0.1.2+7"`, `"build"` and `0` (numeric) in
  /// `"0.1.2+pre.0"`).
  std::vector<Identifier> build;

  [[nodiscard]] auto get_version() const -> std::string {
    std::string version = std::to_string(major);
    version += "." + std::to_string(minor);
    version += "." + std::to_string(patch);
    if (!pre.empty()) {
      version += "-";
      for (const Identifier& s : pre) {
        version += to_string(s) + ".";
      }
      version.pop_back();
    }
    return version;
  }

  [[nodiscard]] auto get_full() const -> std::string {
    std::string full = get_version();
    if (!build.empty()) {
      full += "+";
      for (const Identifier& s : build) {
        full += to_string(s) + ".";
      }
      full.pop_back();
    }
    return full;
  }
};

} // end namespace semver

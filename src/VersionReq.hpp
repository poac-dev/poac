// Semver version requirement parser
//
// Syntax:
//   VersionReq ::= Version | (Comparator "and" Comparator)
//   Comparator ::= (">=" | "<=" | ">" | "<") Version
//   Version ::= num ("." num ("." num ("-" pre)? ("+" build)? )? )?
//
// Note: Whitespace is permitted around Comparator.
// Note2: Build metadata will be just ignored but accepted by the parser.
#pragma once

#include "Rustify.hpp"
#include "Semver.hpp"

#include <ostream>

// 1. NoOp:
//   1.1. `A.B.C` is exactly the version `A.B.C`
//   1.2. `A.B` is equivalent to `>=A.B.0 and <A.(B+1).0`
//   1.3. `A` is equivalent to `>=A.0.0 and <(A+1).0.0`
//
// 2. Gt:
//   2.1. `>A.B.C` is equivalent to `>=A.B.(C+1)`
//   2.2. `>A.B` is equivalent to `>=A.(B+1).0`
//   2.3. `>A` is equivalent to `>=(A+1).0.0`
//
// 3. Gte:
//   3.1. `>=A.B.C`
//   3.2. `>=A.B` is equivalent to `>=A.B.0`
//   3.3. `>=A` is equivalent to `>=A.0.0`
//
// 4. Lt:
//   4.1. `<A.B.C`
//   4.2. `<A.B` is equivalent to `<A.B.0`
//   4.3. `<A` is equivalent to `<A.0.0`
//
// 5. Lte:
//   5.1. `<=A.B.C` is equivalent to `<A.B.(C+1)`
//   5.2. `<=A.B` is equivalent to `<A.(B+1).0`
//   5.3. `<=A` is equivalent to `<(A+1).0.0`
struct Comparator {
  enum Op {
    Gt, // >
    Gte, // >=
    Lt, // <
    Lte, // <=
  };

  Option<Op> op;
  u64 major;
  Option<u64> minor;
  Option<u64> patch;
  Prerelease pre;

  static Comparator parse(StringRef);
  bool satisfiedBy(const Version&) const noexcept;
  Comparator canonicalize() const noexcept;
  Version to_version() const noexcept;
};

struct VersionReq {
  Comparator left;
  Option<Comparator> right;

  static VersionReq parse(StringRef);
  bool satisfiedBy(const Version&) const noexcept;
  String to_string() const noexcept;

private:
  VersionReq canonicalize() const noexcept;
};

std::ostream& operator<<(std::ostream&, const VersionReq&);

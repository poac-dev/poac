// Semver version requirement parser
//
// Syntax:
//   VersionReq ::= (("=" | CompOp)? OptVersion) | (Comparator "and" Comparator)
//   Comparator ::= CompOp OptVersion
//   OptVersion ::= num ("." num ("." num ("-" pre)? ("+" build)? )? )?
//   CompOp     ::= ">=" | "<=" | ">" | "<"
//   num        ::= <defined in Semver.hpp>
//   pre        ::= <defined in Semver.hpp>
//   build      ::= <defined in Semver.hpp>
//
// Note: Whitespace is permitted around VersionReq and Comparator, not
// OptVersion.  Build metadata will be just ignored but accepted by the
// parser.
#pragma once

#include "Rustify.hpp"
#include "Semver.hpp"

#include <ostream>

// 1. NoOp: (Caret (^), "compatible" updates)
//   1.1. `A.B.C` (where A > 0) is equivalent to `>=A.B.C and <(A+1).0.0`
//   1.2. `A.B` (where A > 0 & B > 0) is equivalent to `^A.B.0` (i.e., 1.1)
//   1.3. `A` is equivalent to `=A` (i.e., 2.3)
//   1.4. `0.B.C` (where B > 0) is equivalent to `>=0.B.C and <0.(B+1).0`
//   1.5. `0.0.C` is equivalent to `=0.0.C` (i.e., 2.1)
//   1.6. `0.0` is equivalent to `=0.0` (i.e., 2.2)
//
// 2. Exact:
//   2.1. `=A.B.C` is exactly the version `A.B.C`
//   2.2. `=A.B` is equivalent to `>=A.B.0 and <A.(B+1).0`
//   2.3. `=A` is equivalent to `>=A.0.0 and <(A+1).0.0`
//
// 3. Gt:
//   3.1. `>A.B.C` is equivalent to `>=A.B.(C+1)`
//   3.2. `>A.B` is equivalent to `>=A.(B+1).0`
//   3.3. `>A` is equivalent to `>=(A+1).0.0`
//
// 4. Gte:
//   4.1. `>=A.B.C`
//   4.2. `>=A.B` is equivalent to `>=A.B.0`
//   4.3. `>=A` is equivalent to `>=A.0.0`
//
// 5. Lt:
//   5.1. `<A.B.C`
//   5.2. `<A.B` is equivalent to `<A.B.0`
//   5.3. `<A` is equivalent to `<A.0.0`
//
// 6. Lte:
//   6.1. `<=A.B.C` is equivalent to `<A.B.(C+1)`
//   6.2. `<=A.B` is equivalent to `<A.(B+1).0`
//   6.3. `<=A` is equivalent to `<(A+1).0.0`
struct Comparator {
  enum Op {
    Exact, // =
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
  String to_string() const noexcept;
  bool satisfiedBy(const Version&) const noexcept;
  Comparator canonicalize() const noexcept;
};

struct VersionReq {
  Comparator left;
  Option<Comparator> right;

  static VersionReq parse(StringRef);
  bool satisfiedBy(const Version&) const noexcept;
  String to_string() const noexcept;
  VersionReq canonicalize() const noexcept;
};

std::ostream& operator<<(std::ostream&, const VersionReq&);

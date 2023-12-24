#pragma once

#include "Rustify.hpp"

#include <ostream>
#include <utility>

struct Version {
  u64 major;
  u64 minor;
  u64 patch;
  Option<String> pre;
  Option<String> build;

  Version() = default;
  Version(
      u64 major, u64 minor, u64 patch, Option<String> pre = None,
      Option<String> build = None
  )
      : major(major), minor(minor), patch(patch), pre(std::move(pre)),
        build(std::move(build)) {}
};
std::ostream& operator<<(std::ostream&, const Version&);
bool operator==(const Version&, const Version&);

Version parseSemver(StringRef);

#pragma once

// internal
#include "poac/util/result.hpp"
#include "poac/util/rustify.hpp"

namespace poac::util::file {

Fn write_file(const Path& p, StringRef content)->Result<void>;

using WriteFileFailed = Error<"writing `{}` failed", String>;

} // namespace poac::util::file

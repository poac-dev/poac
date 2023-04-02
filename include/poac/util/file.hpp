#ifndef POAC_UTIL_FILE_HPP_
#define POAC_UTIL_FILE_HPP_

// internal
#include "poac/util/result.hpp"
#include "poac/util/rustify.hpp"

namespace poac::util::file {

Result<void> write_file(const Path& p, StringRef content) noexcept;

using WriteFileFailed = Error<"writing `{}` failed", String>;

} // namespace poac::util::file

#endif // POAC_UTIL_FILE_HPP_

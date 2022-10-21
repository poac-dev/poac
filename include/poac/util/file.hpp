#ifndef POAC_UTIL_FILE_HPP_
#define POAC_UTIL_FILE_HPP_

// internal
#include "poac/poac.hpp"

namespace poac::util::file {

Result<void>
write_file(const Path& p, StringRef content) noexcept;

using WriteFileFailed = Error<"write to `{}` failed", Path>;

} // namespace poac::util::file

#endif // POAC_UTIL_FILE_HPP_

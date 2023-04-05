// std
#include <fstream>

// internal
#include "poac/util/file.hpp"

namespace poac::util::file {

Fn write_file(const Path& p, StringRef content)->Result<void> {
  std::ofstream ofs(p);
  ofs << content;
  ofs.close();

  if (!ofs) {
    return Err<WriteFileFailed>(p.string());
  }

  return Ok();
}

} // namespace poac::util::file

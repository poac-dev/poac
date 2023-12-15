module;

// std
#include <fstream>

export module poac.util.file;

import poac.util.result;
import poac.util.rustify;

namespace poac::util::file {

using WriteFileFailed = Error<"writing `{}` failed", String>;

export auto write_file(const Path& p, StringRef content) -> Result<void> {
  std::ofstream ofs(p);
  ofs << content;
  ofs.close();

  if (!ofs) {
    return Err<WriteFileFailed>(p.string());
  }

  return Ok();
}

} // namespace poac::util::file

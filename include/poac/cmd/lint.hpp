#ifndef POAC_CMD_LINT_HPP_
#define POAC_CMD_LINT_HPP_

// external
#include <structopt/app.hpp>

// internal
#include "poac/config.hpp"
#include "poac/poac.hpp"

namespace poac::cmd::lint {

struct Options : structopt::sub_command {
  // Dummy option: DO NOT USE
  Option<bool> dummy = false;
  // FIXME(ken-matsui): this is structopt limitation; I'd remove this.
};

inline const Path config_file(config::path::cur_dir / "CPPLINT.cfg");

using CppLintNotFound = Error<
    "`lint` command requires `cpplint`; try installing it by:\n"
    "  pip install cpplint">;

[[nodiscard]] Result<void>
lint(StringRef name, Option<String> args);

[[nodiscard]] Result<void>
exec([[maybe_unused]] const Options& opts);

} // namespace poac::cmd::lint

STRUCTOPT(poac::cmd::lint::Options, dummy);

#endif // POAC_CMD_LINT_HPP_

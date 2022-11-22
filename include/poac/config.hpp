#ifndef POAC_CONFIG_HPP_
#define POAC_CONFIG_HPP_

// std
#include <filesystem>

// internal
#include "poac/poac.hpp"
#include "poac/util/misc.hpp"

namespace poac::config::path {

inline const Path user_dir = util::misc::expand_user().unwrap();

// Ref:
// https://specifications.freedesktop.org/basedir-spec/basedir-spec-latest.html
inline const Path xdg_cache_home =
    util::misc::getenv("XDG_CACHE_HOME", user_dir / ".cache");
inline const Path xdg_data_home =
    util::misc::getenv("XDG_DATA_HOME", user_dir / ".local" / "share");
inline const Path xdg_state_home =
    util::misc::getenv("XDG_STATE_HOME", user_dir / ".local" / "state");

inline const Path data_dir(xdg_data_home / "poac");
inline const Path cred_file(data_dir / "credentials");

inline const Path cache_dir(xdg_cache_home / "poac");
inline const Path archive_dir(cache_dir / "archive");
inline const Path extract_dir(cache_dir / "extract");

inline const Path state_dir(xdg_state_home / "poac");
inline const Path log_file(state_dir / "log");

inline const Path cwd = fs::current_path();
inline const Path src_dir(cwd / "src");
inline const Path include_dir(cwd / "include");
inline const Path tests_dir(cwd / "tests");
inline const Path main_cpp_file(src_dir / "main.cpp");
inline const Path out_dir(cwd / "poac-out");

} // namespace poac::config::path

#endif // POAC_CONFIG_HPP_

#pragma once

// std
#include <filesystem>
#include <string_view>

// internal
#include "./Util/Misc.hpp"
#include "./Util/Rustify.hpp"

namespace poac::config {

inline const Path user_dir = util::misc::expand_user();

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
inline const Path registry_dir(cache_dir / "registry");
inline const Path registry_src_dir(registry_dir / "src");
inline const Path default_registry_dir(registry_src_dir / "poac.dev");

inline const Path git_dir(cache_dir / "git");
inline const Path git_src_dir(git_dir / "src");

inline const Path state_dir(xdg_state_home / "poac");
inline const Path log_file(state_dir / "log");

inline const Path cwd = fs::current_path();
inline const Path src_dir(cwd / "src");
inline const Path include_dir(cwd / "include");
inline const Path tests_dir(cwd / "tests");
inline const Path main_cpp_file(src_dir / "main.cpp");
inline constexpr StringRef POAC_OUT = "poac-out";
inline const Path out_dir(cwd / POAC_OUT);

inline const Path conan_deps_dir(out_dir / ".conan");
inline const Path conan_deps_file(conan_deps_dir / "conan_poac.json");

} // namespace poac::config

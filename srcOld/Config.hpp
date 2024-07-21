#pragma once

// std
#include <filesystem>
#include <string_view>

// internal
#include "./Util/Misc.hpp"
#include "./Util/Rustify.hpp"

namespace poac::config {

inline const fs::path user_dir = util::misc::expand_user();

// Ref:
// https://specifications.freedesktop.org/basedir-spec/basedir-spec-latest.html
inline const fs::path xdg_cache_home =
    util::misc::getenv("XDG_CACHE_HOME", user_dir / ".cache");
inline const fs::path xdg_data_home =
    util::misc::getenv("XDG_DATA_HOME", user_dir / ".local" / "share");
inline const fs::path xdg_state_home =
    util::misc::getenv("XDG_STATE_HOME", user_dir / ".local" / "state");

inline const fs::path data_dir(xdg_data_home / "poac");
inline const fs::path cred_file(data_dir / "credentials");

inline const fs::path cache_dir(xdg_cache_home / "poac");
inline const fs::path registry_dir(cache_dir / "registry");
inline const fs::path registry_src_dir(registry_dir / "src");
inline const fs::path default_registry_dir(registry_src_dir / "poac.dev");

inline const fs::path git_dir(cache_dir / "git");
inline const fs::path git_src_dir(git_dir / "src");

inline const fs::path state_dir(xdg_state_home / "poac");
inline const fs::path log_file(state_dir / "log");

inline const fs::path cwd = fs::current_path();
inline const fs::path src_dir(cwd / "src");
inline const fs::path include_dir(cwd / "include");
inline const fs::path tests_dir(cwd / "tests");
inline const fs::path main_cpp_file(src_dir / "main.cpp");
inline constexpr std::string_view POAC_OUT = "poac-out";
inline const fs::path out_dir(cwd / POAC_OUT);

inline const fs::path conan_deps_dir(out_dir / ".conan");
inline const fs::path conan_deps_file(conan_deps_dir / "conan_poac.json");

} // namespace poac::config

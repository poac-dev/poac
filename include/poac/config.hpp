#ifndef POAC_CONFIG_HPP_
#define POAC_CONFIG_HPP_

// std
#include <filesystem>

// internal
#include "poac/poac.hpp"
#include "poac/util/misc.hpp"

namespace poac::config::path {

inline const Path user_dir = util::misc::expand_user().unwrap();
inline const Path cfg_dir(user_dir / ".poac");
inline const Path cred_file(cfg_dir / "credentials");

inline const Path cache_dir(cfg_dir / "cache");
inline const Path archive_dir(cache_dir / "archive");
inline const Path extract_dir(cache_dir / "extract");

inline const Path cur_dir = fs::current_path();
inline const Path src_dir(cur_dir / "src");
inline const Path include_dir(cur_dir / "include");
inline const Path tests_dir(cur_dir / "tests");
inline const Path main_cpp_file(src_dir / "main.cpp");
inline const Path out_dir(cur_dir / "poac-out");

} // namespace poac::config::path

#endif // POAC_CONFIG_HPP_

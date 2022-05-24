#ifndef POAC_CONFIG_HPP_
#define POAC_CONFIG_HPP_

#ifndef POAC_VERSION
#  error "POAC_VERSION is not defined"
#endif

// std
#include <filesystem>

// internal
#include <poac/data/manifest.hpp>
#include <poac/poac.hpp>
#include <poac/util/misc.hpp>

namespace poac {

inline constexpr StringRef SUPABASE_PROJECT_REF = "jbzuxdflqzzgexrcsiwm";
inline constexpr StringRef SUPABASE_ANON_KEY =
    "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpc3MiOiJzdXBhYmFzZSIsInJlZiI6ImpienV4ZGZscXp6Z2V4cmNzaXdtIiwicm9sZSI6ImFub24iLCJpYXQiOjE2NTI1MjgyNTAsImV4cCI6MTk2ODEwNDI1MH0.QZG-b6ab4iKk_ewlhEO3OtGpJfEFRos_G1fdDqcKrsA";

} // namespace poac

namespace poac::config::path {

inline const Path user_dir = util::misc::expand_user().unwrap();
inline const Path cfg_dir(user_dir / ".poac");
inline const Path cred_file(cfg_dir / "credentials");

inline const Path cache_dir(cfg_dir / "cache");
inline const Path archive_dir(cache_dir / "archive");
inline const Path extract_dir(cache_dir / "extract");

inline const Path cur_dir = fs::current_path();
inline const Path manifest_file(cur_dir / data::manifest::name);
inline const Path src_dir(cur_dir / "src");
inline const Path include_dir(cur_dir / "include");
inline const Path tests_dir(cur_dir / "tests");
inline const Path main_cpp_file(src_dir / "main.cpp");
inline const Path output_dir(cur_dir / "poac_output");

} // namespace poac::config::path

#endif // POAC_CONFIG_HPP_

#ifndef POAC_CONFIG_HPP
#define POAC_CONFIG_HPP

#ifndef POAC_VERSION
#  error "POAC_VERSION is not defined"
#endif

// std
#include <filesystem>

// internal
#include <poac/util/misc.hpp>

namespace poac {
    constexpr char const* SUPABASE_PROJECT_REF =
            "jbzuxdflqzzgexrcsiwm";
    constexpr char const* SUPABASE_ANON_KEY =
            "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpc3MiOiJzdXBhYmFzZSIsInJlZiI6ImpienV4ZGZscXp6Z2V4cmNzaXdtIiwicm9sZSI6ImFub24iLCJpYXQiOjE2NTI1MjgyNTAsImV4cCI6MTk2ODEwNDI1MH0.QZG-b6ab4iKk_ewlhEO3OtGpJfEFRos_G1fdDqcKrsA";
} // end namespace

namespace poac::config::path {
    namespace fs = std::filesystem;

    inline const fs::path user = util::misc::expand_user().unwrap();
    inline const fs::path root(user / ".poac");
    inline const fs::path cache_dir(root / "cache");
    inline const fs::path archive_dir(cache_dir / "archive");
    inline const fs::path extract_dir(cache_dir / "extract");

    inline const fs::path current = fs::current_path();
    inline const fs::path output_dir(current / "poac_output");
} // end namespace

#endif // !POAC_CONFIG_HPP

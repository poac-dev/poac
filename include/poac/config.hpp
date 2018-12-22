#ifndef POAC_CONFIG_HPP
#define POAC_CONFIG_HPP

#if !defined(POAC_PROJECT_ROOT)
#   warning "POAC_PROJECT_ROOT is not defined"
#endif
#if !defined(POAC_VERSION)
#   warning "POAC_VERSION is not defined"
#endif
namespace poac {
    static constexpr char const* POAC_API =
            "https://poac.pm/api/";
    static constexpr char const* POAC_API_PACKAGES =
            "https://poac.pm/api/packages/";
    static constexpr char const* POAC_API_TOKENS_VALIDATE =
            "https://poac.pm/api/tokens/validate/";
    static constexpr char const* POAC_API_PACKAGE_UPLOAD =
            "https://poac-pm.appspot.com/packages/upload/";
}

#endif // !POAC_CONFIG_HPP
#ifndef POAC_CONFIG_HPP
#define POAC_CONFIG_HPP

#if !defined(POAC_PROJECT_ROOT)
#   warning "POAC_PROJECT_ROOT is not defined"
#endif
#if !defined(POAC_VERSION)
#   warning "POAC_VERSION is not defined"
#endif

namespace poac {
    static constexpr bool DEBUG_FLAG = false;

    static constexpr char const* POAC_API =
            "https://poac.pm/api/";
    static constexpr char const* POAC_PACKAGES_API =
            "https://poac.pm/api/packages/";
    static constexpr char const* POAC_TOKENS_VALIDATE_API =
            "https://poac.pm/api/tokens/validate/";
    static constexpr char const* POAC_PACKAGE_UPLOAD_API =
            "https://poac-pm.appspot.com/packages/upload/";

    static constexpr char const* ALGOLIA_APPLICATION_ID =
            "IOCVK5FECM";
    static constexpr char const* ALGOLIA_SEARCH_ONLY_KEY =
            "9c0a76bacf692daa9e8eca2aaff4b2ab";
    static constexpr char const* ALGOLIA_INDEX_NAME =
            "packages";
    static constexpr char const* ALGOLIA_SEARCH_INDEX_API =
            "https://IOCVK5FECM-dsn.algolia.net/1/indexes/packages/query";
}
#endif // !POAC_CONFIG_HPP

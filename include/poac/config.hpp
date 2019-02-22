#ifndef POAC_CONFIG_HPP
#define POAC_CONFIG_HPP

#ifndef POAC_PROJECT_ROOT
#   warning "POAC_PROJECT_ROOT is not defined"
#endif
#ifndef POAC_VERSION
#   warning "POAC_VERSION is not defined"
#endif
//#define DEBUG

namespace poac {
    static constexpr char const* POAC_API_HOST =
            "poac.io";
    static constexpr char const* POAC_PACKAGES_API =
            "/api/packages/";
    static constexpr char const* POAC_TOKENS_VALIDATE_API =
            "/api/tokens/validate";

    static constexpr char const* POAC_UPLOAD_API_HOST =
            "poac-pm.appspot.com";
    static constexpr char const* POAC_UPLOAD_API =
            "/packages/upload";

    static constexpr char const* POAC_STORAGE_HOST =
            "storage.googleapis.com";

    static constexpr char const* ALGOLIA_APPLICATION_ID =
            "IOCVK5FECM";
    static constexpr char const* ALGOLIA_SEARCH_ONLY_KEY =
            "9c0a76bacf692daa9e8eca2aaff4b2ab";
    static constexpr char const* ALGOLIA_INDEX_NAME =
            "packages";
    static constexpr char const* ALGOLIA_SEARCH_INDEX_API_HOST =
            "IOCVK5FECM-dsn.algolia.net";
    static constexpr char const* ALGOLIA_SEARCH_INDEX_API =
            "/1/indexes/packages/query";
}
#endif // !POAC_CONFIG_HPP

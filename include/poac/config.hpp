#ifndef POAC_CONFIG_HPP
#define POAC_CONFIG_HPP

#ifndef POAC_VERSION
#   warning "POAC_VERSION is not defined"
#endif

namespace poac {
    constexpr char const* ALGOLIA_APPLICATION_ID =
            "IOCVK5FECM";
    constexpr char const* ALGOLIA_SEARCH_ONLY_KEY =
            "9c0a76bacf692daa9e8eca2aaff4b2ab";
    constexpr char const* ALGOLIA_INDEX_NAME =
            "packages";
    constexpr char const* ALGOLIA_SEARCH_INDEX_API_HOST =
            "IOCVK5FECM-dsn.algolia.net";
    constexpr char const* ALGOLIA_SEARCH_INDEX_API =
            "/1/indexes/packages/query";
}
#endif // !POAC_CONFIG_HPP

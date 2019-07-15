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
    constexpr char const* POAC_API_HOST =
            "api.poac.pm";
    constexpr char const* POAC_ARCHIVE_API =
            "/packages/archive";
    constexpr char const* POAC_DEPS_API =
            "/packages/deps";
    constexpr char const* POAC_EXISTS_API =
            "/packages/exists";
    constexpr char const* POAC_VERSIONS_API =
            "/packages/versions";
    constexpr char const* POAC_REGISTER_API =
            "/packages/register";
    constexpr char const* POAC_TOKENS_VALIDATE_API =
            "/tokens/validate";

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

    constexpr char const* GITHUB_API_HOST =
            "api.github.com";
    constexpr char const* GITHUB_REPOS_API =
            "/repos";
}
#endif // !POAC_CONFIG_HPP

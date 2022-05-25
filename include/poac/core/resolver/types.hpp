#ifndef POAC_CORE_RESOLVER_TYPES_HPP_
#define POAC_CORE_RESOLVER_TYPES_HPP_

// internal
#include "poac/core/resolver/resolve.hpp"

namespace poac::core::resolver {

using ResolvedDeps = resolve::UniqDeps<resolve::WithDeps>;

} // namespace poac::core::resolver

#endif // POAC_CORE_RESOLVER_TYPES_HPP_

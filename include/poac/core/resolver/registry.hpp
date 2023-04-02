#pragma once

// external
#include <boost/functional/hash.hpp>

// internal
#include "poac/util/result.hpp"
#include "poac/util/rustify.hpp"

namespace poac::core::resolver::registry {

struct Registry {
  String index;
  String type;
};

inline Fn operator==(const Registry& lhs, const Registry& rhs)->bool {
  return lhs.index == rhs.index && lhs.type == rhs.type;
}

inline Fn hash_value(const Registry& r)->usize {
  usize seed = 0;
  boost::hash_combine(seed, r.index);
  boost::hash_combine(seed, r.type);
  return seed;
}

using Registries = HashMap<String, Registry>;

} // namespace poac::core::resolver::registry

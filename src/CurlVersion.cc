#include "CurlVersion.hpp"

#include <ostream>

namespace curl {

std::ostream&
operator<<(std::ostream& os, const Version& version) {
  if (version.mData) {
    os << version.mData->version << " (ssl: ";
    if (version.mData->ssl_version) {
      os << version.mData->ssl_version;
    } else {
      os << "none";
    }
    os << ")";
  }
  return os;
}

}; // namespace curl

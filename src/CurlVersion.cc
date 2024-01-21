#include "CurlVersion.hpp"

namespace curl {

std::ostream&
operator<<(std::ostream& os, const Version& version) {
  if (version.data) {
    os << version.data->version << " (ssl: ";
    if (version.data->ssl_version) {
      os << version.data->ssl_version;
    } else {
      os << "none";
    }
    os << ")";
  }
  return os;
}

}; // namespace curl

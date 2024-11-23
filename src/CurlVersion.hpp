#pragma once

#include <curl/curl.h>
#include <ostream>

namespace curl {

struct Version {
  curl_version_info_data* data;

  Version() : data(curl_version_info(CURLVERSION_NOW)) {}
};

std::ostream& operator<<(std::ostream& os, const Version& version);

};  // namespace curl

#ifndef POAC_UTIL_NETIO_HPP
#define POAC_UTIL_NETIO_HPP

#include <iostream>
#include <string>

#include <curl/curl.h>


namespace poac::util::netio {
    size_t callbackWrite(char* ptr, size_t size, size_t nmemb, std::string* stream) {
        int dataLength = size * nmemb;
        stream->append(ptr, dataLength);
        return dataLength;
    }

    // TODO: SSL ?
    std::string get(const std::string& url) {
        std::string chunk;
        if (CURL* curl = curl_easy_init(); curl != nullptr) {
            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, callbackWrite);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &chunk);
            if (CURLcode ret = curl_easy_perform(curl); ret != CURLE_OK)
                std::cerr << "curl_easy_perform() failed." << std::endl;
            curl_easy_cleanup(curl);
        }
        return chunk;
    }
} // end namespace
#endif // !POAC_UTIL_NETIO_HPP

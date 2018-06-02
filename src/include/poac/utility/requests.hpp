#ifndef POAC_UTILITY_REQUESTS_HPP
#define POAC_UTILITY_REQUESTS_HPP

#include <iostream>
#include <string>

#include <curl/curl.h>


namespace poac::utility::requests {
    size_t callbackWrite(char *ptr, size_t size, size_t nmemb, std::string *stream) {
        int dataLength = size * nmemb;
        stream->append(ptr, dataLength);
        return dataLength;
    }

    void get() {
        std::string chunk;
        if (CURL* curl = curl_easy_init(); curl != nullptr) {
//            curl_easy_setopt(curl, CURLOPT_URL, "https://poac.pm/api/v1/");
            curl_easy_setopt(curl, CURLOPT_URL, "https://www.google.co.jp/");
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, callbackWrite);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &chunk);
            if (CURLcode ret = curl_easy_perform(curl); ret != CURLE_OK)
                std::cerr << "curl_easy_perform() failed." << std::endl;
            curl_easy_cleanup(curl);
        }
        std::cout << chunk << std::endl;
    }
} // end namespace
#endif // !POAC_UTILITY_REQUESTS_HPP
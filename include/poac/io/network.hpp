#ifndef POAC_IO_NETWORK_HPP
#define POAC_IO_NETWORK_HPP

#include <iostream>
#include <string>
#include <memory>
#include <map>
#include <cstdio>

#include <curl/curl.h>
#include <boost/filesystem.hpp>

#include "../util/command.hpp"


namespace poac::io::network {
    size_t callback_write(char* ptr, size_t size, size_t nmemb, std::string* stream) {
        int dataLength = size * nmemb;
        stream->append(ptr, dataLength);
        return dataLength;
    }

    // TODO: Check if connecting network

    std::string get(const std::string& url) {
        std::string chunk;
        if (CURL* curl = curl_easy_init(); curl != nullptr) {
            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
            curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, callback_write);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &chunk);
            if (CURLcode res = curl_easy_perform(curl); res != CURLE_OK)
                std::cerr << "curl_easy_perform() failed." << std::endl;
            curl_easy_cleanup(curl);
        }
        return chunk;
    }
    std::string get_github(const std::string& url) {
        std::string chunk;
        std::string useragent(std::string("curl/") + curl_version());
        if (CURL* curl = curl_easy_init(); curl != nullptr) {
            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
            curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
            curl_easy_setopt(curl, CURLOPT_USERAGENT, &useragent);
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, callback_write);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &chunk);
            if (CURLcode res = curl_easy_perform(curl); res != CURLE_OK)
                std::cerr << "curl_easy_perform() failed." << std::endl;
            curl_easy_cleanup(curl);
        }
        return chunk;
    }

    void get_file(const std::string& from_url, const boost::filesystem::path& to_file) {
        if (CURL* curl = curl_easy_init(); curl != nullptr) {
            FILE* fp = std::fopen(to_file.c_str(), "wb");
            curl_easy_setopt(curl, CURLOPT_URL, from_url.c_str());
            curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, std::fwrite);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
            // Switch on full protocol/debug output
//            curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
            if (CURLcode res = curl_easy_perform(curl); res != CURLE_OK)
                std::cerr << "curl told us " << res << std::endl;
            curl_easy_cleanup(curl);
            std::fclose(fp);
        }
    }

    void clone(
            const std::string& url,
            const boost::filesystem::path& dest,
            const std::map<std::string, std::string>& opts=std::map<std::string, std::string>())
    {
        std::string options;
        for (const auto& [ key, val ] : opts) {
            options.append(key + " " + val + " ");
        }
        util::command("git clone " + options + url + " " + dest.string()).stderr_to_stdout().exec();
    }

    std::pair<std::string, std::string> opt_branch(const std::string& tag) {
        return std::make_pair("-b", tag);
    }
    std::pair<std::string, std::string> opt_depth(const unsigned int& d) {
        return std::make_pair("--depth", std::to_string(d));
    }
} // end namespace
#endif // !POAC_IO_NETWORK_HPP

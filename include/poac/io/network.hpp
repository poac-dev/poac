#ifndef POAC_IO_NETWORK_HPP
#define POAC_IO_NETWORK_HPP

#include <iostream>
#include <string>
#include <memory>
#include <map>
#include <cstdio>

// TODO: 依存性はあるのか？？
#include <sys/stat.h>

#include <curl/curl.h>
#include <boost/filesystem.hpp>

#include "../core/exception.hpp"
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

    std::string post(const std::string& url, const std::string& json) {
        std::string chunk;
        if (CURL* curl = curl_easy_init(); curl != nullptr) {
            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
            curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json.c_str());
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
            // follow HTTP 3xx redirects
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

    void post_file(const std::string& to_url, const boost::filesystem::path& from_file) {
        struct stat file_info;
        curl_off_t speed_upload, total_time;

        if (CURL* curl = curl_easy_init(); curl != nullptr) {
            FILE* fp = std::fopen(from_file.c_str(), "rb");
            if(!fp)
                throw core::exception::error("could not open the file");
            if(fstat(fileno(fp), &file_info) != 0)
                throw core::exception::error("could not get the file size");

            // upload to this place
            curl_easy_setopt(curl, CURLOPT_URL, to_url.c_str());
            // tell it to "upload" to the URL
            curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);
            // follow HTTP 3xx redirects
            curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
            // set where to read from (on Windows you need to use READFUNCTION too)
            curl_easy_setopt(curl, CURLOPT_READDATA, fp);
            // and give the size of the upload (optional)
            curl_easy_setopt(curl, CURLOPT_INFILESIZE_LARGE, (curl_off_t)file_info.st_size);

            if (CURLcode res = curl_easy_perform(curl); res != CURLE_OK) {
                // TODO: throw????
                std::cerr << "curl told us " << res << std::endl;
            }
            else {
                /* now extract transfer info */
                curl_easy_getinfo(curl, CURLINFO_SPEED_UPLOAD_T, &speed_upload);
                curl_easy_getinfo(curl, CURLINFO_TOTAL_TIME_T, &total_time);

                std::cerr << "Speed: " << speed_upload
                          << " bytes/sec during " << (total_time / 1000000)
                          << (long)(total_time % 1000000) << " seconds" << std::endl;
            }
            curl_easy_cleanup(curl);
            std::fclose(fp);
        }
    }

    void clone(
            const std::string& url,
            const boost::filesystem::path& dest,
            const std::map<std::string, std::string>& opts=
            std::map<std::string, std::string>())
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

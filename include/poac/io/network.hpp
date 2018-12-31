#ifndef POAC_IO_NETWORK_HPP
#define POAC_IO_NETWORK_HPP

#include <iostream>
#include <fstream>
#include <string>
#include <memory>
#include <map>
#include <cstdio>

// TODO: 依存する必要？？
#include <sys/stat.h>

#include <curl/curl.h>
#include <boost/filesystem.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include "../core/exception.hpp"
#include "./cli.hpp"
#include "../util/command.hpp"


namespace poac::io::network {
    size_t callback_write(char* ptr, size_t size, size_t nmemb, std::string* stream) {
        int dataLength = size * nmemb;
        stream->append(ptr, dataLength);
        return dataLength;
    }

    // TODO: Check if connecting network !!!

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

    std::string get(const std::string& url, const std::map<std::string, std::string>& headers_map) {
        std::string chunk;
        struct curl_slist *headers = NULL;
        for (const auto& [k, v] : headers_map) {
            headers = curl_slist_append(headers, (k + ": " + v).c_str());
        }

        if (CURL* curl = curl_easy_init(); curl != nullptr) {
            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
            curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, callback_write);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &chunk);
            if (CURLcode res = curl_easy_perform(curl); res != CURLE_OK)
                std::cerr << "curl_easy_perform() failed." << std::endl;
            curl_easy_cleanup(curl);
        }
        return chunk;
    }

    std::string post(
            const std::string& url,
            const std::string& json,
            const std::vector<std::string>& headers_v={},
            const std::string& content_type="" )
    {
        std::string chunk;
        struct curl_slist *headers = NULL;

        for (const auto& h : headers_v) {
            headers = curl_slist_append(headers, h.c_str());
        }

        if (content_type.empty()) {
            headers = curl_slist_append(headers, "Content-Type: application/json");
        }
        else {
            headers = curl_slist_append(headers, ("Content-Type: " + content_type).c_str());
        }

        if (CURL* curl = curl_easy_init(); curl != nullptr) {
            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
            curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
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

    bool get_file(const std::string& from_url, const boost::filesystem::path& to_file) {
        if (CURL* curl = curl_easy_init(); curl != nullptr) {
            FILE* fp = std::fopen(to_file.c_str(), "wb");
            curl_easy_setopt(curl, CURLOPT_URL, from_url.c_str());
            // follow HTTP 3xx redirects
            curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, std::fwrite);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
            // Switch on full protocol/debug output
//            curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
            if (CURLcode res = curl_easy_perform(curl); res != CURLE_OK) {
                (void)res;
                return EXIT_FAILURE;
            }
            curl_easy_cleanup(curl);
            std::fclose(fp);
        }
        return EXIT_SUCCESS;
    }


    // 1 or 0
    struct data { char trace_ascii; };
    void dump(
        const char* text, FILE* stream,
        unsigned char* ptr, size_t size,
        char nohex )
    {
        size_t i;
        size_t c;

        unsigned int width = 0x10;

        if (nohex)
            // without the hex output, we can fit more on screen
            width = 0x40;

        fprintf(stream, "%s, %10.10lu bytes (0x%8.8lx)\n",
                text, (unsigned long)size, (unsigned long)size);

        for (i = 0; i<size; i += width)
        {
            fprintf(stream, "%4.4lx: ", (unsigned long)i);

            if (!nohex) {
                /* hex not disabled, show it */
                for (c = 0; c < width; c++)
                    if (i + c < size)
                        fprintf(stream, "%02x ", ptr[i + c]);
                    else
                        fputs("   ", stream);
            }

            for (c = 0; (c < width) && (i + c < size); c++) {
                // check for 0D0A; if found, skip past and start a new line of output
                if (nohex && (i + c + 1 < size) &&
                    ptr[i + c] == 0x0D &&
                    ptr[i + c + 1] == 0x0A )
                {
                    i += (c + 2 - width);
                    break;
                }
                fprintf(stream, "%c",
                        (ptr[i + c] >= 0x20) && (ptr[i + c]<0x80)?ptr[i + c]:'.');
                /* check again for 0D0A, to avoid an extra \n if it's at width */
                if (nohex && (i + c + 2 < size) && ptr[i + c + 1] == 0x0D &&
                   ptr[i + c + 2] == 0x0A) {
                    i += (c + 3 - width);
                    break;
                }
            }
            fputc('\n', stream); /* newline */
        }
        fflush(stream);
    }
    int my_trace(
        CURL* handle, curl_infotype type,
        char* data, size_t size,
        void* userp )
    {
        struct data* config = (struct data *)userp;
        const char* text;
        (void)handle; // prevent compiler warning

        switch(type) {
            case CURLINFO_TEXT:
                fprintf(stderr, "== Info: %s", data);
                // FALLTHROUGH
            default: // in case a new one is introduced to shock us
                return 0;

            case CURLINFO_HEADER_OUT:
                text = "=> Send header";
                break;
            case CURLINFO_DATA_OUT:
                text = "=> Send data";
                break;
            case CURLINFO_SSL_DATA_OUT:
                text = "=> Send SSL data";
                break;
            case CURLINFO_HEADER_IN:
                text = "<= Recv header";
                break;
            case CURLINFO_DATA_IN:
                text = "<= Recv data";
                break;
            case CURLINFO_SSL_DATA_IN:
                text = "<= Recv SSL data";
                break;
        }
        dump(text, stderr, (unsigned char *)data, size, config->trace_ascii);
        return 0;
    }

    void verbose_func(CURL* curl) {
        curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
        curl_easy_setopt(curl, CURLOPT_DEBUGFUNCTION, my_trace);
        struct data config;
        config.trace_ascii = 1; /* enable ascii tracing */
        curl_easy_setopt(curl, CURLOPT_DEBUGDATA, &config);
    }
    // now extract transfer info
    void transfer_info(CURL* curl) {
        double speed;
        curl_easy_getinfo(curl, CURLINFO_SPEED_UPLOAD, &speed);
        double total;
        curl_easy_getinfo(curl, CURLINFO_TOTAL_TIME, &total);

        std::cout << "Speed: " << speed << " bytes/sec during "
                  << total << " seconds" << std::endl;
    }

    void post_file(
        const std::string& to_url,
        const std::string& from_file,
        const std::string& config,
        const std::string& token,
        const bool verbose=false )
    {
        struct curl_httppost* formpost = nullptr;
        struct curl_httppost* lastptr = nullptr;
        struct curl_slist *headers = nullptr;

        curl_global_init(CURL_GLOBAL_ALL);

        curl_formadd(&formpost, &lastptr,
                     CURLFORM_COPYNAME, "config",
                     CURLFORM_COPYCONTENTS, config.c_str(),
                     CURLFORM_END);
        curl_formadd(&formpost, &lastptr,
                     CURLFORM_COPYNAME, "token",
                     CURLFORM_COPYCONTENTS, token.c_str(),
                     CURLFORM_END);
        curl_formadd(&formpost, &lastptr,
                     CURLFORM_COPYNAME, "file",
                     CURLFORM_FILE, from_file.c_str(),
                     CURLFORM_END);
        // Fill in the submit field too, even if this is rarely needed
        curl_formadd(&formpost, &lastptr,
                     CURLFORM_COPYNAME, "submit",
                     CURLFORM_COPYCONTENTS, "send",
                     CURLFORM_END);

        if (CURL* curl = curl_easy_init(); curl != nullptr) {
            curl_easy_setopt(curl, CURLOPT_URL, to_url.c_str());
            curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

            headers = curl_slist_append(headers, "Expect:");
            headers = curl_slist_append(headers, "Content-Type: multipart/form-data");
            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

            curl_easy_setopt(curl, CURLOPT_HTTPPOST, formpost);

            if (verbose) verbose_func(curl);


            if (CURLcode res = curl_easy_perform(curl); res != CURLE_OK)
                // TODO: throw????
                std::cerr << "curl told us " << res << std::endl;
            else if (verbose)
                transfer_info(curl);
            curl_easy_cleanup(curl);
            curl_formfree(formpost);
            curl_slist_free_all(headers);
        }

        // TODO: response is written arbitrarily
        std::cout << '\b';
        std::cout << cli::left(100);
        for (int i = 0; i < 100; ++i)
            std::cout << ' ';
        std::cout << cli::left(100);
    }

    bool clone(
        const std::string& url,
        const boost::filesystem::path& dest,
        const std::map<std::string, std::string>& opts=
            std::map<std::string, std::string>() )
    {
        std::string options;
        for (const auto& [ key, val ] : opts) {
            options.append(key + " " + val + " ");
        }

        if (util::command("git clone " + options + url + " " + dest.string()).stderr_to_stdout().exec())
            return EXIT_SUCCESS;
        else
            return EXIT_FAILURE;
    }

    std::pair<std::string, std::string> opt_branch(const std::string& tag) {
        return std::make_pair("-b", tag);
    }
    std::pair<std::string, std::string> opt_depth(const unsigned int& d) {
        return std::make_pair("--depth", std::to_string(d));
    }
} // end namespace
#endif // !POAC_IO_NETWORK_HPP

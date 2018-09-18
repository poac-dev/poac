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
        struct curl_slist *headers = NULL;
        // TODO: from arguments
        headers = curl_slist_append(headers, "Content-Type: application/json");
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

    static bool get_file(const std::string& from_url, const boost::filesystem::path& to_file) {
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
        // TODO:
        return EXIT_SUCCESS;
    }


    // 1 or 0
    struct data { char trace_ascii; };
    static void dump(
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
    static int my_trace(
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

    std::string post_file(
        const std::string& to_url,
        const boost::filesystem::path& from_file,
        const bool verbose=false )
    {
        struct stat file_info;
        struct curl_slist *headers = NULL;
        headers = curl_slist_append(headers, "Expect:");

        if (CURL* curl = curl_easy_init(); curl != nullptr) {
            FILE* fd = std::fopen(from_file.c_str(), "rb");
            if (!fd)
                throw core::exception::error("could not open the file");
            if (fstat(fileno(fd), &file_info) != 0)
                throw core::exception::error("could not get the file size");

            curl_easy_setopt(curl, CURLOPT_URL, to_url.c_str());
            curl_easy_setopt(curl, CURLOPT_POST, 1L);
            curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
            curl_easy_setopt(curl, CURLOPT_READDATA, fd);
            curl_easy_setopt(curl, CURLOPT_INFILESIZE_LARGE, (curl_off_t)file_info.st_size);
            curl_easy_setopt(curl, CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_1_0);

            if (verbose) {
                curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
                curl_easy_setopt(curl, CURLOPT_DEBUGFUNCTION, my_trace);
                struct data config;
                config.trace_ascii = 1; /* enable ascii tracing */
                curl_easy_setopt(curl, CURLOPT_DEBUGDATA, &config);
            }

            if (CURLcode res = curl_easy_perform(curl); res != CURLE_OK) {
                // TODO: throw????
                std::cerr << "curl told us " << res << std::endl;
            }
            else {
                /* now extract transfer info */
                double speed;
                curl_easy_getinfo(curl, CURLINFO_SPEED_UPLOAD, &speed);
                double total;
                curl_easy_getinfo(curl, CURLINFO_TOTAL_TIME, &total);

                std::cerr << "Speed: " << speed << " bytes/sec during "
                          << total << " seconds" << std::endl;
            }
            curl_easy_cleanup(curl);
            std::fclose(fd);
        }
        return "";
    }

    std::string post_post(
        const std::string& to_url,
        const boost::filesystem::path& from_file,
        const bool verbose=false )
    {
        struct stat file_info;
        struct curl_httppost* post = NULL;
        struct curl_httppost* last = NULL;
        struct curl_slist *headers = NULL;
        headers = curl_slist_append(headers, "Expect:");

        if (CURL* curl = curl_easy_init(); curl != nullptr) {
            FILE* fd = std::fopen(from_file.c_str(), "rb");
            if (!fd)
                throw core::exception::error("could not open the file");
            if (fstat(fileno(fd), &file_info) != 0)
                throw core::exception::error("could not get the file size");

            curl_formadd(&post, &last,
                         CURLFORM_COPYNAME, "name",
                         CURLFORM_BUFFER, "data",
                         CURLFORM_BUFFERPTR, fd,
                         CURLFORM_BUFFERLENGTH, (curl_off_t)file_info.st_size,
                         CURLFORM_END);

            curl_easy_setopt(curl, CURLOPT_URL, to_url.c_str());
            curl_easy_setopt(curl, CURLOPT_POST, 1L);
            curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
//            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
//            curl_easy_setopt(curl, CURLOPT_READDATA, fd);
//            curl_easy_setopt(curl, CURLOPT_INFILESIZE_LARGE, (curl_off_t)file_info.st_size);
            curl_easy_setopt(curl, CURLOPT_HTTPPOST, post);
            curl_easy_setopt(curl, CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_1_0);

            if (verbose) {
                curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
                curl_easy_setopt(curl, CURLOPT_DEBUGFUNCTION, my_trace);
                struct data config;
                config.trace_ascii = 1; /* enable ascii tracing */
                curl_easy_setopt(curl, CURLOPT_DEBUGDATA, &config);
            }

            if (CURLcode res = curl_easy_perform(curl); res != CURLE_OK) {
                // TODO: throw????
                std::cerr << "curl told us " << res << std::endl;
            }
            else {
                /* now extract transfer info */
                double speed;
                curl_easy_getinfo(curl, CURLINFO_SPEED_UPLOAD, &speed);
                double total;
                curl_easy_getinfo(curl, CURLINFO_TOTAL_TIME, &total);

                std::cerr << "Speed: " << speed << " bytes/sec during "
                          << total << " seconds" << std::endl;
            }
            curl_easy_cleanup(curl);
            std::fclose(fd);
        }
        return "";
    }

    int http2post(
        const std::string& to_url,
        const std::string& uuid,
        const boost::filesystem::path& from_file,
        const bool verbose=false )
    {
        std::ifstream ifs(from_file.string(), std::ios::in | std::ios::binary);
        if (!ifs){
            std::cout << "Cloud not open " + from_file.string();
            return 1;
        }
        std::ofstream ofs("hoge.tar.gz", std::ios::binary | std::ios::out);

        std::array<char, 1024*100> buffer;
        int count = 0;
        do {
            ifs.read(buffer.data(), 1024*100);
            boost::property_tree::ptree json;
            json.put("uuid", uuid);
            json.put("count", count++);
            json.put("file", buffer.data());
            std::stringstream ss;
            boost::property_tree::json_parser::write_json(ss, json, false);

            ofs << buffer.data();

            const std::string res = post(to_url, ss.str());

            if (verbose) {
                std::cout << "=> Send data" << std::endl
                          << ss.str() << std::endl
                          << "<= Recv data" << std::endl
                          << res << std::endl;
            }
        } while (!ifs.eof());

        boost::property_tree::ptree json;
        json.put("uuid", "ok");
        json.put("count", -1);
        json.put("file", "");
        std::stringstream ss;
        boost::property_tree::json_parser::write_json(ss, json, false);
        const std::string res = post(to_url, ss.str());
        if (verbose) {
            std::cout << "=> Send data" << std::endl
                      << ss.str() << std::endl
                      << "<= Recv data" << std::endl
                      << res << std::endl;
        }

        return 0;
    }


    static bool clone(
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

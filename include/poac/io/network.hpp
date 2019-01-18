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
#include <boost/asio.hpp>
#define BOOST_COROUTINES_NO_DEPRECATION_WARNING // https://stackoverflow.com/a/38996654
#include <boost/asio/spawn.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl/stream.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/version.hpp>
#include <boost/beast/http.hpp>

#include "../core/exception.hpp"
#include "./cli.hpp"
#include "../util/command.hpp"


namespace poac::io::network {
    namespace ssl = boost::asio::ssl;
    namespace http = boost::beast::http;

    using Headers = std::map<http::field, std::string>;
    using Request = http::request<http::string_body>;

    template <typename S>
    std::string request(const Request& req, S host) {
        // The io_context is required for all I/O
        boost::asio::io_context ioc;
        // The SSL context is required, and holds certificates
        ssl::context ctx{ ssl::context::sslv23 };
        // These objects perform our I/O
        boost::asio::ip::tcp::resolver resolver{ ioc };
        ssl::stream<boost::asio::ip::tcp::socket> stream{ ioc, ctx };

        // Set SNI Hostname (many hosts need this to handshake successfully)
        if(!SSL_set_tlsext_host_name(stream.native_handle(), host))
        {
            boost::system::error_code ec{
                static_cast<int>(::ERR_get_error()), boost::asio::error::get_ssl_category()
            };
            throw boost::system::system_error{ ec };
        }
        // Look up the domain name
        const auto port = "443";
        const auto results = resolver.resolve(host, port);

        // Make the connection on the IP address we get from a lookup
        boost::asio::connect(stream.next_layer(), results.begin(), results.end());
        // Perform the SSL handshake
        stream.handshake(ssl::stream_base::client);

        // Send the HTTP request to the remote host
        http::write(stream, req);

        // This buffer is used for reading and must be persisted
        boost::beast::flat_buffer buffer;
        // Declare a container to hold the response
        // http::response<http::dynamic_body> res;
        http::response<http::string_body> res;
        // Receive the HTTP response
        http::read(stream, buffer, res);

        // Gracefully close the stream
        boost::system::error_code ec;
        stream.shutdown(ec);
        if (ec == boost::asio::error::eof) {
            // Rationale: https://stackoverflow.com/q/25587403
            ec.assign(0, ec.category());
        }
        // return boost::beast::buffers_to_string(res.body().data());
        return res.body().data();
    }

    std::string get(const std::string& target, const Headers& headers={}) {
        // Set up an HTTP GET request message, 10 -> HTTP/1.0, 11 -> HTTP/1.1
        Request req{ http::verb::get, target, 11 };
        req.set(http::field::host, POAC_API_HOST);
        req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);
        for (const auto& [field, string_param] : headers) {
            req.set(field, string_param);
        }
        return request(req, POAC_API_HOST);
    }

    std::string post(const std::string& target, std::string body, const Headers& headers={}) {
        // Set up an HTTP GET request message, 10 -> HTTP/1.0, 11 -> HTTP/1.1
        Request req{ http::verb::post, target, 11 };
        req.set(http::field::host, POAC_API_HOST);
        req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);
        req.set(http::field::content_type, "application/json");
        req.set(http::field::accept, "*/*");
        for (const auto& [field, string_param] : headers) {
            req.set(field, string_param);
        }
        body.erase(std::remove(body.begin(), body.end(), '\n'), body.end());
        req.body() = body;
        req.prepare_payload();
        return request(req, POAC_API_HOST);
    }
    template <typename S>
    std::string post(Request req, S host, std::string body) {
        req.method(http::verb::post);
        req.version(11);
        req.set(http::field::host, host);
        req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);
        body.erase(std::remove(body.begin(), body.end(), '\n'), body.end());
        req.body() = body;
        req.prepare_payload();
        return request(req, host);
    }

    Request custom_request() {
        return Request{};
    }


//    std::string get_github(const std::string& url) {
//        std::string chunk;
//        std::string useragent(std::string("curl/") + curl_version());
//        if (CURL* curl = curl_easy_init(); curl != nullptr) {
//            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
//            curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
//            curl_easy_setopt(curl, CURLOPT_USERAGENT, &useragent);
//            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, callback_write);
//            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &chunk);
//            if (CURLcode res = curl_easy_perform(curl); res != CURLE_OK)
//                std::cerr << "curl_easy_perform() failed." << std::endl;
//            curl_easy_cleanup(curl);
//        }
//        return chunk;
//    }

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

    void post_file(
        const std::string& to_url,
        const std::string& from_file,
        const std::string& config,
        const std::string& token,
        [[maybe_unused]] const bool verbose=false )
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

            if (CURLcode res = curl_easy_perform(curl); res != CURLE_OK) {
                // TODO: throw????
                std::cerr << "curl told us " << res << std::endl;
            }
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
            std::map<std::string, std::string>{} )
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

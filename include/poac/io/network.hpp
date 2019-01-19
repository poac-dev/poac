#ifndef POAC_IO_NETWORK_HPP
#define POAC_IO_NETWORK_HPP

#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <variant>

#include <curl/curl.h>
#include <boost/filesystem.hpp>
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl/stream.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/version.hpp>
#include <boost/beast/http.hpp>

#include "../core/exception.hpp"
#include "./cli.hpp"


namespace poac::io::network {
    namespace ssl = boost::asio::ssl;
    namespace http = boost::beast::http;

    using Headers = std::map<std::variant<http::field, std::string>, std::string>;
    using Request = http::request<http::string_body>;

    template <typename ResponseBody, typename S>
    typename ResponseBody::value_type
    request(const Request& req, S host) {
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
        http::response<ResponseBody> res;
        // Receive the HTTP response
        http::read(stream, buffer, res);

        // Gracefully close the stream
        boost::system::error_code ec;
        stream.shutdown(ec);
        if (ec == boost::asio::error::eof) {
            // Rationale: https://stackoverflow.com/q/25587403
            ec.assign(0, ec.category());
        }
        return res.body();
    }


    Request create_get_request(
            const std::string& target,
            const std::string& host=POAC_API_HOST,
            const Headers& headers={})
    {
        // Set up an HTTP GET request message, 10 -> HTTP/1.0, 11 -> HTTP/1.1
        Request req{ http::verb::get, target, 11 };
        req.set(http::field::host, host);
        req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);
        for (const auto& [field, string_param] : headers) {
            std::visit([&, s=string_param](auto f) { req.set(f, s); }, field);
        }
        return req;
    }
    std::string get(
            const std::string& target,
            const std::string& host=POAC_API_HOST,
            const Headers& headers={})
    {
        const auto req = create_get_request(target, host, headers);
        const auto res = request<http::string_body>(req, host.c_str());
        return res.data();
    }
    void get(
            const std::string& target,
            const boost::filesystem::path& out,
            const std::string& host=POAC_API_HOST,
            const Headers& headers={})
    {
        const auto req = create_get_request(target, host, headers);
        const auto res = request<http::vector_body<unsigned char>>(req, host.c_str());
        std::ofstream output_file(out.string(), std::ofstream::out | std::ofstream::binary);
        for (const auto& r : res) {
            output_file << r;
        }
    }

    std::string post(
            const std::string& target,
            std::string body,
            const std::string& host=POAC_API_HOST,
            const Headers& headers={})
    {
        // Set up an HTTP GET request message, 10 -> HTTP/1.0, 11 -> HTTP/1.1
        Request req{ http::verb::post, target, 11 };
        req.set(http::field::host, host);
        req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);
        req.set(http::field::content_type, "application/json");
        for (const auto& [field, string_param] : headers) {
            std::visit([&, s=string_param](auto f) { req.set(f, s); }, field);
        }
        body.erase(std::remove(body.begin(), body.end(), '\n'), body.end());
        req.body() = body;
        req.prepare_payload();

        const auto res = request<http::string_body>(req, host.c_str());
        return res.data();
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
} // end namespace
#endif // !POAC_IO_NETWORK_HPP

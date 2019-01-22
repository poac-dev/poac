#ifndef POAC_IO_NETWORK_HPP
#define POAC_IO_NETWORK_HPP

#include <iostream>
#include <fstream>
#include <string>
#include <string_view>
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
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/lexical_cast.hpp>

#include "../core/exception.hpp"
#include "./cli.hpp"


namespace poac::io::network {
    namespace ssl = boost::asio::ssl;
    namespace http = boost::beast::http;

    using Headers = std::map<std::variant<http::field, std::string>, std::string>;

    template <typename ResponseBody, typename RequestBody>
    typename ResponseBody::value_type
    request(const http::request<RequestBody>& req, std::string_view host)
    {
        // The io_context is required for all I/O
        boost::asio::io_context ioc;
        // The SSL context is required, and holds certificates
        ssl::context ctx{ ssl::context::sslv23 };
        // These objects perform our I/O
        boost::asio::ip::tcp::resolver resolver{ ioc };
        ssl::stream<boost::asio::ip::tcp::socket> stream{ ioc, ctx };

        // Set SNI Hostname (many hosts need this to handshake successfully)
        if(!SSL_set_tlsext_host_name(stream.native_handle(), std::string(host).c_str()))
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


    template <typename Body>
    http::request<Body>
    create_request(
            http::verb method,
            std::string_view target,
            std::string_view host=POAC_API_HOST,
            const Headers& headers={})
    {
        // Set up an HTTP request message, 10 -> HTTP/1.0, 11 -> HTTP/1.1
        http::request<Body> req{ method, std::string(target), 11 };
        req.set(http::field::host, host);
        req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);
        for (const auto& [field, string_param] : headers) {
            std::visit([&, s=string_param](auto f) { req.set(f, s); }, field);
        }
        return req;
    }

    std::string get(
            std::string_view target,
            std::string_view host=POAC_API_HOST,
            const Headers& headers={})
    {
        const auto req = create_request<http::string_body>(http::verb::get, target, host, headers);
        const auto res = request<http::string_body>(req, host);
        return res.data();
    }
    void get(
            std::string_view target,
            const boost::filesystem::path& out,
            std::string_view host=POAC_API_HOST,
            const Headers& headers={})
    {
        const auto req = create_request<http::string_body>(http::verb::get, target, host, headers);
        const auto res = request<http::vector_body<unsigned char>>(req, host);
        std::ofstream output_file(out.string(), std::ios::out | std::ios::binary);
        for (const auto& r : res) {
            output_file << r;
        }
    }

    std::string post(
            std::string_view target,
            std::string body,
            std::string_view host=POAC_API_HOST,
            const Headers& headers={})
    {
        auto req = create_request<http::string_body>(http::verb::post, target, host, headers);
        req.set(http::field::content_type, "application/json");
        body.erase(std::remove(body.begin(), body.end(), '\n'), body.end());
        req.body() = body;
        req.prepare_payload();

        const auto res = request<http::string_body>(req, host);
        return res.data();
    }


    void PostFile( // TODO: WIP
            const std::string& token,
            const std::string& from_file,
            std::string_view target="/post",
            std::string_view host="httpbin.org")
    {
        auto req = create_request<http::empty_body>(http::verb::post, target, host);

        const std::string boundary = boost::lexical_cast<std::string>(boost::uuids::random_generator()());
        std::string content_disposition =
                "form-data; name=\"token\"\r\n"
                "\r\n"
                + token + "\r\n"
                "--" + boundary + "\r\n"
                "Content-Disposition: form-data; name=\"file\"; filename=\"" + from_file + "\"\r\n"
                "Content-Type: application/x-gzip\r\n"
                "Content-Transfer-Encoding: binary\r\n"
                "\r\n";

//        std::ifstream file(from_file, std::ios::in | std::ios::binary);
//        std::string binary((std::istreambuf_iterator<char>(file)),
//                            std::istreambuf_iterator<char>());
//        content_disposition += binary;

//        http::request_serializer<http::empty_body> sr{ req }; // TODO: chunked
//        sr.

        content_disposition += "\r\n--" + boundary + "--" + "\r\n"; // footer

        req.set(http::field::content_type, "multipart/form-data; boundary=" + boundary);
        // "Content-Disposition: " + "\r\n\r\n--" -> 27
        const auto content_length = 27 + boundary.length() + content_disposition.length();
        req.set(http::field::content_length, std::to_string(content_length) + "\r\n\r\n--" + boundary);
        req.set(http::field::content_disposition, content_disposition);

        std::cout << req << std::endl;
//        const auto res = request<http::string_body>(req, host);
//        std::cout << res << std::endl;
    }

    void post_file(
        const std::string& from_file,
        const std::string& token)
    {
        struct curl_httppost* formpost = nullptr;
        struct curl_httppost* lastptr = nullptr;
        struct curl_slist* headers = nullptr;

        curl_global_init(CURL_GLOBAL_ALL);
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
            curl_easy_setopt(curl, CURLOPT_URL, POAC_PACKAGE_UPLOAD_API);
            curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

            headers = curl_slist_append(headers, "Expect:");
            headers = curl_slist_append(headers, "Content-Type: multipart/form-data");
            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
            curl_easy_setopt(curl, CURLOPT_HTTPPOST, formpost);
            if (CURLcode res = curl_easy_perform(curl); res != CURLE_OK) {
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

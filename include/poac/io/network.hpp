#ifndef POAC_IO_NETWORK_HPP
#define POAC_IO_NETWORK_HPP

#include <iostream>
#include <fstream>
#include <string>
#include <string_view>
#include <sstream>
#include <map>
#include <variant>
#include <optional>

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
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include "../core/exception.hpp"
#include "./cli.hpp"
#include "../util/misc.hpp"
#include "../util/types.hpp"
#include "../util/pretty.hpp"


namespace poac::io::network {
    namespace ssl = boost::asio::ssl;
    namespace http = boost::beast::http;

    using Headers = std::map<std::variant<http::field, std::string>, std::string>;

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
            cli::debugln(ec.message());
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
        http::read(stream, buffer, res); // TODO: ファイルリードでも，一気に読んでしまっている．そのため，以下のcaseでresから，status_codeを読める．

        switch (res.base().result_int() / 100) {
            case 2: {
                // Gracefully close the stream
                boost::system::error_code ec;
                stream.shutdown(ec);
                if (ec == boost::asio::error::eof) {
                    // Rationale: https://stackoverflow.com/q/25587403
                    ec.assign(0, ec.category());
                }
                return res.body();
            }
            case 3: {
                const std::string new_location = res.base()["Location"].to_string();
                cli::debugln("Redirect to ", new_location);
                // https://api.poac.pm/packages/deps -> api.poac.pm
                const std::string new_host = util::misc::split(new_location, "://")[1];
                // https://api.poac.pm/packages/deps -> /packages/deps
                const auto host_pos = new_location.find(new_host);
                const std::string new_target(new_location, host_pos+new_host.size());

                // TODO: header情報が消えている．
                const auto new_req = create_request<RequestBody>(req.method(), new_target, new_host);
                return request<ResponseBody>(new_req, new_host);
            }
            default: {
                // error
                return res.body();
            }
        }
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
            std::string_view host,
            const Headers& headers={})
    {
        const auto req = create_request<http::string_body>(http::verb::get, target, host, headers);
        const auto res = request<http::vector_body<unsigned char>>(req, host);
        std::ofstream output_file(out.string(), std::ios::out | std::ios::binary);

//        std::cout << res.size() << std::endl;
//        TODO: request() 中に，Downloading [====>     ] 11.22MB/32.37MB 的なのがほしい．docker pullと似た感じ

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


    std::string post_file(
            const std::string& token,
            const boost::filesystem::path& file_path,
            std::string_view target=POAC_UPLOAD_API,
            std::string_view host=POAC_API_HOST)
    {
        namespace fs = boost::filesystem;

        const std::string CRLF = "\r\n";
        const std::string boundary = boost::lexical_cast<std::string>(boost::uuids::random_generator{}());
        const std::string boundary_footer = CRLF + "--" + boundary + "--" + CRLF; // footer
        const std::string content_disposition = "Content-Disposition: form-data; ";

        std::string token_header;
        {
            std::stringstream token_header_ss;
            token_header_ss << "--" << boundary << CRLF
                << content_disposition << "name=\"token\"" << CRLF << CRLF
                << token;
            token_header = token_header_ss.str();
        }

        std::string file_header;
        {
            std::stringstream file_header_ss;
            file_header_ss << CRLF << "--" << boundary << CRLF
                << content_disposition << "name=\"file\"; filename=\"" << file_path.filename().string() << "\"" << CRLF
                << "Content-Type: application/x-gzip" << CRLF
                << "Content-Transfer-Encoding: binary" << CRLF << CRLF;
            file_header = file_header_ss.str();
        }

        auto req = create_request<http::string_body>(http::verb::post, target, host);
        req.set(http::field::accept, "*/*");
        req.set(http::field::content_type, "multipart/form-data; boundary=" + boundary);
        const auto file_size = fs::file_size(file_path);
        const auto content_length = token_header.size() + file_header.size() + file_size + boundary_footer.size();
        req.set(http::field::content_length, content_length);


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

        // Convert request to string
        std::stringstream reqss;
        reqss << req;
        // Send the HTTP request to the remote host
        stream.write_some(boost::asio::buffer(reqss.str()));
        stream.write_some(boost::asio::buffer(token_header));
        stream.write_some(boost::asio::buffer(file_header));
        { // Read file and write to stream
            std::ifstream file(file_path.string(), std::ios::in | std::ios::binary);
            char buf[512];

            // TODO: この辺りいい感じにまとめる
            const auto [ parsed_byte, byte_unit ] = util::pretty::to_byte(file_size);
            std::cout << std::fixed;
            std::cout << cli::to_info("Uploading ") << cli::to_progress(0, file_size) << " ";
            std::cout << "0" << byte_unit << "/";
            std::cout << std::setprecision(2) << parsed_byte << byte_unit << std::flush;

            unsigned long count = 0;
            while (!file.eof()) {
                file.read(buf, 512);
                stream.write_some(boost::asio::buffer(buf, file.gcount()));

                unsigned long cur_file_size = ++count * 512;
                if (cur_file_size > file_size) {
                    cur_file_size = file_size;
                }
                const auto [ parsed_cur_byte, cur_byte_unit ] = util::pretty::to_byte(cur_file_size);
                std::cout << std::fixed;
                std::cout << '\r' << cli::clr_line;
                std::cout << cli::to_info("Uploading ") << cli::to_progress(cur_file_size, file_size) << " ";
                std::cout << std::setprecision(2) << parsed_cur_byte << cur_byte_unit << "/";
                std::cout << std::setprecision(2) << parsed_byte << byte_unit << std::flush;
            }
            std::cout << '\r' << cli::clr_line << cli::to_info("Uploaded.") << std::endl;
        }
        // Write footer to stream
        stream.write_some(boost::asio::buffer(boundary_footer));

        // This buffer is used for reading and must be persisted
        boost::beast::flat_buffer buffer;
        // Declare a container to hold the response
        http::response<http::string_body> res;

        cli::echo(cli::to_info("Waiting for server response..."));

        // Receive the HTTP response
        http::read(stream, buffer, res);

        // Gracefully close the stream
        boost::system::error_code ec;
        stream.shutdown(ec);
        if (ec == boost::asio::error::eof) {
            // Rationale: https://stackoverflow.com/q/25587403
            ec.assign(0, ec.category());
        }

        return res.body().data();
    }


    namespace api {
        std::optional<std::vector<std::string>>
        versions(const std::string& name) {
            using namespace std::string_literals;
            boost::property_tree::ptree pt;
            {
                std::stringstream ss;
                ss << io::network::get(POAC_VERSIONS_API + "/"s + name);
                cli::debugln(name, ": ", ss.str());
                if (ss.str() == "null") {
                    return std::nullopt;
                }
                boost::property_tree::json_parser::read_json(ss, pt);
            }
            return util::types::ptree_to_vector<std::string>(pt);
        }

        std::optional<boost::property_tree::ptree>
        deps(const std::string& name, const std::string& version) {
            using namespace std::string_literals;
            std::stringstream ss;
            ss << io::network::get(POAC_DEPS_API + "/"s + name + "/" + version);
            if (ss.str() == "null") {
                return std::nullopt;
            }
            else {
                boost::property_tree::ptree pt;
                boost::property_tree::json_parser::read_json(ss, pt);
                return pt;
            }
        }
    }
} // end namespace
#endif // !POAC_IO_NETWORK_HPP

#ifndef POAC_IO_NETWORK_HPP
#define POAC_IO_NETWORK_HPP

#include <iostream>
#include <fstream>
#include <string>
#include <string_view>
#include <sstream>
#include <map>
#include <memory>
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
    namespace http = boost::beast::http;
    using Headers = std::map<std::variant<http::field, std::string>, std::string>;

    template <typename RequestBody>
    http::request<RequestBody>
    create_request(
            http::verb method,
            std::string_view target,
            std::string_view host=POAC_API_HOST,
            const Headers& headers={})
    {
        // Set up an HTTP request message, 10 -> HTTP/1.0, 11 -> HTTP/1.1
        http::request<RequestBody> req{ method, std::string(target), 11 };
        req.set(http::field::host, host);
        req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);
        for (const auto& [field, string_param] : headers) {
            std::visit([&, s=string_param](auto f) { req.set(f, s); }, field);
        }
        return req;
    }


    namespace ssl = boost::asio::ssl;
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

            // Print progress bar
            std::cout << '\r' << cli::clr_line << cli::to_info("Uploading ");
            cli::echo_byte_progress(file_size, 0);

            unsigned long count = 0;
            while (!file.eof()) {
                file.read(buf, 512);
                stream.write_some(boost::asio::buffer(buf, file.gcount()));

                unsigned long cur_file_size = ++count * 512;
                // Print progress bar
                std::cout << '\r' << cli::clr_line << cli::to_info("Uploading ");
                cli::echo_byte_progress(file_size, cur_file_size);
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



    std::pair<std::string, std::string>
    parse_url(const std::string& url) {
        // https://api.poac.pm/packages/deps -> api.poac.pm
        const std::string host = util::misc::split(url, "://")[1];
        // https://api.poac.pm/packages/deps -> /packages/deps
        const std::string target(url, url.find(host) + host.size());
        return { host, target };
    }

    // Only SSL usage
    class requests { // TODO: classにする意味がなさそう．publish.hppでスコープ作って都度破棄してるから．
    public:
        explicit requests(std::string_view host_=POAC_API_HOST) : host(host_) {
            // The io_context is required for all I/O
            ioc = std::make_unique<boost::asio::io_context>();
            // The SSL context is required, and holds certificates
            ctx = std::make_unique<ssl::context>(ssl::context::sslv23);
            // These objects perform our I/O
            resolver = std::make_unique<boost::asio::ip::tcp::resolver>(*ioc);
            stream = std::make_unique<ssl::stream<boost::asio::ip::tcp::socket>>(*ioc, *ctx);
        }

        template <http::verb method, typename ResponseBody, typename Request, typename Ofstream>
        typename ResponseBody::value_type
        do_(Request&& req, Ofstream&& ofs) const {
            ssl_prepare();
            // TODO: 送信時，ファイルなら，プログレスの付与と，読みながら送信．-> Request::body_typeによって分岐
            write_request(req);
            return read_response<method, ResponseBody>(std::forward<Request>(req), std::forward<Ofstream>(ofs));
        }

        template <typename RequestBody=http::empty_body, typename Ofstream=std::nullptr_t,
                typename ResponseBody=std::conditional_t<
                        std::is_same_v<Ofstream, std::ofstream>, http::empty_body, http::string_body>>
        typename ResponseBody::value_type
        get(std::string_view target, const Headers& headers={}, Ofstream&& ofs=nullptr) const {
            const auto req = create_request<RequestBody>(http::verb::get, target, host, headers);
            cli::debugln(req);
            return do_<http::verb::get, ResponseBody>(std::move(req), std::forward<Ofstream>(ofs));
        }

        // TODO: ここで，RequestBodyが，std::ifstreamであるかの検証をおこなう！
        template <typename ResponseBody, typename RequestBody=http::string_body> // TODO: できれば，RequestBody, ResponseBodyにする
        typename ResponseBody::value_type
        post(std::string_view target, std::string body, const Headers& headers={}) const {
            auto req = create_request<RequestBody>(http::verb::post, target, host, headers);
            req.set(http::field::content_type, "application/json");
            body.erase(std::remove(body.begin(), body.end(), '\n'), body.end());
            req.body() = body;
            req.prepare_payload();
            return do_<http::verb::post, ResponseBody>(std::move(req), nullptr);
        }

    private:
        const std::string CRLF = "\r\n";
        std::string_view port = "443";
        std::string_view host;
        std::unique_ptr<boost::asio::io_context> ioc;
        std::unique_ptr<ssl::context> ctx;
        std::unique_ptr<boost::asio::ip::tcp::resolver> resolver;
        std::unique_ptr<ssl::stream<boost::asio::ip::tcp::socket>> stream;

        template <typename Request>
        void write_request(const Request& req) const {
            cli::debugln("Write type: simple");
            // Send the HTTP request to the remote host
            http::write(*stream, req);
        }

        // simple_write, progress_write


        template <http::verb method, typename ResponseBody, typename Request, typename Ofstream>
        typename ResponseBody::value_type
        read_response(Request&& old_req, Ofstream&& ofs) const {
            if constexpr (!std::is_same_v<Ofstream, std::ofstream>) {
                cli::debugln("Read type: simple");
                return simple_read<method, ResponseBody>(std::forward<Request>(old_req));
            }
            else {
                cli::debugln("Read type: progress");
                return progress_read<method, ResponseBody>(std::forward<Request>(old_req), std::forward<Ofstream>(ofs));
            }
        }

        template <http::verb method, typename ResponseBody, typename Request>
        typename ResponseBody::value_type
        simple_read(Request&& old_req) const {
            // This buffer is used for reading and must be persisted
            boost::beast::flat_buffer buffer;
            // Declare a container to hold the response
            http::response<ResponseBody> res;
            // Receive the HTTP response
            http::read(*stream, buffer, res);
            // Handle HTTP status code
            return handle_status<method, std::nullptr_t>(std::forward<Request>(old_req), std::move(res));
        }

        template <http::verb method, typename ResponseBody, typename Request, typename Ofstream,
                typename RequestBody=typename Request::body_type> // TODO: できれば，RequestBodyを必要無しに．つまり，handle_statusに流す
        typename ResponseBody::value_type
        progress_read(Request&& old_req, Ofstream&& ofs) const {
            // Read the response status line.
            boost::asio::streambuf res;
            boost::asio::read_until(*stream, res, CRLF);

            // Check that response is OK.
            std::istream response_stream(&res);
            std::string http_version;
            response_stream >> http_version;
            unsigned int status_code;
            response_stream >> status_code;
            std::string status_message;
            std::getline(response_stream, status_message);

            // Read the response headers, which are terminated by a blank line.
            boost::asio::read_until(*stream, res, CRLF + CRLF);

//            stream->read_some()

            // TODO: こっちでheaderを読んでしまっておきたい．


            // TODO: const auto res = create_response<>();
            // TODO: return handle_status<method, std::nullptr_t>(std::forward<Request>(old_req), std::move(res));

//            http::response<ResponseBody> res2;
//            res2.base().result(status_code);

            // Handle status code
            switch (status_code / 100) { // TODO: ここを，上のようにhandle_statusで頑張る
                case 2: {
                    // Process the response headers.
                    std::string header;
                    std::string content_length_header = "Content-Length: "; // TODO: ": "でsplitして，左をmapのkeyにする
                    int content_length;
                    while (std::getline(response_stream, header) && header != "\r") {
                        if (const auto pos = header.find(content_length_header); pos != std::string::npos) {
                            content_length = std::stoi(std::string(header, pos + content_length_header.size()));
                        }
                    }

                    unsigned int acc = 0;
                    // Write whatever content we already have to output.
                    if (res.size() > 0) {
                        acc += 1;
                        std::cout << '\r' << cli::clr_line << cli::to_info("Downloading ");
                        cli::echo_byte_progress(content_length, acc);

                        ofs << &res;
                    }

                    // Read until EOF, writing data to output as we go.
                    boost::system::error_code ec;
                    while (boost::asio::read(*stream, res, boost::asio::transfer_exactly(1), ec)) {
                        acc += 1;
                        std::cout << '\r' << cli::clr_line << cli::to_info("Downloading ");
                        cli::echo_byte_progress(content_length, acc);

                        // TODO: note: constexpr if で discarded statement における依存名はこの部分
                        ofs << &res;
                    }

                    close_stream();
                    return {};
                }
                case 3: {
                    // Process the response headers.
                    std::string header;
                    std::string location_header = "Location: ";
                    std::string new_location;
                    while (std::getline(response_stream, header) && header != "\r") {
                        if (const auto pos = header.find(location_header); pos != std::string::npos) {
                            new_location = std::string(header, pos + location_header.size());
                        }
                    }
                    const auto [new_host, new_target] = parse_url(new_location);
                    cli::debugln("Redirect to ", new_location, '\n');

                    // TODO: header情報が消えている．-> ここまで，最初のheaderを運んでもらう？？？
                    const requests req(new_host);
                    if constexpr (method == http::verb::get) {
                        return req.get(new_target, {}, std::forward<Ofstream>(ofs));
                    }
                    else if (method == http::verb::post) {
                        return req.post<ResponseBody, RequestBody>(new_target, old_req.body());
                    }
                    [[fallthrough]]; // verb error
                }
                default: {
                    // TODO: handle error -> Please open issue
                    close_stream();
                    return {};
                }
            }
        }


        template <http::verb method, typename Ofstream, typename Request, typename Response,
                typename RequestBody=typename Request::body_type,
                typename ResponseBody=typename Response::body_type>
        typename ResponseBody::value_type
        handle_status(Request&& old_req, Response&& res, Ofstream&& ofs=nullptr) const {
            switch (res.base().result_int() / 100) {
                case 2: {
                    // TODO: この時，if constexprで，Ofstream

                    close_stream();
                    return res.body();
                }
                case 3: {
                    const std::string new_location = res.base()["Location"].to_string();
                    const auto [new_host, new_target] = parse_url(new_location);
                    cli::debugln("Redirect to ", new_location, '\n');

                    // TODO: header情報が消えている．-> ここまで，最初のheaderを運んでもらう？？？
                    const requests req(new_host);
                    if constexpr (method == http::verb::get) {
                        return req.get(new_target, {}, std::forward<Ofstream>(ofs));
                    }
                    else if (method == http::verb::post) {
                        return req.post<ResponseBody, RequestBody>(new_target, old_req.body());
                    }
                    [[fallthrough]]; // verb error
                }
                default: {
                    // TODO: handle error -> Please open issue
                    close_stream();
                    return res.body();
                }
            }
        }

        void close_stream() const {
            // Gracefully close the stream
            boost::system::error_code ec;
            stream->shutdown(ec);
            if (ec == boost::asio::error::eof) {
                // Rationale: https://stackoverflow.com/q/25587403
                ec.assign(0, ec.category());
            }
        }

        // Prepare ssl connection
        void ssl_prepare() const {
            ssl_set_tlsext();
            lookup();
            ssl_handshake();
        }
        void ssl_set_tlsext() const {
            // Set SNI Hostname (many hosts need this to handshake successfully)
            if(!SSL_set_tlsext_host_name(stream->native_handle(), std::string(host).c_str()))
            {
                boost::system::error_code ec{
                        static_cast<int>(::ERR_get_error()), boost::asio::error::get_ssl_category()
                };
                cli::debugln(ec.message());
                throw boost::system::system_error{ ec };
            }
        }
        void lookup() const {
            // Look up the domain name
            const auto results = resolver->resolve(host, port);
            // Make the connection on the IP address we get from a lookup
            boost::asio::connect(stream->next_layer(), results.begin(), results.end());
        }
        void ssl_handshake() const {
            // Perform the SSL handshake
            stream->handshake(ssl::stream_base::client);
        }
    };


    namespace api {
        std::optional<std::vector<std::string>>
        versions(const std::string& name) {
            using namespace std::string_literals;
            boost::property_tree::ptree pt;
            {
                std::stringstream ss;
                {
                    requests req{};
                    const auto res = req.get(POAC_VERSIONS_API + "/"s + name); // TODO: /演算子が欲しい
                    ss << res.data();
                }
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
            {
                requests req{};
                const auto res = req.get(POAC_DEPS_API + "/"s + name + "/" + version);
                ss << res.data();
            }
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

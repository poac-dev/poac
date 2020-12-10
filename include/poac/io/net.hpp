#ifndef POAC_IO_NET_HPP
#define POAC_IO_NET_HPP

// std
#include <cstdint>
#include <filesystem>
#include <iostream>
#include <fstream>
#include <string>
#include <string_view>
#include <sstream>
#include <numeric>
#include <map>
#include <unordered_map>
#include <memory>
#include <variant>
#include <optional>

// external
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
#include <fmt/core.h>
#include <fmt/ranges.h>
#include <mitama/result/result.hpp>
#include <plog/Log.h>

// internal
#include <poac/config.hpp>
#include <poac/core/except.hpp>
#include <poac/io/path.hpp>
#include <poac/util/meta.hpp>
#include <poac/util/misc.hpp>
#include <poac/util/pretty.hpp>

namespace poac::io::net {
    // Create progress bar, [====>   ]
    std::string to_progress(const int& max_count, int now_count) {
        if (now_count > max_count) {
            now_count = max_count;
        }

        const int bar_size = 50;
        const int percent = (now_count * 100) / max_count;
        const int bar_pos = percent / 2;

        std::string bar = "[";
        if (now_count == max_count) {
            for (int i = 0; i < (bar_size - 1); ++i) {
                bar += "=";
            }
            bar += ">]";
        } else if ((bar_pos - 1) > 0) {
            for (int i = 0; i < (bar_pos - 1); ++i) {
                bar += "=";
            }
            bar += ">";
            for (int i = 0; i < (bar_size - bar_pos); ++i) {
                bar += " ";
            }
            bar += "]";
        } else if (bar_pos == 1) {
            bar += ">";
            for (int i = 0; i < (bar_size - 1); ++i) {
                bar += " ";
            }
            bar += "]";
        } else {
            for (int i = 0; i < bar_size; ++i) {
                bar += " ";
            }
            bar += "]";
        }
        return bar;
    }

    // Print byte progress bar, [====>   ] 10.21B/21.28KB
    void echo_byte_progress(const int& max_count, const int& now_count) {
        const auto [ parsed_max_byte, max_byte_unit ] = util::pretty::to_byte(max_count);
        const auto [ parsed_now_byte, now_byte_unit ] = util::pretty::to_byte(now_count);
        std::cout << to_progress(max_count, now_count) << " ";
        std::cout << std::fixed;
        std::cout << std::setprecision(2) << parsed_now_byte << now_byte_unit << "/";
        std::cout << std::setprecision(2) << parsed_max_byte << max_byte_unit << std::flush;
    }

    namespace http = boost::beast::http;
    namespace ssl = boost::asio::ssl;
    using Headers = std::map<std::variant<http::field, std::string>, std::string>;

    template <typename RequestBody>
    http::request<RequestBody>
    create_request(
            http::verb method,
            std::string_view target,
            std::string_view host=POAC_API_HOST,
            const Headers& headers={}
    ) {
        // Set up an HTTP request message, 10 -> HTTP/1.0, 11 -> HTTP/1.1
        http::request<RequestBody> req{ method, std::string(target), 11 };
        req.set(http::field::host, host);
        req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);
        for (const auto& [field, string_param] : headers) {
            std::visit([&, s=string_param](auto& f) { req.set(f, s); }, field);
        }
        return req;
    }

    std::pair<std::string, std::string>
    parse_url(const std::string& url) {
        // https://api.poac.pm/packages/deps -> api.poac.pm
        const std::string host = util::misc::split(url, "://")[1];
        // https://api.poac.pm/packages/deps -> /packages/deps
        const std::string target(url, url.find(host) + host.size());
        return { host, target };
    }

    class MultiPartForm {
    private:
        const std::string m_crlf = "\r\n";
        std::string m_header;
        std::string m_boundary;
        std::string m_footer;
        const std::string m_content_disposition = "Content-Disposition: form-data; ";
        std::vector<std::string> m_form_param;

    public:
        using file_name_type = std::string;
        using file_path_type = std::filesystem::path;
        using header_type = std::map<http::field, std::string>;
        using self_reference = MultiPartForm&;
        using const_self_reference = const MultiPartForm&;

    private:
        std::vector<std::tuple<file_name_type, file_path_type, header_type>> m_file_param;

    public:
        MultiPartForm()
            : m_boundary(boost::lexical_cast<std::string>(boost::uuids::random_generator{}()))
            , m_footer(fmt::format("{}--{}--{}", m_crlf, m_boundary, m_crlf))
        {}

        std::string
        get_header() const noexcept {
            return m_header;
        }
        std::string
        get_footer() const noexcept {
            return m_footer;
        }

        void set(const file_name_type& name, const std::string& value) {
            using namespace fmt::literals;
            m_form_param.emplace_back(
                fmt::format(
                    "--{boundary}{crlf}{cd}name=\"{name}\"{crlf}{crlf}{value}",
                    "boundary"_a=m_boundary,
                    "crlf"_a=m_crlf,
                    "cd"_a=m_content_disposition,
                    "name"_a=name,
                    "value"_a=value
                )
            );
            generate_header(); // re-generate
        }
        void set(const file_name_type& name, const file_path_type& value, const header_type& h) {
            m_file_param.emplace_back(name, value, h);
            generate_header(); // re-generate
        }
        template <typename Request>
        void set_req(const Request& req) {
            std::stringstream ss;
            ss << req;
            m_form_param.insert(m_form_param.begin(), ss.str());
            generate_header(); // re-generate
        }

        std::string content_type() const {
            return fmt::format("multipart/form-data; boundary={}", m_boundary);
        }
        std::uintmax_t content_length() const {
            return std::accumulate(m_file_param.begin(), m_file_param.end(), m_header.size() + m_footer.size(),
                [](std::uintmax_t acc, const auto& f) {
                    return acc + std::filesystem::file_size(std::get<1>(f));
                }
            );
        }

        struct FileInfo {
            std::string path;
            std::uintmax_t size;
        };
        std::vector<FileInfo>
        get_files() const {
            std::vector<FileInfo> file_info;
            for (const auto& f : m_file_param) {
                const std::filesystem::path file_path = std::get<1>(f);
                file_info.push_back({file_path.string(), std::filesystem::file_size(file_path)});
            }
            return file_info;
        }

        self_reference
        body() noexcept {
            return *this;
        }
        const_self_reference
        body() const noexcept {
            return *this;
        }

        const_self_reference
        cbody() const noexcept {
            return *this;
        }

    private:
        void generate_header() {
            m_header = fmt::format("{}{}", m_crlf, fmt::join(m_form_param, ""));
            for (const auto& [name, filename, header] : m_file_param) {
                std::string h = fmt::format(
                    "--{}{}{}name=\"{}\"; filename=\"{}\"",
                    m_boundary,
                    m_crlf,
                    m_content_disposition,
                    name,
                    filename.filename().string()
                );
                for (const auto& [field, content] : header) {
                    h += fmt::format("{}{}: {}", m_crlf, field, content);
                }
                m_header += m_crlf + h;
            }
            m_header += m_crlf + m_crlf;
        }
    };

    // TODO: ioc, ctx, resolver,...等はget等を呼び出し後，解体し，host等は残すことで，連続で呼び出し可能にする．
    // Only SSL usage
    class requests {
    public:
        explicit requests(std::string_view host)
            : host(host)
            // The io_context is required for all I/O
            , ioc(std::make_unique<boost::asio::io_context>())
            // The SSL context is required, and holds certificates
            , ctx(std::make_unique<ssl::context>(ssl::context::sslv23))
            // These objects perform our I/O
            , resolver(std::make_unique<boost::asio::ip::tcp::resolver>(*ioc))
            , stream(std::make_unique<ssl::stream<boost::asio::ip::tcp::socket>>(*ioc, *ctx))
        {}

        template <http::verb method, typename ResponseBody, typename Request, typename Ofstream>
        typename ResponseBody::value_type
        request(Request&& req, Ofstream&& ofs) const {
            ssl_prepare();
            write_request(req);
            return read_response<method, ResponseBody>(std::forward<Request>(req), std::forward<Ofstream>(ofs));
        }

        template <typename RequestBody=http::empty_body, typename Ofstream=std::nullptr_t,
                typename ResponseBody=std::conditional_t<
                        std::is_same_v<util::meta::remove_cvref_t<Ofstream>, std::ofstream>,
                        http::vector_body<unsigned char>, http::string_body>>
        typename ResponseBody::value_type
        get(std::string_view target, const Headers& headers={}, Ofstream&& ofs=nullptr) const {
            const auto req = create_request<RequestBody>(http::verb::get, target, host, headers);
            PLOG_DEBUG << req;
            return request<http::verb::get, ResponseBody>(std::move(req), std::forward<Ofstream>(ofs));
        }

        template <typename BodyType, typename Ofstream=std::nullptr_t,
                typename RequestBody=std::conditional_t<
                        std::is_same_v<util::meta::remove_cvref_t<BodyType>, MultiPartForm>,
                        http::empty_body, http::string_body>,
                typename ResponseBody=std::conditional_t<
                        std::is_same_v<util::meta::remove_cvref_t<Ofstream>, std::ofstream>,
                        http::vector_body<unsigned char>, http::string_body>>
        typename ResponseBody::value_type
        post(std::string_view target, BodyType&& body, const Headers& headers={}, Ofstream&& ofs=nullptr) const {
            auto req = create_request<RequestBody>(http::verb::post, target, host, headers);
            if constexpr (!std::is_same_v<util::meta::remove_cvref_t<BodyType>, MultiPartForm>) {
                req.set(http::field::content_type, "application/json");
//                body.erase(std::remove(body.begin(), body.end(), '\n'), body.end());
                req.body() = body;
                req.prepare_payload();
                return request<http::verb::post, ResponseBody>(
                        std::forward<decltype(req)>(req), std::forward<Ofstream>(ofs));
            }
            else {
                req.set(http::field::accept, "*/*");
                req.set(http::field::content_type, body.content_type());
                req.set(http::field::content_length, body.content_length());
                body.set_req(req);
                return request<http::verb::post, ResponseBody>(
                        std::forward<BodyType>(body), std::forward<Ofstream>(ofs));
            }
        }

    private:
        std::string_view port = "443";
        std::string_view host;
        std::unique_ptr<boost::asio::io_context> ioc;
        std::unique_ptr<ssl::context> ctx;
        std::unique_ptr<boost::asio::ip::tcp::resolver> resolver;
        std::unique_ptr<ssl::stream<boost::asio::ip::tcp::socket>> stream;

        template <typename Request>
        void write_request(const Request& req) const {
            if constexpr (!std::is_same_v<util::meta::remove_cvref_t<Request>, MultiPartForm>) {
                simple_write(req);
            } else {
                progress_write(req);
            }
        }

        template <typename Request>
        void simple_write(const Request& req) const {
            PLOG_DEBUG << "Write type: string";
            // Send the HTTP request to the remote host
            http::write(*stream, req);
        }

        template <typename Request>
        void progress_write(const Request& req) const {
            PLOG_DEBUG << "Write type: multipart/form-data";

            // Send the HTTP request to the remote host
            stream->write_some(boost::asio::buffer(req.get_header()));
            // Read file and write to stream
            // TODO: 複数のファイル送信を想定していない．
            //  TODO: -> 複数ファイルだと，req.headerをちょびちょびで送る必要がある．
            for (const auto& file : req.get_files()) {
                std::ifstream ifs(file.path, std::ios::in | std::ios::binary);
                constexpr std::size_t read_bites = 512;

                char buf[read_bites];
//                unsigned long cur_file_size = 0;
                while (!ifs.eof()) {
                    ifs.read(buf, read_bites);
                    stream->write_some(boost::asio::buffer(buf, ifs.gcount()));

                    // Print progress bar TODO:
//                    std::cout << '\r' << term::info << "Uploading ";
//                    term::echo_byte_progress(file.size, cur_file_size += read_bites);
//                    std::cout << "  ";
                }
//                std::cout << '\r' << term::clr_line << term::info << "Uploaded." << std::endl;
            }
            // Send footer to stream
            stream->write_some(boost::asio::buffer(req.get_footer()));
            PLOG_DEBUG << "Waiting for server response...";
        }

        template <http::verb method, typename ResponseBody, typename Request, typename Ofstream>
        typename ResponseBody::value_type
        read_response(Request&& old_req, Ofstream&& ofs) const {
            // This buffer is used for reading and must be persisted
            boost::beast::flat_buffer buffer;
            // Declare a container to hold the response
            http::response<ResponseBody> res;
            // Receive the HTTP response
            http::read(*stream, buffer, res);
            // Handle HTTP status code
            return handle_status<method>(
                    std::forward<Request>(old_req),
                    std::move(res),
                    std::forward<Ofstream>(ofs));
        }

        template <http::verb method, typename Request, typename Response, typename Ofstream,
                typename ResponseBody=typename Response::body_type>
        typename ResponseBody::value_type
        handle_status(Request&& old_req, Response&& res, Ofstream&& ofs) const
        {
            close_stream();
            switch (res.base().result_int() / 100) {
                case 2:
                    return parse_response(
                            std::forward<Response>(res),
                            std::forward<Ofstream>(ofs));
                case 3:
                    return redirect<method>(
                            std::forward<Request>(old_req),
                            std::forward<Response>(res),
                            std::forward<Ofstream>(ofs));
                default:
                    if constexpr (!std::is_same_v<util::meta::remove_cvref_t<Ofstream>, std::ofstream>) {
                        throw core::except::error(
                                "io::net received a bad response code: ", res.base().result_int(), "\n",
                                res.body()
                        );
                    } else {
                        throw core::except::error(
                                "io::net received a bad response code: ", res.base().result_int()
                        );
                    }
            }
        }

        template <typename Response, typename Ofstream,
                typename ResponseBody=typename Response::body_type>
        typename ResponseBody::value_type
        parse_response(Response&& res, Ofstream&& ofs) const {
            if constexpr (!std::is_same_v<util::meta::remove_cvref_t<Ofstream>, std::ofstream>) {
                PLOG_DEBUG << "Read type: string";
                return res.body();
            } else {
                PLOG_DEBUG << "Read type: file with progress";
                const typename ResponseBody::value_type response_body = res.body();
                const auto content_length = response_body.size();
                if (content_length < 100'000 /* 100KB */) {
                    for (const auto& r : response_body) {
                        ofs << r;
                    }
                } else {
                    int acc = 0;
                    for (const auto& r : response_body) {
                        ofs << r;
                        if (++acc % 100 == 0) {
                            // To be accurate, not downloading.
                            PLOG_INFO << '\r' << "Downloading ";
                            echo_byte_progress(content_length, acc);
                            PLOG_INFO << "  ";
                        }
                    }
                }
                return {};
            }
        }

        template <http::verb method, typename Request, typename Response, typename Ofstream,
                typename ResponseBody=typename Response::body_type>
        typename ResponseBody::value_type
        redirect(Request&& old_req, Response&& res, Ofstream&& ofs) const {
            const std::string new_location = std::string(res.base()["Location"]);
            const auto [new_host, new_target] = parse_url(new_location);
            PLOG_DEBUG << fmt::format("Redirect to {}\n", new_location);

            // FIXME: header information is gone.
            const requests req(new_host);
            if constexpr (method == http::verb::get) {
                return req.get(new_target, {}, std::forward<Ofstream>(ofs));
            } else if (method == http::verb::post) {
                return req.post(new_target, old_req.body(), {}, std::forward<Ofstream>(ofs));
            } else { // verb error
                return {};
            }
        }

        void close_stream() const {
            // Gracefully close the stream
            boost::system::error_code error;
            stream->shutdown(error);
            if (error == boost::asio::error::eof) {
                // Rationale: https://stackoverflow.com/q/25587403
                error.assign(0, error.category());
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
                boost::system::error_code error{
                        static_cast<int>(::ERR_get_error()), boost::asio::error::get_ssl_category()
                };
                PLOG_DEBUG << error.message();
                throw boost::system::system_error{ error };
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
        [[nodiscard]] mitama::result<boost::property_tree::ptree, std::string>
        search_impl(std::string_view body) noexcept {
            try {
                const requests request{ALGOLIA_SEARCH_INDEX_API_HOST};
                Headers headers;
                headers.emplace("X-Algolia-API-Key", ALGOLIA_SEARCH_ONLY_KEY);
                headers.emplace("X-Algolia-Application-Id", ALGOLIA_APPLICATION_ID);

                const auto response = request.post(ALGOLIA_SEARCH_INDEX_API, body, headers);
                std::stringstream response_body;
                response_body << response.data();

                boost::property_tree::ptree pt;
                boost::property_tree::json_parser::read_json(response_body, pt);
                return mitama::success(pt);
            } catch (const core::except::error& e) {
                return mitama::failure(e.what());
            } catch (...) {
                return mitama::failure("unknown error caused when calling search api");
            }
        }

        [[nodiscard]] mitama::result<boost::property_tree::ptree, std::string>
        search(std::string_view query, const std::uint64_t& count = 0) noexcept {
            boost::property_tree::ptree pt;
            const std::string hits_per_page =
                count != 0 ? fmt::format("&hitsPerPage={}", count) : "";
            const std::string params = fmt::format("query={}{}", query, hits_per_page);
            pt.put("params", params);
            std::stringstream body;
            boost::property_tree::json_parser::write_json(body, pt);
            return search_impl(body.str());
        }

        [[nodiscard]] mitama::result<boost::property_tree::ptree, std::string>
        all_indices() noexcept {
            // ref: https://www.algolia.com/doc/
            //   guides/sending-and-managing-data/manage-your-indices/
            //   how-to/export-an-algolia-index/#exporting-the-index
            // You can use an empty query to indicate
            //   that you want to retrieve all records.
            return search("");
        }

        std::optional<std::vector<std::string>>
        versions(const std::string& name) {
            const requests req{ POAC_API_HOST };
            const auto res = req.get(POAC_VERSIONS_API + ("/" + name));
            std::stringstream ss;
            ss << res.data();
            PLOG_DEBUG << fmt::format("{}: {}", name, ss.str());

            if (ss.str() == "null") {
                return std::nullopt;
            }
            boost::property_tree::ptree pt;
            boost::property_tree::json_parser::read_json(ss, pt);
            return util::meta::to_vector<std::string>(pt);
        }

        [[nodiscard]] auto
        deps(std::string_view name, std::string_view version)
          noexcept
          -> mitama::result<
               std::unordered_map<std::string, std::string>,
               std::string>
        {
            const boost::property_tree::ptree res = MITAMA_TRY(search(name));
            for (const auto& child : res.get_child("hits")) {
                const boost::property_tree::ptree& hits = child.second;

                if (hits.get<std::string>("package.name") != name)
                    continue;
                if (hits.get<std::string>("package.version") != version)
                    continue;

                return mitama::success(
                    util::meta::to_unordered_map<std::string>(
                        hits,"dependencies"
                    )
                );
            }
            return mitama::failure(
                fmt::format("no such package `{}: {}`", name, version)
            );
        }
    }
} // end namespace
#endif // !POAC_IO_NET_HPP

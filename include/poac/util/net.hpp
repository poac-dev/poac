#pragma once

// std
#include <cstdint>
#include <fstream>
#include <iostream>
#include <memory>
#include <numeric>
#include <sstream>
#include <string>
#include <tuple>
#include <type_traits>
#include <utility>
#include <variant>

// external
#include <boost/algorithm/string.hpp>
#define OPENSSL_SUPPRESS_DEPRECATED 1
#include <boost/asio.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/asio/ssl/stream.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <spdlog/spdlog.h> // NOLINT(build/include_order)

// internal
#include "poac/util/format.hpp"
#include "poac/util/log.hpp"
#include "poac/util/meta.hpp"
#include "poac/util/misc.hpp"
#include "poac/util/pretty.hpp"
#include "poac/util/result.hpp"
#include "poac/util/rustify.hpp"
#include "poac/util/verbosity.hpp"

namespace poac::util::net {

// Create progress bar, [====>   ]
Fn to_progress(const i32& max_count, i32 now_count, const i32& bar_size = 50)
    ->String;

// Create byte progress bar, [====>   ] 10.21B/21.28KB
Fn to_byte_progress(const i32& max_count, i32 now_count)->String;

namespace http = boost::beast::http;
using Headers =
    HashMap<std::variant<boost::beast::http::field, String>, String>;

template <typename RequestBody>
Fn create_request(
    http::verb method, const StringRef target, const StringRef host,
    const Headers& headers = {}
)
    ->http::request<RequestBody> {
  // Set up an HTTP request message, 10 -> HTTP/1.0, 11 -> HTTP/1.1
  http::request<RequestBody> req{method, String(target), 11};
  req.set(
      http::field::host, String(host)
  ); // no matching member function for call to 'set'
  req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);
  for (Let & [ field, value ] : headers) {
    std::visit([&, v = value](Let& f) { req.set(f, v); }, field);
  }
  return req;
}

inline Fn parse_url(const String& url)->std::pair<String, String> {
  // https://api.poac.dev/packages/deps -> api.poac.dev
  const String host = util::misc::split(url, "://")[1];
  // https://api.poac.dev/packages/deps -> /packages/deps
  const String target(url, url.find(host) + host.size());
  return {host, target};
}

class MultiPartForm {
public:
  ~MultiPartForm() = default;
  MultiPartForm(const MultiPartForm&) = default;
  Fn operator=(const MultiPartForm&)->MultiPartForm& = default;
  MultiPartForm(MultiPartForm&&) = default;
  Fn operator=(MultiPartForm&&)->MultiPartForm& = default;

  using FileNameType = String;
  using FilePathType = Path;
  using HeaderType = Map<http::field, String>;
  using SelfReference = MultiPartForm&;
  using ConstSelfReference = const MultiPartForm&;

private:
  String m_crlf = "\r\n";
  String m_header;
  String m_boundary;
  String m_footer;
  String m_content_disposition = "Content-Disposition: form-data; ";
  Vec<String> m_form_param;
  Vec<std::tuple<FileNameType, FilePathType, HeaderType>> m_file_param;

public:
  MultiPartForm()
      : m_boundary(boost::uuids::to_string(boost::uuids::random_generator{}())),
        m_footer(format("{}--{}--{}", m_crlf, m_boundary, m_crlf)) {}

  [[nodiscard]] inline Fn get_header() const->String { return m_header; }
  [[nodiscard]] inline Fn get_footer() const->String { return m_footer; }

  inline void set(const FileNameType& name, const String& value) {
    m_form_param.emplace_back(format(
        "--{boundary}{crlf}{cd}name=\"{name}\"{crlf}{crlf}{value}",
        "boundary"_a = m_boundary, "crlf"_a = m_crlf,
        "cd"_a = m_content_disposition, "name"_a = name, "value"_a = value
    ));
    generate_header(); // re-generate
  }
  inline void
  set(const FileNameType& name, const FilePathType& value,
      const HeaderType& h) {
    m_file_param.emplace_back(name, value, h);
    generate_header(); // re-generate
  }
  template <typename Request>
  inline void set_req(const Request& req) {
    const std::ostringstream ss;
    ss << req;
    m_form_param.insert(m_form_param.begin(), ss.str());
    generate_header(); // re-generate
  }

  [[nodiscard]] inline Fn content_type() const->String {
    return format("multipart/form-data; boundary={}", m_boundary);
  }
  [[nodiscard]] inline Fn content_length() const->std::uintmax_t {
    return std::accumulate(
        m_file_param.begin(), m_file_param.end(),
        m_header.size() + m_footer.size(),
        [](std::uintmax_t acc, Let& f) {
          return acc + fs::file_size(std::get<1>(f));
        }
    );
  }

  // NOLINTNEXTLINE(bugprone-exception-escape)
  struct FileInfo {
    String path;
    std::uintmax_t size;
  };
  [[nodiscard]] Fn get_files() const->Vec<FileInfo>;

  inline Fn body() noexcept -> SelfReference { return *this; }
  [[nodiscard]] inline Fn body() const noexcept -> ConstSelfReference {
    return *this;
  }

  [[nodiscard]] inline Fn cbody() const noexcept -> ConstSelfReference {
    return *this;
  }

private:
  void generate_header();
};

// TODO(ken-matsui): ioc, ctx,
// resolver,...等はget等を呼び出し後，解体し，host等は残すことで，連続で呼び出し可能にする．
// Only SSL usage
class Requests {
public:
  Requests() = delete;
  ~Requests() = default;
  Requests(const Requests&) = delete;
  Fn operator=(const Requests&)->Requests& = delete;
  Requests(Requests&&) = default;
  Fn operator=(Requests&&)->Requests& = default;

  explicit Requests(const StringRef host)
      : host(host), ioc(std::make_unique<boost::asio::io_context>()),
        ctx(std::make_unique<boost::asio::ssl::context>(
            boost::asio::ssl::context::sslv23
        )),
        resolver(std::make_unique<boost::asio::ip::tcp::resolver>(*ioc)),
        stream(std::make_unique<
               boost::asio::ssl::stream<boost::asio::ip::tcp::socket>>(
            *ioc, *ctx
        )) {}

  template <
      http::verb method, typename ResponseBody, typename Request,
      typename Ofstream>
  [[nodiscard]] Fn request(Request&& req, Ofstream&& ofs) const
      ->Result<typename ResponseBody::value_type, String> {
    ssl_prepare();
    write_request(req);
    return read_response<method, ResponseBody>(
        std::forward<Request>(req), std::forward<Ofstream>(ofs)
    );
  }

  template <
      typename RequestBody = http::empty_body,
      typename Ofstream = std::nullptr_t,
      typename ResponseBody = std::conditional_t<
          std::is_same_v<std::remove_cvref_t<Ofstream>, std::ofstream>,
          http::vector_body<unsigned char>, http::string_body>>
  [[nodiscard]] Fn
  get(const StringRef target, const Headers& headers = {},
      Ofstream&& ofs = nullptr
  ) const->Result<typename ResponseBody::value_type, String> {
    Let req =
        create_request<RequestBody>(http::verb::get, target, host, headers);

    if (verbosity::is_verbose()) {
      std::stringstream ss; // NOLINT(misc-const-correctness)
      ss << req;
      log::debug("{}", ss.str());
    }

    return request<http::verb::get, ResponseBody>(
        std::move(req), std::forward<Ofstream>(ofs)
    );
  }

  template <
      typename BodyType, typename Ofstream = std::nullptr_t,
      typename RequestBody = std::conditional_t<
          std::is_same_v<std::remove_cvref_t<BodyType>, MultiPartForm>,
          http::empty_body, http::string_body>,
      typename ResponseBody = std::conditional_t<
          std::is_same_v<std::remove_cvref_t<Ofstream>, std::ofstream>,
          http::vector_body<unsigned char>, http::string_body>>
  [[nodiscard]] Fn post(
      const StringRef target, BodyType&& body, const Headers& headers = {},
      Ofstream&& ofs = nullptr
  ) const->Result<typename ResponseBody::value_type, String> {
    auto req =
        create_request<RequestBody>(http::verb::post, target, host, headers);
    if constexpr (!std::is_same_v<
                      std::remove_cvref_t<BodyType>, MultiPartForm>) {
      req.set(http::field::content_type, "application/json");
      req.body() = body;
      req.prepare_payload();
      return request<http::verb::post, ResponseBody>(
          std::forward<decltype(req)>(req), std::forward<Ofstream>(ofs)
      );
    } else {
      req.set(http::field::accept, "*/*");
      req.set(http::field::content_type, body.content_type());
      req.set(http::field::content_length, body.content_length());
      body.set_req(req);
      return request<http::verb::post, ResponseBody>(
          std::forward<BodyType>(body), std::forward<Ofstream>(ofs)
      );
    }
  }

private:
  String port = "443";
  String host;
  // The io_context is required for all I/O
  std::unique_ptr<boost::asio::io_context> ioc;
  // The SSL context is required, and holds certificates
  std::unique_ptr<boost::asio::ssl::context> ctx;
  // These objects perform our I/O
  std::unique_ptr<boost::asio::ip::tcp::resolver> resolver;
  std::unique_ptr<boost::asio::ssl::stream<boost::asio::ip::tcp::socket>>
      stream;

  template <
      typename Request, std::enable_if_t<
                            std::negation_v<std::is_same<
                                std::remove_cvref_t<Request>, MultiPartForm>>,
                            std::nullptr_t> = nullptr>
  void write_request(const Request& req) const {
    log::debug("[util::net::requests] write type: string");
    // Send the HTTP request to the remote host
    http::write(*stream, req);
  }

  template <
      typename Request,
      std::enable_if_t<
          std::is_same_v<std::remove_cvref_t<Request>, MultiPartForm>,
          std::nullptr_t> = nullptr>
  void write_request(const Request& req) const {
    log::debug("[util::net::requests] write type: multipart/form-data");

    // Send the HTTP request to the remote host
    stream->write_some(boost::asio::buffer(req.get_header()));
    // Read file and write to stream
    // TODO(ken-matsui): 複数のファイル送信を想定していない．
    //  TODO(ken-matsui): ->
    //  複数ファイルだと，req.headerをちょびちょびで送る必要がある．
    for (Let& file : req.get_files()) {
      std::ifstream ifs(file.path, std::ios::in | std::ios::binary);
      constexpr usize K_READ_BITES = 512;

      // NOLINTNEXTLINE(modernize-avoid-c-arrays)
      char buf[K_READ_BITES];
      //                unsigned long cur_file_size = 0;
      while (!ifs.eof()) {
        ifs.read(buf, K_READ_BITES);
        stream->write_some(boost::asio::buffer(buf, ifs.gcount()));

        // Print progress bar TODO:
        //                    std::cout << '\r' << term::info << "Uploading ";
        //                    term::echo_byte_progress(file.size,
        //                    cur_file_size
        //                    += read_bites); std::cout << "  ";
      }
      //                std::cout << '\r' << term::clr_line << term::info <<
      //                "Uploaded." << std::endl;
    }
    // Send footer to stream
    stream->write_some(boost::asio::buffer(req.get_footer()));
    log::debug("[util::net::requests] waiting for server response...");
  }

  template <
      http::verb method, typename ResponseBody, typename Request,
      typename Ofstream>
  [[nodiscard]] Fn read_response(Request&& old_req, Ofstream&& ofs) const
      ->Result<typename ResponseBody::value_type, String> {
    // This buffer is used for reading and must be persisted
    boost::beast::flat_buffer buffer;
    // Declare a container to hold the response
    http::response<ResponseBody> res;
    // Receive the HTTP response
    http::read(*stream, buffer, res);
    // Handle HTTP status code
    return handle_status<method>(
        std::forward<Request>(old_req), std::move(res),
        std::forward<Ofstream>(ofs)
    );
  }

  template <
      http::verb method, typename Request, typename Response, typename Ofstream,
      typename ResponseBody = typename Response::body_type>
  [[nodiscard]] Fn handle_status(
      Request&& old_req, Response&& res, Ofstream&& ofs
  ) const->Result<typename ResponseBody::value_type, String> {
    close_stream();
    // NOLINTNEXTLINE(clang-analyzer-cplusplus.Move)
    switch (res.base().result_int() / 100) {
      case 2:
        return Ok(parse_response(
            std::forward<Response>(res), std::forward<Ofstream>(ofs)
        ));
      case 3:
        return redirect<method>(
            std::forward<Request>(old_req), std::forward<Response>(res),
            std::forward<Ofstream>(ofs)
        );
      default:
        if constexpr (!std::is_same_v<
                          std::remove_cvref_t<Ofstream>, std::ofstream>) {
          return Err(format(
              "util::net received a bad response code: {}\n{}",
              res.base().result_int(), res.body()
          ));
        } else {
          throw Err(format(
              "util::net received a bad response code: {}",
              res.base().result_int()
          ));
        }
    }
  }

  template <
      typename Response, typename Ofstream,
      typename ResponseBody = typename Response::body_type>
  Fn parse_response(Response&& res, Ofstream&& ofs) const->
      typename ResponseBody::value_type {
    if constexpr (!std::is_same_v<
                      std::remove_cvref_t<Ofstream>, std::ofstream>) {
      log::debug("[util::net::requests] read type: string");
      return res.body();
    } else {
      log::debug("[util::net::requests] read type: file with progress");
      const typename ResponseBody::value_type response_body = res.body();
      Let content_length = response_body.size();
      if (content_length < 100'000 /* 100KB */) {
        for (Let& r : response_body) {
          ofs << r;
        }
      } else {
        i32 acc = 0;
        for (Let& r : response_body) {
          ofs << r;
          if (++acc % 100 == 0) {
            // To be accurate, not downloading.
            if (verbosity::is_verbose()) {
              std::cout << '\r' << "Downloading "
                        << to_byte_progress(content_length, acc) << "  ";
            }
          }
        }
      }
      return {};
    }
  }

  template <
      http::verb method, typename Request, typename Response, typename Ofstream,
      typename ResponseBody = typename Response::body_type>
  [[nodiscard]] Fn redirect(Request&& old_req, Response&& res, Ofstream&& ofs)
      const->Result<typename ResponseBody::value_type, String> {
    const String new_location(res.base()["Location"]);
    Let[new_host, new_target] = parse_url(new_location);
    spdlog::debug("Redirect to {}\n", new_location);

    // FIXME: header information is gone.
    const Requests req(new_host);
    if constexpr (method == http::verb::get) {
      return req.get(new_target, {}, std::forward<Ofstream>(ofs));
    } else if (method == http::verb::post) {
      return req.post(
          new_target, old_req.body(), {}, std::forward<Ofstream>(ofs)
      );
    } else { // verb error
      return Err("[util::net::requests] unknown method used");
    }
  }

  void close_stream() const;

  // Prepare ssl connection
  inline void ssl_prepare() const {
    ssl_set_tlsext();
    lookup();
    ssl_handshake();
  }

  void ssl_set_tlsext() const;

  inline void lookup() const {
    // Look up the domain name
    Let results = resolver->resolve(host, port);
    // Make the connection on the IP address we get from a lookup
    boost::asio::connect(stream->next_layer(), results.begin(), results.end());
  }

  inline void ssl_handshake() const {
    // Perform the SSL handshake
    stream->handshake(boost::asio::ssl::stream_base::client);
  }
};

} // namespace poac::util::net

namespace poac::util::net::api {

[[nodiscard]] Fn
call(StringRef path, const Option<String>& body = None) noexcept
    -> Result<boost::property_tree::ptree, String>;

[[nodiscard]] Fn search(StringRef query, const u64& count = 0)
    ->Result<boost::property_tree::ptree, String>;

[[nodiscard]] Fn deps(StringRef name, StringRef version)
    ->Result<HashMap<String, String>, String>;

[[nodiscard]] Fn versions(StringRef name)->Result<Vec<String>, String>;

[[nodiscard]] Fn repoinfo(StringRef name, StringRef version)
    ->Result<std::pair<String, String>, String>;

[[nodiscard]] Fn login(StringRef api_token)->Result<bool, String>;

} // namespace poac::util::net::api

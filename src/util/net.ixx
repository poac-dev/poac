module;

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
#include "result-macros.hpp"

export module poac.util.net;

// internal
import poac.core.resolver.types;
import poac.util.format;
import poac.util.log;
import poac.util.meta;
import poac.util.misc;
import poac.util.pretty;
import poac.util.result;
import poac.util.rustify;
import poac.util.verbosity;

namespace poac::util::net {

using namespace fmt::literals;

} // namespace poac::util::net

export namespace poac::util::net {

// Create progress bar, [====>   ]
auto to_progress(const i32& max_count, i32 now_count, const i32& bar_size = 50)
    -> String {
  if (now_count > max_count) {
    now_count = max_count;
  }
  const i32 percent = (now_count * 100) / max_count;
  const i32 bar_pos = percent / 2;

  if (now_count == max_count) {
    return format("[{:=>{}}", ">]", bar_size + 1);
  } else if ((bar_pos - 1) > 0) {
    return format("[{:=>{}}{:>{}}", ">", bar_pos, "]", bar_size - bar_pos + 1);
  } else if (bar_pos == 1) {
    return format("[>{:>{}}", "]", bar_size);
  } else {
    return format("[{:>{}}", "]", bar_size + 1);
  }
}

// Create byte progress bar, [====>   ] 10.21B/21.28KB
auto to_byte_progress(const i32& max_count, i32 now_count) -> String {
  if (now_count > max_count) {
    now_count = max_count;
  }
  return format(
      "{} {}/{}", to_progress(max_count, now_count),
      util::pretty::to_byte(now_count), util::pretty::to_byte(max_count)
  );
}

namespace http = boost::beast::http;
using Headers =
    HashMap<std::variant<boost::beast::http::field, String>, String>;

template <typename RequestBody>
auto create_request(
    http::verb method, const StringRef target, const StringRef host,
    const Headers& headers = {}
) -> http::request<RequestBody> {
  // Set up an HTTP request message, 10 -> HTTP/1.0, 11 -> HTTP/1.1
  http::request<RequestBody> req{method, String(target), 11};
  req.set(
      http::field::host, String(host)
  ); // no matching member function for call to 'set'
  req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);
  for (const auto& [field, value] : headers) {
    std::visit([&, v = value](const auto& f) { req.set(f, v); }, field);
  }
  return req;
}

inline auto parse_url(const String& url) -> std::pair<String, String> {
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
  auto operator=(const MultiPartForm&) -> MultiPartForm& = default;
  MultiPartForm(MultiPartForm&&) = default;
  auto operator=(MultiPartForm&&) -> MultiPartForm& = default;

  using FileNameType = String;
  using FilePathType = Path;
  using HeaderType = Map<http::field, String>;
  using SelfReference = MultiPartForm&;
  using ConstSelfReference = const MultiPartForm&;

private:
  // Note: false positive
  // NOLINTNEXTLINE(misc-non-private-member-variables-in-classes)
  String _crlf = "\r\n";
  String _header;
  String _boundary;
  String _footer;
  // Note: false positive
  // NOLINTNEXTLINE(misc-non-private-member-variables-in-classes)
  String _content_disposition = "Content-Disposition: form-data; ";
  Vec<String> _form_param;
  Vec<std::tuple<FileNameType, FilePathType, HeaderType>> _file_param;

public:
  MultiPartForm()
      : _boundary(boost::uuids::to_string(boost::uuids::random_generator{}())),
        _footer(format("{}--{}--{}", _crlf, _boundary, _crlf)) {}

  [[nodiscard]] inline auto get_header() const -> String { return _header; }
  [[nodiscard]] inline auto get_footer() const -> String { return _footer; }

  inline void set(const FileNameType& name, const String& value) {
    _form_param.emplace_back(format(
        "--{boundary}{crlf}{cd}name=\"{name}\"{crlf}{crlf}{value}",
        "boundary"_a = _boundary, "crlf"_a = _crlf,
        "cd"_a = _content_disposition, "name"_a = name, "value"_a = value
    ));
    generate_header(); // re-generate
  }
  inline void
  set(const FileNameType& name, const FilePathType& value,
      const HeaderType& h) {
    _file_param.emplace_back(name, value, h);
    generate_header(); // re-generate
  }
  template <typename Request>
  inline void set_req(const Request& req) {
    const std::ostringstream ss;
    ss << req;
    _form_param.insert(_form_param.begin(), ss.str());
    generate_header(); // re-generate
  }

  [[nodiscard]] inline auto content_type() const -> String {
    return format("multipart/form-data; boundary={}", _boundary);
  }
  [[nodiscard]] inline auto content_length() const -> std::uintmax_t {
    return std::accumulate(
        _file_param.begin(), _file_param.end(), _header.size() + _footer.size(),
        [](std::uintmax_t acc, const auto& f) {
          return acc + fs::file_size(std::get<1>(f));
        }
    );
  }

  // NOLINTNEXTLINE(bugprone-exception-escape)
  struct FileInfo {
    String path;
    std::uintmax_t size;
  };
  [[nodiscard]] auto get_files() const -> Vec<FileInfo> {
    Vec<FileInfo> file_info;
    for (const auto& f : _file_param) {
      const Path file_path = std::get<1>(f);
      file_info.push_back({file_path.string(), fs::file_size(file_path)});
    }
    return file_info;
  }

  inline auto body() noexcept -> SelfReference { return *this; }
  [[nodiscard]] inline auto body() const noexcept -> ConstSelfReference {
    return *this;
  }

  [[nodiscard]] inline auto cbody() const noexcept -> ConstSelfReference {
    return *this;
  }

private:
  void generate_header() {
    _header = format("{}{}", _crlf, fmt::join(_form_param, ""));
    for (const auto& [name, filename, header] : _file_param) {
      String h = format(
          R"(--{}{}{}name="{}"; filename="{}")", _boundary, _crlf,
          _content_disposition, name, filename.filename().string()
      );
      for (const auto& [field, content] : header) {
        // NOLINTNEXTLINE(google-readability-casting)
        h += format("{}{}: {}", _crlf, String(to_string(field)), content);
      }
      _header += _crlf + h;
    }
    _header += _crlf + _crlf;
  }
};

// TODO(ken-matsui): ioc, ctx,
// resolver,...等はget等を呼び出し後，解体し，host等は残すことで，連続で呼び出し可能にする．
// Only SSL usage
class Requests {
public:
  Requests() = delete;
  ~Requests() = default;
  Requests(const Requests&) = delete;
  auto operator=(const Requests&) -> Requests& = delete;
  Requests(Requests&&) = default;
  auto operator=(Requests&&) -> Requests& = default;

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
  [[nodiscard]] auto request(Request&& req, Ofstream&& ofs) const
      -> Result<typename ResponseBody::value_type, String> {
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
  [[nodiscard]] auto
  get(const StringRef target, const Headers& headers = {},
      Ofstream&& ofs = nullptr) const
      -> Result<typename ResponseBody::value_type, String> {
    const auto req =
        create_request<RequestBody>(http::verb::get, target, host, headers);

    //    if (verbosity::is_verbose()) {
    //      std::stringstream ss; // NOLINT(misc-const-correctness)
    //      ss << req; // FIXME(ken-matsui): weird error
    //      log::debug("{}", ss.str());
    //    }

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
  [[nodiscard]] auto post(
      const StringRef target, BodyType&& body, const Headers& headers = {},
      Ofstream&& ofs = nullptr
  ) const -> Result<typename ResponseBody::value_type, String> {
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
    for (const auto& file : req.get_files()) {
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
  [[nodiscard]] auto read_response(Request&& old_req, Ofstream&& ofs) const
      -> Result<typename ResponseBody::value_type, String> {
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
  [[nodiscard]] auto
  handle_status(Request&& old_req, Response&& res, Ofstream&& ofs) const
      -> Result<typename ResponseBody::value_type, String> {
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
  auto parse_response(Response&& res, Ofstream&& ofs) const ->
      typename ResponseBody::value_type {
    if constexpr (!std::is_same_v<
                      std::remove_cvref_t<Ofstream>, std::ofstream>) {
      log::debug("[util::net::requests] read type: string");
      return res.body();
    } else {
      log::debug("[util::net::requests] read type: file with progress");
      const typename ResponseBody::value_type response_body = res.body();
      const auto content_length = response_body.size();
      if (content_length < 100'000 /* 100KB */) {
        for (const auto& r : response_body) {
          ofs << r;
        }
      } else {
        i32 acc = 0;
        for (const auto& r : response_body) {
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
  [[nodiscard]] auto
  redirect(Request&& old_req, Response&& res, Ofstream&& ofs) const
      -> Result<typename ResponseBody::value_type, String> {
    const String new_location(res.base()["Location"]);
    const auto [new_host, new_target] = parse_url(new_location);
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
  inline void ssl_prepare() const {
    ssl_set_tlsext();
    lookup();
    ssl_handshake();
  }

  void ssl_set_tlsext() const {
    // Set SNI Hostname (many hosts need this to handshake successfully)
    if (!SSL_set_tlsext_host_name(
            stream->native_handle(), String(host).c_str()
        )) {
      const boost::system::error_code error{
          static_cast<i32>(::ERR_get_error()),
          boost::asio::error::get_ssl_category()};
      log::debug(error.message());
      throw boost::system::system_error{error};
    }
  }

  // Note: false positive
  // NOLINTNEXTLINE(readability-convert-member-functions-to-static)
  inline void lookup() const {
    // Look up the domain name
    const auto results = resolver->resolve(host, port);
    // Make the connection on the IP address we get from a lookup
    boost::asio::connect(stream->next_layer(), results.begin(), results.end());
  }

  inline void ssl_handshake() const {
    // Perform the SSL handshake
    stream->handshake(boost::asio::ssl::stream_base::client);
  }
};

} // namespace poac::util::net

export namespace poac::util::net::api {

[[nodiscard]] auto
call(StringRef path, const Option<String>& body = None) noexcept
    -> Result<boost::property_tree::ptree, String> {
  try {
    const Requests request{"api.poac.dev"};
    const String target = format("/v1{}", path);
    const auto response =
        Try(body.has_value() ? request.post(target, body.value())
                             : request.get(target));
    std::stringstream response_body;
    response_body << response.data();

    boost::property_tree::ptree pt;
    boost::property_tree::json_parser::read_json(response_body, pt);
    return Ok(pt);
  } catch (const std::exception& e) {
    return Err(e.what());
  } catch (...) {
    return Err(format("unknown error caused when calling API for {}", path));
  }
}

[[nodiscard]] auto search(StringRef query, const u64& count = 0)
    -> Result<boost::property_tree::ptree, String> {
  const String path =
      format("/packages/search?query={}&perPage={}", query, count);
  return call(path);
}

[[nodiscard]] auto deps(
    StringRef name, StringRef version
) -> Result<HashMap<String, core::resolver::resolve::DependencyInfo>, String> {
  const String path = format("/packages/{}/{}/deps", name, version);
  const boost::property_tree::ptree res = Try(call(path));
  if (verbosity::is_verbose()) {
    boost::property_tree::write_json(std::cout, res);
  }
  const HashMap<String, String> temp =
      util::meta::to_hash_map<String>(res, "data");
  HashMap<String, poac::core::resolver::resolve::DependencyInfo> ret;
  for (const auto& [name, data] : temp) {
    ret.emplace(name, poac::core::resolver::resolve::DependencyInfo{data});
  }
  return Ok(std::move(ret));
}

[[nodiscard]] auto versions(StringRef name) -> Result<Vec<String>, String> {
  const String path = format("/packages/{}/versions", name);
  const boost::property_tree::ptree res = Try(call(path));
  if (verbosity::is_verbose()) {
    boost::property_tree::write_json(std::cout, res);
  }
  const auto results = util::meta::to_vec<String>(res, "data");
  log::debug(
      "[util::net::api::versions] versions of {} are [{}]", name,
      fmt::join(results, ", ")
  );
  return Ok(results);
}

[[nodiscard]] auto repoinfo(StringRef name, StringRef version)
    -> Result<std::pair<String, String>, String> {
  const String path = format("/packages/{}/{}/repoinfo", name, version);
  const boost::property_tree::ptree res = Try(call(path));
  if (verbosity::is_verbose()) {
    boost::property_tree::write_json(std::cout, res);
  }
  return Ok(std::make_pair(
      res.get<String>("data.repository"), res.get<String>("data.sha256sum")
  ));
}

[[nodiscard]] auto login(StringRef api_token) -> Result<bool, String> {
  boost::property_tree::ptree pt;
  pt.put("api_token", api_token);

  std::ostringstream body;
  boost::property_tree::write_json(body, pt);
  const boost::property_tree::ptree res = Try(call("/login", body.str()));
  if (verbosity::is_verbose()) {
    boost::property_tree::write_json(std::cout, res);
  }
  return Ok(res.get<bool>("data"));
}

} // namespace poac::util::net::api

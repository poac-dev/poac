#ifndef POAC_UTIL_NET_HPP_
#define POAC_UTIL_NET_HPP_

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
#include <poac/config.hpp>
#include <poac/poac.hpp>
#include <poac/util/meta.hpp>
#include <poac/util/misc.hpp>
#include <poac/util/pretty.hpp>
#include <poac/util/verbosity.hpp>

namespace poac::util::net {

// Create progress bar, [====>   ]
String
to_progress(const i32& max_count, i32 now_count, const i32& bar_size = 50) {
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
String
to_byte_progress(const i32& max_count, i32 now_count) {
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
http::request<RequestBody>
create_request(
    http::verb method, const StringRef target, const StringRef host,
    const Headers& headers = {}
) {
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

std::pair<String, String>
parse_url(const String& url) {
  // https://api.poac.pm/packages/deps -> api.poac.pm
  const String host = util::misc::split(url, "://")[1];
  // https://api.poac.pm/packages/deps -> /packages/deps
  const String target(url, url.find(host) + host.size());
  return {host, target};
}

class MultiPartForm {
public:
  ~MultiPartForm() = default;
  MultiPartForm(const MultiPartForm&) = default;
  MultiPartForm&
  operator=(const MultiPartForm&) = default;
  MultiPartForm(MultiPartForm&&) = default;
  MultiPartForm&
  operator=(MultiPartForm&&) = default;

public:
  using file_name_type = String;
  using file_path_type = fs::path;
  using header_type = Map<http::field, String>;
  using self_reference = MultiPartForm&;
  using const_self_reference = const MultiPartForm&;

private:
  String m_crlf = "\r\n";
  String m_header;
  String m_boundary;
  String m_footer;
  String m_content_disposition = "Content-Disposition: form-data; ";
  Vec<String> m_form_param;
  Vec<std::tuple<file_name_type, file_path_type, header_type>> m_file_param;

public:
  MultiPartForm()
      : m_boundary(boost::uuids::to_string(boost::uuids::random_generator{}())),
        m_footer(format("{}--{}--{}", m_crlf, m_boundary, m_crlf)) {}

  String
  get_header() const noexcept {
    return m_header;
  }
  String
  get_footer() const noexcept {
    return m_footer;
  }

  void
  set(const file_name_type& name, const String& value) {
    m_form_param.emplace_back(format(
        "--{boundary}{crlf}{cd}name=\"{name}\"{crlf}{crlf}{value}",
        "boundary"_a = m_boundary, "crlf"_a = m_crlf,
        "cd"_a = m_content_disposition, "name"_a = name, "value"_a = value
    ));
    generate_header(); // re-generate
  }
  void
  set(const file_name_type& name, const file_path_type& value,
      const header_type& h) {
    m_file_param.emplace_back(name, value, h);
    generate_header(); // re-generate
  }
  template <typename Request>
  void
  set_req(const Request& req) {
    std::ostringstream ss;
    ss << req;
    m_form_param.insert(m_form_param.begin(), ss.str());
    generate_header(); // re-generate
  }

  String
  content_type() const {
    return format("multipart/form-data; boundary={}", m_boundary);
  }
  std::uintmax_t
  content_length() const {
    return std::accumulate(
        m_file_param.begin(), m_file_param.end(),
        m_header.size() + m_footer.size(),
        [](std::uintmax_t acc, const auto& f) {
          return acc + fs::file_size(std::get<1>(f));
        }
    );
  }

  struct FileInfo {
    String path;
    std::uintmax_t size;
  };
  Vec<FileInfo>
  get_files() const {
    Vec<FileInfo> file_info;
    for (const auto& f : m_file_param) {
      const fs::path file_path = std::get<1>(f);
      file_info.push_back({file_path.string(), fs::file_size(file_path)});
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
  void
  generate_header() {
    m_header = format("{}{}", m_crlf, fmt::join(m_form_param, ""));
    for (const auto& [name, filename, header] : m_file_param) {
      String h = format(
          "--{}{}{}name=\"{}\"; filename=\"{}\"", m_boundary, m_crlf,
          m_content_disposition, name, filename.filename().string()
      );
      for (const auto& [field, content] : header) {
        h += format("{}{}: {}", m_crlf, String(to_string(field)), content);
      }
      m_header += m_crlf + h;
    }
    m_header += m_crlf + m_crlf;
  }
};

// TODO(ken-matsui): ioc, ctx,
// resolver,...等はget等を呼び出し後，解体し，host等は残すことで，連続で呼び出し可能にする．
// Only SSL usage
class Requests {
public:
  // clang-format off
    Requests() = delete;
    ~Requests() = default;
    Requests(const Requests&) = delete;
    Requests& operator=(const Requests&) = delete;
    Requests(Requests&&) = default;
    Requests& operator=(Requests&&) = default;
  // clang-format on

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
  [[nodiscard]] Result<typename ResponseBody::value_type, String>
  request(Request&& req, Ofstream&& ofs) const {
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
  [[nodiscard]] Result<typename ResponseBody::value_type, String>
  get(const StringRef target, const Headers& headers = {},
      Ofstream&& ofs = nullptr) const {
    const auto req =
        create_request<RequestBody>(http::verb::get, target, host, headers);

    if (verbosity::is_verbose()) {
      std::stringstream ss;
      ss << req;
      spdlog::debug("{}", ss.str());
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
  [[nodiscard]] Result<typename ResponseBody::value_type, String>
  post(
      const StringRef target, BodyType&& body, const Headers& headers = {},
      Ofstream&& ofs = nullptr
  ) const {
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
  void
  write_request(const Request& req) const {
    spdlog::debug("[util::net::requests] write type: string");
    // Send the HTTP request to the remote host
    http::write(*stream, req);
  }

  template <
      typename Request,
      std::enable_if_t<
          std::is_same_v<std::remove_cvref_t<Request>, MultiPartForm>,
          std::nullptr_t> = nullptr>
  void
  write_request(const Request& req) const {
    spdlog::debug("[util::net::requests] write type: multipart/form-data");

    // Send the HTTP request to the remote host
    stream->write_some(boost::asio::buffer(req.get_header()));
    // Read file and write to stream
    // TODO(ken-matsui): 複数のファイル送信を想定していない．
    //  TODO(ken-matsui): ->
    //  複数ファイルだと，req.headerをちょびちょびで送る必要がある．
    for (const auto& file : req.get_files()) {
      std::ifstream ifs(file.path, std::ios::in | std::ios::binary);
      constexpr usize kReadBites = 512;

      char buf[kReadBites];
      //                unsigned long cur_file_size = 0;
      while (!ifs.eof()) {
        ifs.read(buf, kReadBites);
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
    spdlog::debug("[util::net::requests] waiting for server response...");
  }

  template <
      http::verb method, typename ResponseBody, typename Request,
      typename Ofstream>
  [[nodiscard]] Result<typename ResponseBody::value_type, String>
  read_response(Request&& old_req, Ofstream&& ofs) const {
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
  [[nodiscard]] Result<typename ResponseBody::value_type, String>
  handle_status(Request&& old_req, Response&& res, Ofstream&& ofs) const {
    close_stream();
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
  typename ResponseBody::value_type
  parse_response(Response&& res, Ofstream&& ofs) const {
    if constexpr (!std::is_same_v<
                      std::remove_cvref_t<Ofstream>, std::ofstream>) {
      spdlog::debug("[util::net::requests] read type: string");
      return res.body();
    } else {
      spdlog::debug("[util::net::requests] read type: file with progress");
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
  [[nodiscard]] Result<typename ResponseBody::value_type, String>
  redirect(Request&& old_req, Response&& res, Ofstream&& ofs) const {
    const String new_location(res.base()["Location"]);
    const auto [new_host, new_target] = parse_url(new_location);
    spdlog::debug(format("Redirect to {}\n", new_location));

    // FIXME: header information is gone.
    const Requests req(new_host);
    if constexpr (method == http::verb::get) {
      return req.get(new_target, {}, std::forward<Ofstream>(ofs));
    } else if (method == http::verb::post) {
      return req.post(
          new_target, old_req.body(), {}, std::forward<Ofstream>(ofs)
      );
    } else { // verb error
      return Err("[util::net::requests] unknown verb used");
    }
  }

  void
  close_stream() const {
    // Gracefully close the stream
    boost::system::error_code ec;
    stream->shutdown(ec);
    if (ec == boost::asio::error::eof) {
      // Rationale: https://stackoverflow.com/q/25587403
      ec.assign(0, ec.category());
    }
  }

  // Prepare ssl connection
  void
  ssl_prepare() const {
    ssl_set_tlsext();
    lookup();
    ssl_handshake();
  }

  void
  ssl_set_tlsext() const {
    // Set SNI Hostname (many hosts need this to handshake successfully)
    if (!SSL_set_tlsext_host_name(
            stream->native_handle(), String(host).c_str()
        )) {
      boost::system::error_code error{
          static_cast<i32>(::ERR_get_error()),
          boost::asio::error::get_ssl_category()};
      spdlog::debug(error.message());
      throw boost::system::system_error{error};
    }
  }

  void
  lookup() const {
    // Look up the domain name
    const auto results = resolver->resolve(host, port);
    // Make the connection on the IP address we get from a lookup
    boost::asio::connect(stream->next_layer(), results.begin(), results.end());
  }

  void
  ssl_handshake() const {
    // Perform the SSL handshake
    stream->handshake(boost::asio::ssl::stream_base::client);
  }
};
} // namespace poac::util::net

namespace poac::util::net::api {
[[nodiscard]] Result<boost::property_tree::ptree, String>
call(StringRef path, StringRef body) noexcept {
  try {
    const Requests request{
        format("{}.functions.supabase.co", SUPABASE_PROJECT_REF)};
    Headers headers;
    headers.emplace(
        boost::beast::http::field::authorization,
        format("Bearer {}", SUPABASE_ANON_KEY)
    );

    const auto response = Try(request.post(path, body, headers));
    std::stringstream response_body;
    response_body << response.data();

    boost::property_tree::ptree pt;
    boost::property_tree::json_parser::read_json(response_body, pt);
    return Ok(pt);
  } catch (const std::exception& e) {
    return Err(e.what());
  } catch (...) {
    return Err("unknown error caused when calling search api");
  }
}

[[nodiscard]] Result<boost::property_tree::ptree, String>
search(StringRef query, const u64& count = 0) noexcept {
  boost::property_tree::ptree pt;
  pt.put("query", query);
  pt.put("perPage", count);

  std::ostringstream body;
  boost::property_tree::json_parser::write_json(body, pt);
  return call("/search", body.str());
}

[[nodiscard]] auto
deps(StringRef name, StringRef version) noexcept
    -> Result<HashMap<String, String>, String> {
  boost::property_tree::ptree pt;
  pt.put("name", name);
  pt.put("version", version);

  std::ostringstream body;
  boost::property_tree::json_parser::write_json(body, pt);
  const boost::property_tree::ptree res = Try(call("/deps", body.str()));
  if (verbosity::is_verbose()) {
    boost::property_tree::json_parser::write_json(std::cout, res);
  }
  return Ok(util::meta::to_hash_map<String>(res, "data.dependencies"));
}

[[nodiscard]] Result<Vec<String>, String>
versions(StringRef name) {
  boost::property_tree::ptree pt;
  pt.put("name", name);

  std::ostringstream body;
  boost::property_tree::json_parser::write_json(body, pt);
  const boost::property_tree::ptree res = Try(call("/versions", body.str()));
  if (verbosity::is_verbose()) {
    boost::property_tree::json_parser::write_json(std::cout, res);
  }
  const auto results = util::meta::to_vec<String>(res, "data");
  spdlog::debug(
      "[util::net::api::versions] versions of {} are [{}]", name,
      fmt::join(results, ", ")
  );
  return Ok(results);
}

[[nodiscard]] Result<std::pair<String, String>, String>
repoinfo(StringRef name, StringRef version) {
  boost::property_tree::ptree pt;
  pt.put("name", name);
  pt.put("version", version);

  std::ostringstream body;
  boost::property_tree::json_parser::write_json(body, pt);
  const boost::property_tree::ptree res = Try(call("/repoinfo", body.str()));
  if (verbosity::is_verbose()) {
    boost::property_tree::json_parser::write_json(std::cout, res);
  }
  return Ok(std::make_pair(
      res.get<String>("data.repository"), res.get<String>("data.sha256sum")
  ));
}

[[nodiscard]] Result<bool, String>
login(StringRef api_token) {
  boost::property_tree::ptree pt;
  pt.put("api_token", api_token);

  std::ostringstream body;
  boost::property_tree::json_parser::write_json(body, pt);
  const boost::property_tree::ptree res = Try(call("/login", body.str()));
  if (verbosity::is_verbose()) {
    boost::property_tree::json_parser::write_json(std::cout, res);
  }
  return Ok(res.get<bool>("data"));
}

} // namespace poac::util::net::api

#endif // POAC_UTIL_NET_HPP_

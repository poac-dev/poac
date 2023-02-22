// internal
#include "poac/util/net.hpp"

namespace poac::util::net {

// Create progress bar, [====>   ]
Fn to_progress(const i32& max_count, i32 now_count, const i32& bar_size)
    ->String {
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
Fn to_byte_progress(const i32& max_count, i32 now_count)->String {
  if (now_count > max_count) {
    now_count = max_count;
  }
  return format(
      "{} {}/{}", to_progress(max_count, now_count),
      util::pretty::to_byte(now_count), util::pretty::to_byte(max_count)
  );
}

Fn MultiPartForm::get_files() const->Vec<MultiPartForm::FileInfo> {
  Vec<FileInfo> file_info;
  for (Let& f : m_file_param) {
    const Path file_path = std::get<1>(f);
    file_info.push_back({file_path.string(), fs::file_size(file_path)});
  }
  return file_info;
}

void MultiPartForm::generate_header() {
  m_header = format("{}{}", m_crlf, fmt::join(m_form_param, ""));
  for (Let & [ name, filename, header ] : m_file_param) {
    String h = format(
        R"(--{}{}{}name="{}"; filename="{}")", m_boundary, m_crlf,
        m_content_disposition, name, filename.filename().string()
    );
    for (Let & [ field, content ] : header) {
      // NOLINTNEXTLINE(google-readability-casting)
      h += format("{}{}: {}", m_crlf, String(to_string(field)), content);
    }
    m_header += m_crlf + h;
  }
  m_header += m_crlf + m_crlf;
}

// TODO(ken-matsui): ioc, ctx,
// resolver,...等はget等を呼び出し後，解体し，host等は残すことで，連続で呼び出し可能にする．
// Only SSL usage
void Requests::close_stream() const {
  // Gracefully close the stream
  boost::system::error_code ec;
  stream->shutdown(ec);
  if (ec == boost::asio::error::eof) {
    // Rationale: https://stackoverflow.com/q/25587403
    ec.assign(0, ec.category());
  }
}

void Requests::ssl_set_tlsext() const {
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

} // namespace poac::util::net

namespace poac::util::net::api {

[[nodiscard]] Fn call(StringRef path, const Option<String>& body) noexcept
    -> Result<boost::property_tree::ptree, String> {
  try {
    const Requests request{"api.poac.dev"};
    const String target = format("/v1{}", path);
    Let response =
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

[[nodiscard]] Fn search(StringRef query, const u64& count)
    ->Result<boost::property_tree::ptree, String> {
  boost::property_tree::ptree pt;
  pt.put("query", query);
  pt.put("perPage", count);

  std::ostringstream body;
  boost::property_tree::json_parser::write_json(body, pt);
  return call("/packages/search", body.str());
}

[[nodiscard]] Fn deps(StringRef name, StringRef version)
    ->Result<HashMap<String, String>, String> {
  const String path = format("/packages/{}/{}/deps", name, version);
  const boost::property_tree::ptree res = Try(call(path));
  if (verbosity::is_verbose()) {
    boost::property_tree::json_parser::write_json(std::cout, res);
  }
  return Ok(util::meta::to_hash_map<String>(res, "data"));
}

[[nodiscard]] Fn versions(StringRef name)->Result<Vec<String>, String> {
  const String path = format("/packages/{}/versions", name);
  const boost::property_tree::ptree res = Try(call(path));
  if (verbosity::is_verbose()) {
    boost::property_tree::json_parser::write_json(std::cout, res);
  }
  Let results = util::meta::to_vec<String>(res, "data");
  log::debug(
      "[util::net::api::versions] versions of {} are [{}]", name,
      fmt::join(results, ", ")
  );
  return Ok(results);
}

[[nodiscard]] Fn repoinfo(StringRef name, StringRef version)
    ->Result<std::pair<String, String>, String> {
  const String path = format("/packages/{}/{}/repoinfo", name, version);
  const boost::property_tree::ptree res = Try(call(path));
  if (verbosity::is_verbose()) {
    boost::property_tree::json_parser::write_json(std::cout, res);
  }
  return Ok(std::make_pair(
      res.get<String>("data.repository"), res.get<String>("data.sha256sum")
  ));
}

[[nodiscard]] Fn login(StringRef api_token)->Result<bool, String> {
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

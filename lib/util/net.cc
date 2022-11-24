// internal
#include "poac/util/net.hpp"

namespace poac::util::net {

// Create progress bar, [====>   ]
String
to_progress(const i32& max_count, i32 now_count, const i32& bar_size) {
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

Vec<MultiPartForm::FileInfo>
MultiPartForm::get_files() const {
  Vec<FileInfo> file_info;
  for (const auto& f : m_file_param) {
    const Path file_path = std::get<1>(f);
    file_info.push_back({file_path.string(), fs::file_size(file_path)});
  }
  return file_info;
}

void
MultiPartForm::generate_header() {
  m_header = format("{}{}", m_crlf, fmt::join(m_form_param, ""));
  for (const auto& [name, filename, header] : m_file_param) {
    String h = format(
        R"(--{}{}{}name="{}"; filename="{}")", m_boundary, m_crlf,
        m_content_disposition, name, filename.filename().string()
    );
    for (const auto& [field, content] : header) {
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
void
Requests::close_stream() const {
  // Gracefully close the stream
  boost::system::error_code ec;
  stream->shutdown(ec);
  if (ec == boost::asio::error::eof) {
    // Rationale: https://stackoverflow.com/q/25587403
    ec.assign(0, ec.category());
  }
}

void
Requests::ssl_set_tlsext() const {
  // Set SNI Hostname (many hosts need this to handshake successfully)
  if (!SSL_set_tlsext_host_name(
          stream->native_handle(), String(host).c_str()
      )) {
    boost::system::error_code error{
        static_cast<i32>(::ERR_get_error()),
        boost::asio::error::get_ssl_category()};
    log::debug(error.message());
    throw boost::system::system_error{error};
  }
}

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
search(StringRef query, const u64& count) noexcept {
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
  log::debug(
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

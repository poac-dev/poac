// external
#include <boost/scope_exit.hpp>

// internal
#include "poac/config.hpp"
#include "poac/core/resolver.hpp"
#include "poac/data/lockfile.hpp"
#include "poac/util/file.hpp"
#include "poac/util/registry/conan/resolver.hpp"
#include "poac/util/shell.hpp"

namespace {

constexpr poac::StringRef generator_content = R"(
from conans.model.conan_generator import Generator
from conans import ConanFile


class PoacGenerator(ConanFile):
    name = "poac_generator"
    version = "0.1.0"
    url = "https://github.com/poacpm/poac"
    description = "Conan build generator for poac build system"
    topics = ("conan", "generator", "poac")
    homepage = "https://poac.pm"
    license = "MIT"


class poac(Generator):
    @property
    def filename(self):
        return "conan_poac.json"

    @property
    def content(self):
        import json
        deps = self.deps_build_info
        return json.dumps({
          'defines': deps.defines,
          'include_paths': deps.include_paths,
          'lib_paths': deps.lib_paths,
          'libs': deps.libs
        })
)";

constexpr poac::StringRef conanfile_template = R"(
[generators]
poac

[requires]
{}

[build_requires]
poac_generator/0.1.0@poac/generator
)";

} // namespace

namespace poac::util::registry::conan::resolver {

Result<void> check_conan_command() {
  if (!poac::util::shell::has_command("conan")) {
    return Err<ConanNotFound>();
  }

  return Ok();
}

Result<void> install_conan_generator() {
  const Path generator_dir = config::data_dir / "conan" / "generator";
  const Path lockfile = generator_dir / "poac.lock";

  if (fs::exists(lockfile)) {
    return Ok();
  }

  if (!fs::exists(generator_dir)) {
    fs::create_directories(generator_dir);
  }

  const Path conanfile = generator_dir / "conanfile.py";

  log::status("Installing", "conan generator to {}", conanfile);
  Try(poac::util::file::write_file(conanfile, generator_content));

  util::shell::Cmd cmd(format("conan export {} poac/generator", generator_dir));
  const auto r = cmd.dump_stdout().stderr_to_stdout().exec();
  if (r.is_err()) {
    return Err<poac::core::resolver::FailedToResolveDepsWithCause>(
        "export conan generator failed"
    );
  }

  Try(poac::util::file::write_file(lockfile, data::lockfile::LOCKFILE_HEADER));

  return Ok();
}

inline StringRef get_conan_raw_name(StringRef name) noexcept {
  name.remove_prefix("conan::"sv.size());
  return name;
}

String format_conan_requires(
    const Vec<poac::core::resolver::resolve::Package>& packages
) {
  Vec<String> lines;
  lines.reserve(packages.size());

  for (const auto& [name, dep_info] : packages) {
    log::status("Resolved", "{} v{}", name, dep_info.version_rq);
    lines.push_back(
        format("{}/{}", get_conan_raw_name(name), dep_info.version_rq)
    );
  }

  return boost::algorithm::join(lines, "\n");
}

Result<void>
generate_conanfile(const Vec<poac::core::resolver::resolve::Package>& packages
) {
  if (!fs::exists(config::conan_deps_dir)) {
    fs::create_directories(config::conan_deps_dir);
  }

  const Path conanfile = config::conan_deps_dir / "conanfile.txt";
  const auto content =
      format(conanfile_template, format_conan_requires(packages));

  return poac::util::file::write_file(conanfile, content);
}

String get_conan_config() {
  // TODO(qqiangwu): pass build config to conan

  // I know this is ugly, but let it be
#if BOOST_OS_LINUX
  // if we are in linux, assume we are using libstdc++, with new ABI
  return " -s compiler.libcxx=libstdc++11";
#else
  return "";
#endif
}

Result<void> install_conan_packages() {
  const Path cwd = fs::current_path();
  BOOST_SCOPE_EXIT_ALL(&cwd) { fs::current_path(cwd); };

  fs::current_path(config::conan_deps_dir);
  util::shell::Cmd cmd(
      format("conan install . --build=missing {}", get_conan_config())
  );
  const auto r = cmd.dump_stdout().stderr_to_stdout().exec();
  if (r.is_err()) {
    return Err<poac::core::resolver::FailedToResolveDepsWithCause>(r.output());
  }

  return Ok();
}

Result<void>
fetch_conan_packages(const Vec<poac::core::resolver::resolve::Package>& packages
) noexcept {
  try {
    // pass all conan packages to conan and conan can resolve conflict deps
    Try(check_conan_command());
    Try(install_conan_generator());
    Try(generate_conanfile(packages));
    Try(install_conan_packages());

    return Ok();
  } catch (const std::exception& e) {
    return Err<poac::core::resolver::FailedToResolveDepsWithCause>(e.what());
  } catch (...) {
    return Err<poac::core::resolver::FailedToResolveDepsWithCause>(
        "fetching conan packages failed"
    );
  }
}

} // namespace poac::util::registry::conan::resolver

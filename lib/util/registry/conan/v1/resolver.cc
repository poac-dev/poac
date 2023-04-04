// external
#include <boost/scope_exit.hpp>

// internal
#include "poac/config.hpp"
#include "poac/core/resolver.hpp"
#include "poac/data/lockfile.hpp"
#include "poac/util/file.hpp"
#include "poac/util/registry/conan/v1/resolver.hpp"
#include "poac/util/shell.hpp"

namespace {

constexpr poac::StringRef GENERATOR_CONTENT = R"(
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

constexpr poac::StringRef CONANFILE_TEMPLATE = R"(
[generators]
poac

[requires]
{}

[build_requires]
poac_generator/0.1.0@poac/generator
)";

} // namespace

namespace poac::util::registry::conan::v1::resolver {

Fn check_conan_command()->Result<void> {
  if (!util::shell::has_command("conan")) {
    return Err<ConanNotFound>();
  }
  util::shell::Cmd cmd("conan --version");
  const auto r = cmd.dump_stderr().exec();
  if (r.is_err()) {
    return Err<ConanNotFound>();
  }
  if (!r.output().starts_with("Conan version 1.")) {
    return Err<ConanIsNotV1>();
  }

  return Ok();
}

Fn install_conan_generator()->Result<void> {
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
  Try(util::file::write_file(conanfile, GENERATOR_CONTENT));

  util::shell::Cmd cmd(format("conan export {} poac/generator", generator_dir));
  const auto r = cmd.dump_stdout().stderr_to_stdout().exec();
  if (r.is_err()) {
    return Err<core::resolver::FailedToResolveDepsWithCause>(
        "export conan generator failed"
    );
  }

  Try(util::file::write_file(lockfile, data::lockfile::LOCKFILE_HEADER));

  return Ok();
}

Fn format_conan_requires(const Vec<core::resolver::resolve::Package>& packages)
    ->String {
  Vec<String> lines;
  lines.reserve(packages.size());

  for (const auto& [name, dep_info] : packages) {
    log::status("Resolved", "{} v{}", name, dep_info.version_rq);
    lines.push_back(format("{}/{}", name, dep_info.version_rq));
  }

  return boost::algorithm::join(lines, "\n");
}

Fn generate_conanfile(const Vec<core::resolver::resolve::Package>& packages)
    ->Result<void> {
  if (!fs::exists(config::conan_deps_dir)) {
    fs::create_directories(config::conan_deps_dir);
  }

  const Path conanfile = config::conan_deps_dir / "conanfile.txt";
  const auto content =
      format(CONANFILE_TEMPLATE, format_conan_requires(packages));

  return util::file::write_file(conanfile, content);
}

Fn get_conan_config()->String {
  // TODO(qqiangwu): pass build config to conan

  // I know this is ugly, but let it be
#if BOOST_OS_LINUX
  // if we are in linux, assume we are using libstdc++, with new ABI
  return " -s compiler.libcxx=libstdc++11";
#else
  return "";
#endif
}

Fn install_conan_packages()->Result<void> {
  const Path cwd = fs::current_path();
  BOOST_SCOPE_EXIT_ALL(&cwd) { fs::current_path(cwd); };

  fs::current_path(config::conan_deps_dir);
  util::shell::Cmd cmd(
      format("conan install . --build=missing {}", get_conan_config())
  );
  const auto r = cmd.dump_stdout().stderr_to_stdout().exec();
  if (r.is_err()) {
    return Err<core::resolver::FailedToResolveDepsWithCause>(r.output());
  }

  return Ok();
}

Fn fetch_conan_packages(const Vec<core::resolver::resolve::Package>& packages
) noexcept -> Result<void> {
  try {
    // pass all conan packages to conan and conan can resolve conflict deps
    Try(check_conan_command());
    Try(install_conan_generator());
    Try(generate_conanfile(packages));
    Try(install_conan_packages());

    return Ok();
  } catch (const std::exception& e) {
    return Err<core::resolver::FailedToResolveDepsWithCause>(e.what());
  } catch (...) {
    return Err<core::resolver::FailedToResolveDepsWithCause>(
        "fetching conan packages failed"
    );
  }
}

Fn is_conan(const core::resolver::resolve::Package& package) noexcept -> bool {
  return package.dep_info.type == "conan-v1";
}

} // namespace poac::util::registry::conan::v1::resolver

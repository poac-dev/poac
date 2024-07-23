#include "Manifest.hpp"

#include "Algos.hpp"
#include "Exception.hpp"
#include "Git2.hpp"
#include "Logger.hpp"
#include "Rustify.hpp"
#include "Semver.hpp"
#include "TermColor.hpp"
#include "VersionReq.hpp"

#include <cctype>
#include <cstdlib>
#include <string>
#include <string_view>
#include <variant>

#define TOML11_NO_ERROR_PREFIX
#include <toml.hpp>

Edition::Edition(const std::string& str) : str(str) {
  if (str == "98") {
    edition = Cpp98;
    return;
  } else if (str == "03") {
    edition = Cpp03;
    return;
  } else if (str == "0x" || str == "11") {
    edition = Cpp11;
    return;
  } else if (str == "1y" || str == "14") {
    edition = Cpp14;
    return;
  } else if (str == "1z" || str == "17") {
    edition = Cpp17;
    return;
  } else if (str == "2a" || str == "20") {
    edition = Cpp20;
    return;
  } else if (str == "2b" || str == "23") {
    edition = Cpp23;
    return;
  } else if (str == "2c") {
    edition = Cpp26;
    return;
  }
  throw PoacError("invalid edition: ", str);
}

std::string
Edition::getString() const noexcept {
  return str;
}

struct Package {
  std::string name;
  Edition edition;
  Version version;
};

// NOLINTBEGIN(readability-identifier-naming)
namespace toml {
template <>
struct from<Edition> {
  static Edition from_toml(const value& val) {
    const std::string& editionStr = toml::get<std::string>(val);
    return Edition(editionStr);
  }
};
template <>
struct into<Edition> {
  static toml::value into_toml(const Edition& edition) {
    return edition.getString();
  }
};

template <>
struct from<Version> {
  static Version from_toml(const value& val) {
    const std::string& versionStr = toml::get<std::string>(val);
    return Version::parse(versionStr);
  }
};
template <>
struct into<Version> {
  static std::string into_toml(const Version& ver) {
    return ver.toString();
  }
};
} // namespace toml
// NOLINTEND(readability-identifier-naming)

TOML11_DEFINE_CONVERSION_NON_INTRUSIVE(Package, name, edition, version);

static fs::path
findManifest() {
  fs::path candidate = fs::current_path();
  while (true) {
    const fs::path configPath = candidate / "poac.toml";
    logger::debug("Finding manifest: ", configPath);
    if (fs::exists(configPath)) {
      return configPath;
    }

    const fs::path parentPath = candidate.parent_path();
    if (candidate.has_parent_path()
        && parentPath != candidate.root_directory()) {
      candidate = parentPath;
    } else {
      break;
    }
  }

  throw PoacError("could not find `poac.toml` here and in its parents");
}

struct GitDependency {
  std::string name;
  std::string url;
  Option<std::string> target;

  DepMetadata install() const;
};

struct SystemDependency {
  std::string name;
  VersionReq versionReq;

  DepMetadata install() const;
};

void
Profile::merge(const Profile& other) {
  cxxflags.insert(other.cxxflags.begin(), other.cxxflags.end());
  if (!lto) { // false is the default value
    lto = other.lto;
  }
  if (other.debug.has_value() && !debug.has_value()) {
    debug = other.debug;
  }
  if (other.opt_level.has_value() && !opt_level.has_value()) {
    opt_level = other.opt_level;
  }
}

struct Manifest {
  // Manifest is a singleton
  Manifest(const Manifest&) = delete;
  Manifest(Manifest&&) noexcept = delete;
  Manifest& operator=(const Manifest&) = delete;
  Manifest& operator=(Manifest&&) noexcept = delete;
  ~Manifest() noexcept = default;

  static Manifest& instance() {
    static Manifest instance;
    instance.load();
    return instance;
  }

  Option<fs::path> manifestPath = None;

  Option<toml::value> data = None;

  Option<Package> package = None;
  Option<Vec<std::variant<GitDependency, SystemDependency>>> dependencies =
      None;
  Option<Vec<std::variant<GitDependency, SystemDependency>>> devDependencies =
      None;

  Option<Profile> profile = None;
  Option<Profile> devProfile = None;
  Option<Profile> releaseProfile = None;

  Option<Vec<std::string>> cpplintFilters = None;

private:
  Manifest() noexcept = default;

  void load() {
    if (data.has_value()) {
      return;
    }

    if (shouldColor()) {
      toml::color::enable();
    } else {
      toml::color::disable();
    }

    manifestPath = findManifest();
    data = toml::parse(manifestPath.value());
  }
};

const fs::path&
getManifestPath() {
  return Manifest::instance().manifestPath.value();
}

// Returns an error message if the package name is invalid.
Option<std::string> // TODO: result-like types make more sense.
validatePackageName(const std::string_view name) noexcept {
  // Empty
  if (name.empty()) {
    return "must not be empty";
  }

  // Only one character
  if (name.size() == 1) {
    return "must be more than one character";
  }

  // Only lowercase letters, numbers, dashes, and underscores
  for (const char c : name) {
    if (!std::islower(c) && !std::isdigit(c) && c != '-' && c != '_') {
      return "must only contain lowercase letters, numbers, dashes, and "
             "underscores";
    }
  }

  // Start with a letter
  if (!std::isalpha(name[0])) {
    return "must start with a letter";
  }

  // End with a letter or digit
  if (!std::isalnum(name[name.size() - 1])) {
    return "must end with a letter or digit";
  }

  // Using C++ keywords
  const HashSet<std::string_view> keywords = {
#include "Keywords.def"
  };
  if (keywords.contains(name)) {
    return "must not be a C++ keyword";
  }

  return None;
}

static Package&
parsePackage() {
  Manifest& manifest = Manifest::instance();
  if (manifest.package.has_value()) {
    return manifest.package.value();
  }

  const toml::value& data = manifest.data.value();
  const auto package = toml::find<Package>(data, "package");

  if (const auto err = validatePackageName(package.name)) {
    throw PoacError(
        toml::format_error("invalid name", data.at("package.name"), err.value())
    );
  }

  manifest.package = package;
  return manifest.package.value();
}

const std::string&
getPackageName() {
  return parsePackage().name;
}
const Edition&
getPackageEdition() {
  return parsePackage().edition;
}
const Version&
getPackageVersion() {
  return parsePackage().version;
}

static void
validateCxxflag(const std::string_view cxxflag) {
  // cxxflag must start with `-`
  if (cxxflag.empty() || cxxflag[0] != '-') {
    throw PoacError("cxxflag must start with `-`");
  }

  // cxxflag only contains alphanumeric characters, `-`, `_`, `=`, `+`, `:`,
  // or `.`.
  for (const char c : cxxflag) {
    if (!std::isalnum(c) && c != '-' && c != '_' && c != '=' && c != '+'
        && c != ':' && c != '.') {
      throw PoacError(
          "cxxflag must only contain alphanumeric characters, `-`, `_`, `=`, "
          "`+`, `:`, or `.`"
      );
    }
  }
}

static Profile
parseProfile(const toml::table& table) {
  Profile profile;
  if (table.contains("cxxflags") && table.at("cxxflags").is_array()) {
    const auto& cxxflags = table.at("cxxflags").as_array();
    for (const auto& flag : cxxflags) {
      if (!flag.is_string()) {
        throw PoacError("[profile.cxxflags] must be an array of strings");
      }
      const std::string flagStr = flag.as_string();
      validateCxxflag(flagStr);
      profile.cxxflags.insert(flagStr);
    }
  }
  if (table.contains("lto") && table.at("lto").is_boolean()) {
    profile.lto = table.at("lto").as_boolean();
  }
  if (table.contains("debug") && table.at("debug").is_boolean()) {
    profile.debug = table.at("debug").as_boolean();
  }
  if (table.contains("opt_level") && table.at("opt_level").is_integer()) {
    const i32 optLevel = table.at("opt_level").as_integer();
    if (optLevel < 0 || optLevel > 3) {
      throw PoacError("opt_level must be between 0 and 3");
    }
    profile.opt_level = optLevel;
  }
  return profile;
}

static Profile
getProfile(Option<std::string> profileName) {
  Manifest& manifest = Manifest::instance();
  if (!manifest.data.value().contains("profile")) {
    return {};
  }
  if (!manifest.data.value().at("profile").is_table()) {
    throw PoacError("[profile] must be a table");
  }
  const auto& table = toml::find<toml::table>(manifest.data.value(), "profile");

  if (profileName.has_value()) {
    if (!table.contains(profileName.value())) {
      return {};
    }
    if (!table.at(profileName.value()).is_table()) {
      throw PoacError("[profile.", profileName.value(), "] must be a table");
    }
    const auto& profileTable = toml::find<toml::table>(
        manifest.data.value(), "profile", profileName.value()
    );
    return parseProfile(profileTable);
  } else {
    return parseProfile(table);
  }
}

static const Profile&
getBaseProfile() {
  Manifest& manifest = Manifest::instance();
  if (manifest.profile.has_value()) {
    return manifest.profile.value();
  }

  const Profile baseProfile = getProfile(None);
  manifest.profile = baseProfile;
  return manifest.profile.value();
}

const Profile&
getDevProfile() {
  Manifest& manifest = Manifest::instance();
  if (manifest.devProfile.has_value()) {
    return manifest.devProfile.value();
  }

  Profile devProfile = getProfile("dev");
  devProfile.merge(getBaseProfile());
  if (!devProfile.debug.has_value()) {
    devProfile.debug = true;
  }
  if (!devProfile.opt_level.has_value()) {
    devProfile.opt_level = 0;
  }
  manifest.devProfile = devProfile;
  return manifest.devProfile.value();
}

const Profile&
getReleaseProfile() {
  Manifest& manifest = Manifest::instance();
  if (manifest.releaseProfile.has_value()) {
    return manifest.releaseProfile.value();
  }

  Profile releaseProfile = getProfile("release");
  releaseProfile.merge(getBaseProfile());
  if (!releaseProfile.debug.has_value()) {
    releaseProfile.debug = false;
  }
  if (!releaseProfile.opt_level.has_value()) {
    releaseProfile.opt_level = 3;
  }
  manifest.releaseProfile = releaseProfile;
  return manifest.releaseProfile.value();
}

const Vec<std::string>&
getLintCpplintFilters() {
  Manifest& manifest = Manifest::instance();
  if (manifest.cpplintFilters.has_value()) {
    return manifest.cpplintFilters.value();
  }

  const auto& table = toml::get<toml::table>(*manifest.data);
  Vec<std::string> filters;
  if (!table.contains("lint")) {
    filters = {};
  } else {
    filters = toml::find_or<Vec<std::string>>(
        *manifest.data, "lint", "cpplint", "filters", Vec<std::string>{}
    );
  }
  manifest.cpplintFilters = filters;
  return manifest.cpplintFilters.value();
}

static fs::path
getXdgCacheHome() {
  if (const char* envP = std::getenv("XDG_CACHE_HOME")) {
    return envP;
  }
  const fs::path userDir = std::getenv("HOME");
  return userDir / ".cache";
}

static const fs::path CACHE_DIR(getXdgCacheHome() / "poac");
static const fs::path GIT_DIR(CACHE_DIR / "git");
static const fs::path GIT_SRC_DIR(GIT_DIR / "src");

static const HashSet<char> ALLOWED_CHARS = {
  '-', '_', '/', '.', '+' // allowed in the dependency name
};

static void
validateDepName(const std::string_view name) {
  if (name.empty()) {
    throw PoacError("dependency name is empty");
  }

  if (!std::isalnum(name.front())) {
    throw PoacError("dependency name must start with an alphanumeric character"
    );
  }
  if (!std::isalnum(name.back()) && name.back() != '+') {
    throw PoacError(
        "dependency name must end with an alphanumeric character or `+`"
    );
  }

  for (const char c : name) {
    if (!std::isalnum(c) && !ALLOWED_CHARS.contains(c)) {
      throw PoacError(
          "dependency name must be alphanumeric, `-`, `_`, `/`, "
          "`.`, or `+`"
      );
    }
  }

  for (usize i = 1; i < name.size(); ++i) {
    if (name[i] == '+') {
      // Allow consecutive `+` characters.
      continue;
    }

    if (!std::isalnum(name[i]) && name[i] == name[i - 1]) {
      throw PoacError(
          "dependency name must not contain consecutive non-alphanumeric "
          "characters"
      );
    }
  }
  for (usize i = 1; i < name.size() - 1; ++i) {
    if (name[i] != '.') {
      continue;
    }

    if (!std::isdigit(name[i - 1]) || !std::isdigit(name[i + 1])) {
      throw PoacError("dependency name must contain `.` wrapped by digits");
    }
  }

  HashMap<char, int> charsFreq;
  for (const char c : name) {
    ++charsFreq[c];
  }

  if (charsFreq['/'] > 1) {
    throw PoacError("dependency name must not contain more than one `/`");
  }
  if (charsFreq['+'] != 0 && charsFreq['+'] != 2) {
    throw PoacError("dependency name must contain zero or two `+`");
  }
  if (charsFreq['+'] == 2) {
    if (name.find('+') + 1 != name.rfind('+')) {
      throw PoacError("`+` in the dependency name must be consecutive");
    }
  }
}

static GitDependency
parseGitDep(const std::string& name, const toml::table& info) {
  validateDepName(name);
  std::string gitUrlStr;
  Option<std::string> target = None;

  const auto& gitUrl = info.at("git");
  if (gitUrl.is_string()) {
    gitUrlStr = gitUrl.as_string();

    // rev, tag, or branch
    for (const char* key : { "rev", "tag", "branch" }) {
      if (info.contains(key)) {
        const auto& value = info.at(key);
        if (value.is_string()) {
          target = value.as_string();
          break;
        }
      }
    }
  }
  return { name, gitUrlStr, target };
}

static SystemDependency
parseSystemDep(const std::string& name, const toml::table& info) {
  validateDepName(name);
  const auto& version = info.at("version");
  if (!version.is_string()) {
    throw PoacError("system dependency version must be a string");
  }

  const std::string versionReq = version.as_string();
  return { name, VersionReq::parse(versionReq) };
}

static Option<Vec<std::variant<GitDependency, SystemDependency>>>
parseDependencies(const char* key) {
  Manifest& manifest = Manifest::instance();
  const auto& table = toml::get<toml::table>(manifest.data.value());
  if (!table.contains(key)) {
    logger::debug("[dependencies] not found");
    return None;
  }
  const auto tomlDeps = toml::find<toml::table>(manifest.data.value(), key);

  Vec<std::variant<GitDependency, SystemDependency>> deps;
  for (const auto& dep : tomlDeps) {
    if (dep.second.is_table()) {
      const auto& info = dep.second.as_table();
      if (info.contains("git")) {
        deps.emplace_back(parseGitDep(dep.first, info));
        continue;
      } else if (info.contains("system") && info.at("system").as_boolean()) {
        deps.emplace_back(parseSystemDep(dep.first, info));
        continue;
      }
    }

    throw PoacError(
        "Only Git dependency and system dependency are supported for now: ",
        dep.first
    );
  }
  return deps;
}

DepMetadata
GitDependency::install() const {
  fs::path installDir = GIT_SRC_DIR / name;
  if (target.has_value()) {
    installDir += '-' + target.value();
  }

  if (fs::exists(installDir) && !fs::is_empty(installDir)) {
    logger::debug(name, " is already installed");
  } else {
    git2::Repository repo;
    repo.clone(url, installDir.string());

    if (target.has_value()) {
      // Checkout to target.
      const std::string target = this->target.value();
      const git2::Object obj = repo.revparseSingle(target);
      repo.setHeadDetached(obj.id());
      repo.checkoutHead(true);
    }

    logger::info(
        "Downloaded", name, ' ', target.has_value() ? target.value() : url
    );
  }

  const fs::path includeDir = installDir / "include";
  std::string includes = "-isystem ";

  if (fs::exists(includeDir) && fs::is_directory(includeDir)
      && !fs::is_empty(includeDir)) {
    includes += includeDir.string();
  } else {
    includes += installDir.string();
  }

  // Currently, no libs are supported.
  return { includes, "" };
}

DepMetadata
SystemDependency::install() const {
  const std::string pkgConfigVer = versionReq.toPkgConfigString(name);
  const std::string cflagsCmd = "pkg-config --cflags '" + pkgConfigVer + "'";
  const std::string libsCmd = "pkg-config --libs '" + pkgConfigVer + "'";

  std::string cflags = getCmdOutput(cflagsCmd);
  cflags.pop_back(); // remove '\n'
  std::string libs = getCmdOutput(libsCmd);
  libs.pop_back(); // remove '\n'

  return { cflags, libs };
}

Vec<DepMetadata>
installDependencies(const bool includeDevDeps) {
  Manifest& manifest = Manifest::instance();
  if (!manifest.dependencies.has_value()) {
    manifest.dependencies = parseDependencies("dependencies");
  }
  if (includeDevDeps && !manifest.devDependencies.has_value()) {
    manifest.devDependencies = parseDependencies("dev-dependencies");
  }

  Vec<DepMetadata> installed;
  if (manifest.dependencies.has_value()) {
    for (const auto& dep : manifest.dependencies.value()) {
      std::visit(
          [&installed](auto&& arg) { installed.emplace_back(arg.install()); },
          dep
      );
    }
  }
  if (includeDevDeps && manifest.devDependencies.has_value()) {
    for (const auto& dep : manifest.devDependencies.value()) {
      std::visit(
          [&installed](auto&& arg) { installed.emplace_back(arg.install()); },
          dep
      );
    }
  }
  return installed;
}

#ifdef POAC_TEST

namespace tests {

void
testValidateDepName() {
  assertException<PoacError>(
      []() { validateDepName(""); }, "dependency name is empty"
  );
  assertException<PoacError>(
      []() { validateDepName("-"); },
      "dependency name must start with an alphanumeric character"
  );
  assertException<PoacError>(
      []() { validateDepName("1-"); },
      "dependency name must end with an alphanumeric character or `+`"
  );

  for (unsigned char c = 0; c < 255; ++c) {
    if (std::isalnum(c) || ALLOWED_CHARS.contains(c)) {
      continue;
    }
    assertException<PoacError>(
        [c]() { validateDepName("1" + std::string(1, c) + "1"); },
        "dependency name must be alphanumeric, `-`, `_`, `/`, `.`, or `+`"
    );
  }

  assertException<PoacError>(
      []() { validateDepName("1--1"); },
      "dependency name must not contain consecutive non-alphanumeric characters"
  );
  assertNoException([]() { validateDepName("1-1-1"); });

  assertNoException([]() { validateDepName("1.1"); });
  assertNoException([]() { validateDepName("1.1.1"); });
  assertException<PoacError>(
      []() { validateDepName("a.a"); },
      "dependency name must contain `.` wrapped by digits"
  );

  assertNoException([]() { validateDepName("a/b"); });
  assertException<PoacError>(
      []() { validateDepName("a/b/c"); },
      "dependency name must not contain more than one `/`"
  );

  assertException<PoacError>(
      []() { validateDepName("a+"); },
      "dependency name must contain zero or two `+`"
  );
  assertException<PoacError>(
      []() { validateDepName("a+++"); },
      "dependency name must contain zero or two `+`"
  );

  assertException<PoacError>(
      []() { validateDepName("a+b+c"); },
      "`+` in the dependency name must be consecutive"
  );

  // issue #921
  assertNoException([]() { validateDepName("gtkmm-4.0"); });
  assertNoException([]() { validateDepName("ncurses++"); });

  pass();
}

} // namespace tests

int
main() {
  tests::testValidateDepName();
}

#endif

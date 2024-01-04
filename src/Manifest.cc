#include "Manifest.hpp"

#include "Algos.hpp"
#include "Exception.hpp"
#include "Logger.hpp"
#include "Rustify.hpp"
#include "Semver.hpp"
#include "TermColor.hpp"
#include "VersionReq.hpp"

#include <cctype>
#include <cstdlib>
#include <string>
#include <variant>

#define TOML11_NO_ERROR_PREFIX
#include <toml.hpp>

static Path findManifest() {
  Path candidate = fs::current_path();
  while (true) {
    const Path config_path = candidate / "poac.toml";
    Logger::debug("Finding manifest: ", config_path);
    if (fs::exists(config_path)) {
      return config_path;
    }

    const Path parent_path = candidate.parent_path();
    if (candidate.has_parent_path()
        && parent_path != candidate.root_directory()) {
      candidate = parent_path;
    } else {
      break;
    }
  }

  throw PoacError("could not find `poac.toml` here and in its parents");
}

struct GitDependency {
  String name;
  String url;
  Option<String> target;

  DepMetadata install() const;
};

struct SystemDependency {
  String name;
  VersionReq versionReq;

  DepMetadata install() const;
};

void Profile::merge(const Profile& other) {
  if (!lto) { // false is the default value
    lto = other.lto;
  }
}

struct Manifest {
  static Manifest& instance() noexcept {
    static Manifest instance;
    instance.load();
    return instance;
  }

  void load() {
    if (data.has_value()) {
      return;
    }

    if (shouldColor()) {
      toml::color::enable();
    } else {
      toml::color::disable();
    }

    data = toml::parse(findManifest());
  }

  Option<toml::value> data = None;

  Option<String> packageName = None;
  Option<String> packageEdition = None;
  Option<Version> packageVersion = None;

  Option<Vec<std::variant<GitDependency, SystemDependency>>> dependencies =
      None;

  Option<Profile> profile = None;
  Option<Profile> debugProfile = None;
  Option<Profile> releaseProfile = None;

  Option<Vec<String>> cpplintFilters = None;

private:
  Manifest() noexcept = default;

  // Delete copy constructor and assignment operator to prevent copying
  Manifest(const Manifest&) = delete;
  Manifest& operator=(const Manifest&) = delete;
};

String getPackageName() {
  Manifest& manifest = Manifest::instance();
  if (manifest.packageName.has_value()) {
    Logger::debug("[package.name] is cached");
    return manifest.packageName.value();
  }
  Logger::debug("[package.name] is not cached");

  const String packageName =
      toml::find<String>(manifest.data.value(), "package", "name");
  if (packageName.empty()) {
    throw PoacError("[package.name] is empty");
  }
  manifest.packageName = packageName;
  return packageName;
}

u16 editionToYear(StringRef edition) {
  if (edition == "98") {
    return 1998;
  } else if (edition == "03") {
    return 2003;
  } else if (edition == "0x" || edition == "11") {
    return 2011;
  } else if (edition == "1y" || edition == "14") {
    return 2014;
  } else if (edition == "1z" || edition == "17") {
    return 2017;
  } else if (edition == "2a" || edition == "20") {
    return 2020;
  } else if (edition == "2b" || edition == "23") {
    return 2023;
  } else if (edition == "2c") {
    return 2026;
  }
  throw PoacError("invalid edition: ", edition);
}

String getPackageEdition() {
  Manifest& manifest = Manifest::instance();
  if (manifest.packageEdition.has_value()) {
    return manifest.packageEdition.value();
  }

  const String edition =
      toml::find<String>(manifest.data.value(), "package", "edition");
  editionToYear(edition); // verification

  manifest.packageEdition = edition;
  return edition;
}

Version getPackageVersion() {
  Manifest& manifest = Manifest::instance();
  if (manifest.packageVersion.has_value()) {
    return manifest.packageVersion.value();
  }

  const String versionStr =
      toml::find<String>(manifest.data.value(), "package", "version");
  const Version version = Version::parse(versionStr);
  manifest.packageVersion = version;
  return version;
}

static Profile getProfile(Option<String> profileName) {
  Manifest& manifest = Manifest::instance();
  if (!manifest.data.value().contains("profile")) {
    return {};
  }
  if (!manifest.data.value().at("profile").is_table()) {
    throw PoacError("[profile] must be a table");
  }
  auto& table = toml::find<toml::table>(manifest.data.value(), "profile");

  if (profileName.has_value()) {
    if (!table.contains(profileName.value())) {
      return {};
    }
    if (!table.at(profileName.value()).is_table()) {
      throw PoacError("[profile.", profileName.value(), "] must be a table");
    }
    table = toml::find<toml::table>(
        manifest.data.value(), "profile", profileName.value()
    );
  }

  Profile profile;
  if (table.contains("lto") && table.at("lto").is_boolean()) {
    profile.lto = table.at("lto").as_boolean();
  }
  return profile;
}

static Profile getBaseProfile() {
  Manifest& manifest = Manifest::instance();
  if (manifest.profile.has_value()) {
    return manifest.profile.value();
  }

  Profile baseProfile = getProfile(None);
  manifest.profile = baseProfile;
  return baseProfile;
}

Profile getDebugProfile() {
  Manifest& manifest = Manifest::instance();
  if (manifest.debugProfile.has_value()) {
    return manifest.debugProfile.value();
  }

  Profile debugProfile = getProfile("debug");
  debugProfile.merge(getBaseProfile());
  manifest.debugProfile = debugProfile;
  return debugProfile;
}

Profile getReleaseProfile() {
  Manifest& manifest = Manifest::instance();
  if (manifest.releaseProfile.has_value()) {
    return manifest.releaseProfile.value();
  }

  Profile releaseProfile = getProfile("release");
  releaseProfile.merge(getBaseProfile());
  manifest.releaseProfile = releaseProfile;
  return releaseProfile;
}

Vec<String> getLintCpplintFilters() {
  Manifest& manifest = Manifest::instance();
  if (manifest.cpplintFilters.has_value()) {
    return manifest.cpplintFilters.value();
  }

  const auto& table = toml::get<toml::table>(*manifest.data);
  Vec<String> filters;
  if (!table.contains("lint")) {
    filters = {};
  } else {
    filters = toml::find_or<Vec<String>>(
        *manifest.data, "lint", "cpplint", "filters", Vec<String>{}
    );
  }
  manifest.cpplintFilters = filters;
  return filters;
}

static Path getXdgCacheHome() {
  if (const char* env_p = std::getenv("XDG_CACHE_HOME")) {
    return env_p;
  }
  const Path userDir = std::getenv("HOME");
  return userDir / ".cache";
}

static inline const Path CACHE_DIR(getXdgCacheHome() / "poac");
static inline const Path GIT_DIR(CACHE_DIR / "git");
static inline const Path GIT_SRC_DIR(GIT_DIR / "src");

// Dependency name can contain alphanumeric characters, and non-leading &
// non-trailing & non-consecutive `-`, and `_`.  Also, `/` is allowed only
// once with the same constrains as `-` and `_`.
static void validateDepName(StringRef name) {
  if (name.empty()) {
    throw PoacError("dependency name is empty");
  }

  // Leading `-`, `_`, and `/` are not allowed.
  if (!std::isalnum(name[0])) {
    throw PoacError("dependency name must start with an alphanumeric character"
    );
  }
  // Trailing `-`, `_`, and `/` are not allowed.
  if (!std::isalnum(name.back())) {
    throw PoacError("dependency name must end with an alphanumeric character");
  }

  // Only alphanumeric characters, `-`, `_`, and `/` are allowed.
  for (const char c : name) {
    if (!std::isalnum(c) && c != '-' && c != '_' && c != '/') {
      throw PoacError("dependency name must be alphanumeric, `-`, `_`, or `/`");
    }
  }

  // Consecutive `-`, `_`, and `/` are not allowed.
  for (usize i = 1; i < name.size(); ++i) {
    if (!std::isalnum(name[i]) && name[i] == name[i - 1]) {
      throw PoacError(
          "dependency name must not contain consecutive non-alphanumeric "
          "characters"
      );
    }
  }

  // `/` is allowed only once.
  if (std::count(name.begin(), name.end(), '/') > 1) {
    throw PoacError("dependency name must not contain more than one `/`");
  }
}

static void validateGitUrl(StringRef url) {
  if (url.empty()) {
    throw PoacError("git url is empty");
  }

  // start with "https://" for now
  if (!url.starts_with("https://")) {
    throw PoacError("git url must start with \"https://\"");
  }
  // end with ".git"
  if (!url.ends_with(".git")) {
    throw PoacError("git url must end with \".git\"");
  }
}

static void validateGitRev(StringRef rev) {
  if (rev.empty()) {
    throw PoacError("git rev is empty");
  }

  // The length of a SHA-1 hash is 40 characters.
  if (rev.size() != 40) {
    throw PoacError("git rev must be 40 characters");
  }
  // The characters must be in the range of [0-9a-f].
  for (const char c : rev) {
    if (!std::isxdigit(c)) {
      throw PoacError("git rev must be in the range of [0-9a-f]");
    }
  }
}

static void validateGitTagAndBranch(StringRef target) {
  if (target.empty()) {
    throw PoacError("git tag or branch is empty");
  }

  // The length of a tag or branch is less than 256 characters.
  if (target.size() >= 256) {
    throw PoacError("git tag or branch must be less than 256 characters");
  }

  // The first character must be an alphabet.
  if (!std::isalpha(target[0])) {
    throw PoacError("git tag or branch must start with an alphabet");
  }

  // The characters must be in the range of [0-9a-zA-Z_-].
  for (const char c : target) {
    if (!std::isalnum(c) && c != '_' && c != '-') {
      throw PoacError("git tag or branch must be in the range of [0-9a-zA-Z_-]"
      );
    }
  }
}

static const HashMap<StringRef, Fn<void(StringRef)>> gitValidators = {
  { "rev", validateGitRev },
  { "tag", validateGitTagAndBranch },
  { "branch", validateGitTagAndBranch },
};

static GitDependency parseGitDep(const String& name, const toml::table& info) {
  validateDepName(name);
  String gitUrlStr;
  Option<String> target = None;

  const auto& gitUrl = info.at("git");
  if (gitUrl.is_string()) {
    gitUrlStr = gitUrl.as_string();
    validateGitUrl(gitUrlStr);

    // rev, tag, or branch
    for (const String key : { "rev", "tag", "branch" }) {
      if (info.contains(key)) {
        const auto& value = info.at(key);
        if (value.is_string()) {
          target = value.as_string();
          gitValidators.at(key)(target.value());
          break;
        }
      }
    }
  }
  return { name, gitUrlStr, target };
}

static SystemDependency
parseSystemDep(const String& name, const toml::table& info) {
  validateDepName(name);
  const auto& version = info.at("version");
  if (!version.is_string()) {
    throw PoacError("system dependency version must be a string");
  }

  const String versionReq = version.as_string();
  return { name, VersionReq::parse(versionReq) };
}

static void parseDependencies() {
  Manifest& manifest = Manifest::instance();
  if (manifest.dependencies.has_value()) {
    return;
  }

  const auto& table = toml::get<toml::table>(manifest.data.value());
  if (!table.contains("dependencies")) {
    Logger::debug("no dependencies");
    return;
  }
  const auto tomlDeps =
      toml::find<toml::table>(manifest.data.value(), "dependencies");

  Vec<std::variant<GitDependency, SystemDependency>> deps;
  for (const auto& dep : tomlDeps) {
    if (dep.second.is_table()) {
      const auto& info = dep.second.as_table();
      if (info.contains("git")) {
        deps.push_back(parseGitDep(dep.first, info));
        continue;
      } else if (info.contains("system") && info.at("system").as_boolean()) {
        deps.push_back(parseSystemDep(dep.first, info));
        continue;
      }
    }

    throw PoacError(
        "Only Git dependency and system dependency are supported for now: ",
        dep.first
    );
  }
  manifest.dependencies = deps;
}

DepMetadata GitDependency::install() const {
  Path installDir = GIT_SRC_DIR / name;
  if (target.has_value()) {
    installDir += '-' + target.value();
  }

  if (fs::exists(installDir) && !fs::is_empty(installDir)) {
    Logger::debug(name, " is already installed");
  } else {
    const String gitCloneCmd = "git clone " + url + " " + installDir.string();
    if (runCmd(gitCloneCmd + " >/dev/null 2>&1") != EXIT_SUCCESS) {
      throw PoacError("failed to clone ", url, " to ", installDir.string());
    }

    if (target.has_value()) {
      const String target = this->target.value();

      const String gitResetCmd =
          "git -C " + installDir.string() + " reset --hard " + target;
      if (runCmd(gitResetCmd + " >/dev/null 2>&1") != EXIT_SUCCESS) {
        throw PoacError("failed to reset ", url, " to ", target);
      }
    }

    Logger::info(
        "Downloaded", name, ' ', target.has_value() ? target.value() : url
    );
  }

  const Path includeDir = installDir / "include";
  if (fs::exists(includeDir) && fs::is_directory(includeDir)
      && !fs::is_empty(includeDir)) {
    return { "-I" + includeDir.string(), "" };
  } else {
    return { "-I" + installDir.string(), "" };
  }
  // currently, no libs are supported.
}

DepMetadata SystemDependency::install() const {
  const String pkgConfigVer = versionReq.to_pkg_config_string(name);
  const String cflagsCmd = "pkg-config --cflags '" + pkgConfigVer + "'";
  const String libsCmd = "pkg-config --libs '" + pkgConfigVer + "'";

  String cflags = getCmdOutput(cflagsCmd);
  cflags.pop_back(); // remove '\n'
  String libs = getCmdOutput(libsCmd);
  libs.pop_back(); // remove '\n'

  return { cflags, libs };

  // TODO: do this instead of above.  We need to emit -MM depfile within
  // the generated Makefile.
  // return {
  //     "$(shell pkg-config --cflags '" + pkgConfigVer + "')",
  //     "$(shell pkg-config --libs '" + pkgConfigVer + "')"
  // };
}

Vec<DepMetadata> installDependencies() {
  parseDependencies();

  Manifest& manifest = Manifest::instance();
  if (!manifest.dependencies.has_value()) {
    return {};
  }

  Vec<DepMetadata> installed;
  for (const auto& dep : manifest.dependencies.value()) {
    std::visit(
        [&installed](auto&& arg) { installed.emplace_back(arg.install()); }, dep
    );
  }
  return installed;
}

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
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <optional>
#include <string>
#include <string_view>
#include <toml.hpp>
#include <unordered_map>
#include <unordered_set>
#include <variant>
#include <vector>

Edition::Edition(const std::string& str) : mStr(str) {
  if (str == "98") {
    mEdition = Cpp98;
    return;
  } else if (str == "03") {
    mEdition = Cpp03;
    return;
  } else if (str == "0x" || str == "11") {
    mEdition = Cpp11;
    return;
  } else if (str == "1y" || str == "14") {
    mEdition = Cpp14;
    return;
  } else if (str == "1z" || str == "17") {
    mEdition = Cpp17;
    return;
  } else if (str == "2a" || str == "20") {
    mEdition = Cpp20;
    return;
  } else if (str == "2b" || str == "23") {
    mEdition = Cpp23;
    return;
  } else if (str == "2c") {
    mEdition = Cpp26;
    return;
  }
  throw PoacError("invalid edition: ", str);
}

std::string
Edition::getString() const noexcept {
  return mStr;
}

// NOLINTBEGIN(readability-identifier-naming)
// TODO: use `m` prefix for member variables?  If we do so, we will need to
// manually define TOML conversions.
struct Package {
  std::string name;
  Edition edition;
  Version version;
};

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
    logger::debug("Finding manifest: {}", configPath.string());
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
  std::string mName;
  std::string mUrl;
  std::optional<std::string> mTarget;

  DepMetadata install() const;
};

struct SystemDependency {
  std::string mName;
  VersionReq mVersionReq;

  DepMetadata install() const;
};

void
Profile::merge(const Profile& other) {
  mCxxflags.insert(other.mCxxflags.begin(), other.mCxxflags.end());
  if (!mLto) { // false is the default value
    mLto = other.mLto;
  }
  if (other.mDebug.has_value() && !mDebug.has_value()) {
    mDebug = other.mDebug;
  }
  if (other.mOptLevel.has_value() && !mOptLevel.has_value()) {
    mOptLevel = other.mOptLevel;
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

  std::optional<fs::path> mManifestPath = std::nullopt;

  std::optional<toml::value> mData = std::nullopt;

  std::optional<Package> mPackage = std::nullopt;
  std::optional<std::vector<std::variant<GitDependency, SystemDependency>>>
      mDependencies = std::nullopt;
  std::optional<std::vector<std::variant<GitDependency, SystemDependency>>>
      mDevDependencies = std::nullopt;

  std::optional<Profile> mProfile = std::nullopt;
  std::optional<Profile> mDevProfile = std::nullopt;
  std::optional<Profile> mReleaseProfile = std::nullopt;

  std::optional<std::vector<std::string>> mCpplintFilters = std::nullopt;

private:
  Manifest() noexcept = default;

  void load() {
    if (mData.has_value()) {
      return;
    }

    if (shouldColor()) {
      toml::color::enable();
    } else {
      toml::color::disable();
    }

    mManifestPath = findManifest();
    mData = toml::parse(mManifestPath.value());
  }
};

const fs::path&
getManifestPath() {
  return Manifest::instance().mManifestPath.value();
}

fs::path
getProjectBasePath() {
  return fs::absolute(getManifestPath().parent_path());
}

// Returns an error message if the package name is invalid.
std::optional<std::string> // TODO: result-like types make more sense.
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
  const std::unordered_set<std::string_view> keywords = {
#include "Keywords.def"
  };
  if (keywords.contains(name)) {
    return "must not be a C++ keyword";
  }

  return std::nullopt;
}

static Package&
parsePackage() {
  Manifest& manifest = Manifest::instance();
  if (manifest.mPackage.has_value()) {
    return manifest.mPackage.value();
  }

  const toml::value& data = manifest.mData.value();
  const auto package = toml::find<Package>(data, "package");

  if (const auto err = validatePackageName(package.name)) {
    throw PoacError(
        toml::format_error("invalid name", data.at("package.name"), err.value())
    );
  }

  manifest.mPackage = package;
  return manifest.mPackage.value();
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
      const std::string& flagStr = flag.as_string();
      validateCxxflag(flagStr);
      profile.mCxxflags.insert(flagStr);
    }
  }
  if (table.contains("lto") && table.at("lto").is_boolean()) {
    profile.mLto = table.at("lto").as_boolean();
  }
  if (table.contains("debug") && table.at("debug").is_boolean()) {
    profile.mDebug = table.at("debug").as_boolean();
  }
  if (table.contains("opt_level") && table.at("opt_level").is_integer()) {
    const auto optLevel =
        static_cast<int32_t>(table.at("opt_level").as_integer());
    if (optLevel < 0 || optLevel > 3) {
      throw PoacError("opt_level must be between 0 and 3");
    }
    profile.mOptLevel = optLevel;
  }
  return profile;
}

static Profile
getProfile(std::optional<std::string> profileName) {
  const Manifest& manifest = Manifest::instance();
  if (!manifest.mData.value().contains("profile")) {
    return {};
  }
  if (!manifest.mData.value().at("profile").is_table()) {
    throw PoacError("[profile] must be a table");
  }
  const auto& table =
      toml::find<toml::table>(manifest.mData.value(), "profile");

  if (profileName.has_value()) {
    if (!table.contains(profileName.value())) {
      return {};
    }
    if (!table.at(profileName.value()).is_table()) {
      throw PoacError("[profile.", profileName.value(), "] must be a table");
    }
    const auto& profileTable = toml::find<toml::table>(
        manifest.mData.value(), "profile", profileName.value()
    );
    return parseProfile(profileTable);
  } else {
    return parseProfile(table);
  }
}

static const Profile&
getBaseProfile() {
  Manifest& manifest = Manifest::instance();
  if (manifest.mProfile.has_value()) {
    return manifest.mProfile.value();
  }

  const Profile baseProfile = getProfile(std::nullopt);
  manifest.mProfile = baseProfile;
  return manifest.mProfile.value();
}

const Profile&
getDevProfile() {
  Manifest& manifest = Manifest::instance();
  if (manifest.mDevProfile.has_value()) {
    return manifest.mDevProfile.value();
  }

  Profile devProfile = getProfile("dev");
  devProfile.merge(getBaseProfile());
  if (!devProfile.mDebug.has_value()) {
    devProfile.mDebug = true;
  }
  if (!devProfile.mOptLevel.has_value()) {
    devProfile.mOptLevel = 0;
  }
  manifest.mDevProfile = devProfile;
  return manifest.mDevProfile.value();
}

const Profile&
getReleaseProfile() {
  Manifest& manifest = Manifest::instance();
  if (manifest.mReleaseProfile.has_value()) {
    return manifest.mReleaseProfile.value();
  }

  Profile releaseProfile = getProfile("release");
  releaseProfile.merge(getBaseProfile());
  if (!releaseProfile.mDebug.has_value()) {
    releaseProfile.mDebug = false;
  }
  if (!releaseProfile.mOptLevel.has_value()) {
    releaseProfile.mOptLevel = 3;
  }
  manifest.mReleaseProfile = releaseProfile;
  return manifest.mReleaseProfile.value();
}

const std::vector<std::string>&
getLintCpplintFilters() {
  Manifest& manifest = Manifest::instance();
  if (manifest.mCpplintFilters.has_value()) {
    return manifest.mCpplintFilters.value();
  }

  const auto& table = toml::get<toml::table>(*manifest.mData);
  std::vector<std::string> filters;
  if (!table.contains("lint")) {
    filters = {};
  } else {
    filters = toml::find_or<std::vector<std::string>>(
        *manifest.mData, "lint", "cpplint", "filters",
        std::vector<std::string>{}
    );
  }
  manifest.mCpplintFilters = filters;
  return manifest.mCpplintFilters.value();
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

static const std::unordered_set<char> ALLOWED_CHARS = {
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

  for (size_t i = 1; i < name.size(); ++i) {
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
  for (size_t i = 1; i < name.size() - 1; ++i) {
    if (name[i] != '.') {
      continue;
    }

    if (!std::isdigit(name[i - 1]) || !std::isdigit(name[i + 1])) {
      throw PoacError("dependency name must contain `.` wrapped by digits");
    }
  }

  std::unordered_map<char, int> charsFreq;
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
  std::optional<std::string> target = std::nullopt;

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
  return { .mName = name, .mUrl = gitUrlStr, .mTarget = target };
}

static SystemDependency
parseSystemDep(const std::string& name, const toml::table& info) {
  validateDepName(name);
  const auto& version = info.at("version");
  if (!version.is_string()) {
    throw PoacError("system dependency version must be a string");
  }

  const std::string versionReq = version.as_string();
  return { .mName = name, .mVersionReq = VersionReq::parse(versionReq) };
}

static std::optional<std::vector<std::variant<GitDependency, SystemDependency>>>
parseDependencies(const char* key) {
  const Manifest& manifest = Manifest::instance();
  const auto& table = toml::get<toml::table>(manifest.mData.value());
  if (!table.contains(key)) {
    logger::debug("[dependencies] not found");
    return std::nullopt;
  }
  const auto tomlDeps = toml::find<toml::table>(manifest.mData.value(), key);

  std::vector<std::variant<GitDependency, SystemDependency>> deps;
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
  fs::path installDir = GIT_SRC_DIR / mName;
  if (mTarget.has_value()) {
    installDir += '-' + mTarget.value();
  }

  if (fs::exists(installDir) && !fs::is_empty(installDir)) {
    logger::debug("{} is already installed", mName);
  } else {
    git2::Repository repo;
    repo.clone(mUrl, installDir.string());

    if (mTarget.has_value()) {
      // Checkout to target.
      const std::string target = mTarget.value();
      const git2::Object obj = repo.revparseSingle(target);
      repo.setHeadDetached(obj.id());
      repo.checkoutHead(true);
    }

    logger::info(
        "Downloaded", "{} {}", mName,
        mTarget.has_value() ? mTarget.value() : mUrl
    );
  }

  const fs::path includeDir = installDir / "include";
  std::string includes = "-isystem";

  if (fs::exists(includeDir) && fs::is_directory(includeDir)
      && !fs::is_empty(includeDir)) {
    includes += includeDir.string();
  } else {
    includes += installDir.string();
  }

  // Currently, no libs are supported.
  return { .mIncludes = includes, .mLibs = "" };
}

DepMetadata
SystemDependency::install() const {
  const std::string pkgConfigVer = mVersionReq.toPkgConfigString(mName);
  const Command cflagsCmd =
      Command("pkg-config").addArg("--cflags").addArg(pkgConfigVer);
  const Command libsCmd =
      Command("pkg-config").addArg("--libs").addArg(pkgConfigVer);

  std::string cflags = getCmdOutput(cflagsCmd);
  cflags.pop_back(); // remove '\n'
  std::string libs = getCmdOutput(libsCmd);
  libs.pop_back(); // remove '\n'

  return { .mIncludes = cflags, .mLibs = libs };
}

std::vector<DepMetadata>
installDependencies(const bool includeDevDeps) {
  Manifest& manifest = Manifest::instance();
  if (!manifest.mDependencies.has_value()) {
    manifest.mDependencies = parseDependencies("dependencies");
  }
  if (includeDevDeps && !manifest.mDevDependencies.has_value()) {
    manifest.mDevDependencies = parseDependencies("dev-dependencies");
  }

  std::vector<DepMetadata> installed;
  if (manifest.mDependencies.has_value()) {
    for (const auto& dep : manifest.mDependencies.value()) {
      std::visit(
          [&installed](auto&& arg) { installed.emplace_back(arg.install()); },
          dep
      );
    }
  }
  if (includeDevDeps && manifest.mDevDependencies.has_value()) {
    for (const auto& dep : manifest.mDevDependencies.value()) {
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

static void
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

  for (char c = 0; c < CHAR_MAX; ++c) {
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

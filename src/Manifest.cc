#include "Manifest.hpp"

#include "Logger.hpp"
#include "Rustify.hpp"
#include "Semver.hpp"
#include "TermColor.hpp"

#include <cctype>
#include <cstdlib>
#include <stdexcept>
#include <string>

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

  throw std::runtime_error("could not find `poac.toml` here and in its parents"
  );
}

class Manifest {
public:
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

private:
  Manifest() noexcept = default;

  // Delete copy constructor and assignment operator to prevent copying
  Manifest(const Manifest&) = delete;
  Manifest& operator=(const Manifest&) = delete;
};

String getPackageName() {
  Manifest& manifest = Manifest::instance();
  if (manifest.packageName.has_value()) {
    return manifest.packageName.value();
  }

  const String packageName =
      toml::find<String>(manifest.data.value(), "package", "name");
  if (packageName.empty()) {
    throw std::runtime_error("package name is empty");
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
  throw std::runtime_error("invalid edition: " + String(edition));
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

Vec<String> getLintCpplintFilters() {
  Manifest& manifest = Manifest::instance();
  const auto& table = toml::get<toml::table>(*manifest.data);
  if (!table.contains("lint")) {
    return {};
  }
  return toml::find_or<Vec<String>>(
      *manifest.data, "lint", "cpplint", "filters", Vec<String>{}
  );
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

static void validateGitUrl(StringRef url) {
  if (url.empty()) {
    throw std::runtime_error("git url is empty");
  }

  // start with "https://" for now
  if (!url.starts_with("https://")) {
    throw std::runtime_error("git url must start with \"https://\"");
  }
  // end with ".git"
  if (!url.ends_with(".git")) {
    throw std::runtime_error("git url must end with \".git\"");
  }
}

static void validateGitRev(StringRef rev) {
  if (rev.empty()) {
    throw std::runtime_error("git rev is empty");
  }

  // The length of a SHA-1 hash is 40 characters.
  if (rev.size() != 40) {
    throw std::runtime_error("git rev must be 40 characters");
  }
  // The characters must be in the range of [0-9a-f].
  for (const char c : rev) {
    if (!std::isxdigit(c)) {
      throw std::runtime_error("git rev must be in the range of [0-9a-f]");
    }
  }
}

static void validateGitTagAndBranch(StringRef target) {
  if (target.empty()) {
    throw std::runtime_error("git tag or branch is empty");
  }

  // The length of a tag or branch is less than 256 characters.
  if (target.size() >= 256) {
    throw std::runtime_error(
        "git tag or branch must be less than 256 characters"
    );
  }

  // The first character must be an alphabet.
  if (!std::isalpha(target[0])) {
    throw std::runtime_error("git tag or branch must start with an alphabet");
  }

  // The characters must be in the range of [0-9a-zA-Z_-].
  for (const char c : target) {
    if (!std::isalnum(c) && c != '_' && c != '-') {
      throw std::runtime_error(
          "git tag or branch must be in the range of [0-9a-zA-Z_-]"
      );
    }
  }
}

static const HashMap<StringRef, Fn<void(StringRef)>> gitValidators = {
    {"rev", validateGitRev},
    {"tag", validateGitTagAndBranch},
    {"branch", validateGitTagAndBranch},
};

/// @brief Install git dependencies.  We do not need to resolve dependencies
///        (solve the SAT problem).
/// @return paths to the source files
Vec<Path> installGitDependencies() {
  Manifest& manifest = Manifest::instance();
  const auto& table = toml::get<toml::table>(*manifest.data);
  if (!table.contains("dependencies")) {
    Logger::debug("no dependencies");
    return {};
  }
  const auto deps = toml::find<toml::table>(*manifest.data, "dependencies");

  Vec<Path> gitDeps;
  for (const auto& dep : deps) {
    if (dep.second.is_table()) {
      const auto& info = dep.second.as_table();
      if (info.contains("git")) {
        const auto& gitUrl = info.at("git");
        if (gitUrl.is_string()) {
          const String gitUrlStr = gitUrl.as_string();
          validateGitUrl(gitUrlStr);

          // rev, tag, or branch
          String target = "main";
          for (const String key : {"rev", "tag", "branch"}) {
            if (info.contains(key)) {
              const auto& value = info.at(key);
              if (value.is_string()) {
                target = value.as_string();
                gitValidators.at(key)(target);
                break;
              }
            }
          }

          const Path installDir = GIT_SRC_DIR / (dep.first + '-' + target);
          if (fs::exists(installDir) && !fs::is_empty(installDir)) {
            Logger::debug(dep.first, " is already installed");
            gitDeps.push_back(installDir);
            continue;
          }

          const String gitCloneCmd =
              "git clone " + gitUrlStr + " " + installDir.string();
          if (std::system((gitCloneCmd + " >/dev/null 2>&1").c_str())
              != EXIT_SUCCESS) {
            throw std::runtime_error(
                "failed to clone " + gitUrlStr + " to " + installDir.string()
            );
          }

          const String gitResetCmd =
              "git -C " + installDir.string() + " reset --hard " + target;
          if (std::system((gitResetCmd + " >/dev/null 2>&1").c_str())
              != EXIT_SUCCESS) {
            throw std::runtime_error(
                "failed to reset " + gitUrlStr + " to " + target
            );
          }

          Logger::info(
              "Downloaded", dep.first, ' ', target.empty() ? gitUrlStr : target
          );
          gitDeps.push_back(installDir);
          continue;
        }
      }
    }

    throw std::runtime_error(
        "non-git dependency is not supported yet: " + dep.first
    );
  }
  return gitDeps;
}

#include "Manifest.hpp"

#include "Logger.hpp"
#include "Rustify.hpp"
#include "TermColor.hpp"

#include <cctype>
#include <cstdlib>
#include <iostream>
#include <stdexcept>

#define TOML11_NO_ERROR_PREFIX
#include <toml.hpp>

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

    data = toml::parse("poac.toml");
  }

  Option<toml::value> data = None;
  Option<String> packageName = None;
  Option<String> packageEdition = None;

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

String getPackageEdition() {
  Manifest& manifest = Manifest::instance();
  if (manifest.packageEdition.has_value()) {
    return manifest.packageEdition.value();
  }

  const String edition =
      toml::find<String>(manifest.data.value(), "package", "edition");
  if (edition.size() == 2 && isdigit(edition[0]) && isalnum(edition[1])) {
    manifest.packageEdition = edition;
    return edition;
  }
  throw std::runtime_error("invalid edition: " + edition);
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
          // rev, tag, or branch
          String target = "main";
          for (const String key : {"rev", "tag", "branch"}) {
            if (info.contains(key)) {
              const auto& value = info.at(key);
              if (value.is_string()) {
                target = value.as_string();
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

          const String gitUrlStr = gitUrl.as_string();
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

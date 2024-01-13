#pragma once

#include "../Rustify.hpp"
#include "Config.hpp"
#include "Exception.hpp"

#include <git2/repository.h>
#include <string>

namespace git2 {

struct Repository {
  git_repository* raw = nullptr;

  Repository() = default;
  ~Repository() {
    git_repository_free(this->raw);
  }

  Repository(const Repository&) = delete;
  Repository& operator=(const Repository&) = delete;
  Repository(Repository&&) = default;
  Repository& operator=(Repository&&) = default;

  /// Attempt to open an already-existing repository at `path`.
  ///
  /// The path can point to either a normal or bare repository.
  Repository& open(const StringRef path) {
    git2::init();
    git2Throw(git_repository_open(&this->raw, path.data()));
    return *this;
  }
  /// Attempt to open an already-existing bare repository at `path`.
  ///
  /// The path can point to only a bare repository.
  Repository& openBare(const StringRef path) {
    git2::init();
    git2Throw(git_repository_open_bare(&this->raw, path.data()));
    return *this;
  }

  /// Creates a new repository in the specified folder.
  ///
  /// This by default will create any necessary directories to create the
  /// repository, and it will read any user-specified templates when creating
  /// the repository. This behavior can be configured through `init_opts`.
  Repository& init(const StringRef path) {
    git2::init();
    git2Throw(git_repository_init(&this->raw, path.data(), false));
    return *this;
  }
  /// Creates a new `--bare` repository in the specified folder.
  ///
  /// The folder must exist prior to invoking this function.
  Repository& initBare(const StringRef path) {
    git2::init();
    git2Throw(git_repository_init(&this->raw, path.data(), true));
    return *this;
  }

  /// Get the configuration file for this repository.
  ///
  /// If a configuration file has not been set, the default config set for the
  /// repository will be returned, including global and system configurations
  /// (if they are available).
  git2::Config config() const {
    git_config* cfg = nullptr;
    git2Throw(git_repository_config(&cfg, this->raw));
    return git2::Config(cfg);
  }
};

} // end namespace git2

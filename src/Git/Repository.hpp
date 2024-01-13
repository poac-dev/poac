#pragma once

#include "../Rustify.hpp"
#include "Config.hpp"

#include <git2/repository.h>

namespace git2 {

struct Repository {
  git_repository* raw = nullptr;

  Repository() = default;
  ~Repository();

  Repository(const Repository&) = delete;
  Repository& operator=(const Repository&) = delete;
  Repository(Repository&&) = default;
  Repository& operator=(Repository&&) = default;

  /// Attempt to open an already-existing repository at `path`.
  ///
  /// The path can point to either a normal or bare repository.
  Repository& open(const StringRef path);
  /// Attempt to open an already-existing bare repository at `path`.
  ///
  /// The path can point to only a bare repository.
  Repository& openBare(const StringRef path);

  /// Creates a new repository in the specified folder.
  ///
  /// This by default will create any necessary directories to create the
  /// repository, and it will read any user-specified templates when creating
  /// the repository. This behavior can be configured through `init_opts`.
  Repository& init(const StringRef path);
  /// Creates a new `--bare` repository in the specified folder.
  ///
  /// The folder must exist prior to invoking this function.
  Repository& initBare(const StringRef path);

  /// Check if path is ignored by the ignore rules.
  bool isIgnored(const StringRef path) const;

  /// Get the configuration file for this repository.
  ///
  /// If a configuration file has not been set, the default config set for
  /// the repository will be returned, including global and system
  /// configurations (if they are available).
  git2::Config config() const;
};

} // end namespace git2

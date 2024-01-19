#include "Repository.hpp"

#include "../Rustify.hpp"
#include "Config.hpp"
#include "Exception.hpp"
#include "Oid.hpp"

#include <git2/ignore.h>
#include <git2/repository.h>
#include <git2/revparse.h>

namespace git2 {

Repository::~Repository() {
  git_repository_free(this->raw);
}

Repository&
Repository::open(const StringRef path) {
  git2Throw(git_repository_open(&this->raw, path.data()));
  return *this;
}
Repository&
Repository::openBare(const StringRef path) {
  git2Throw(git_repository_open_bare(&this->raw, path.data()));
  return *this;
}

Repository&
Repository::init(const StringRef path) {
  git2Throw(git_repository_init(&this->raw, path.data(), false));
  return *this;
}
Repository&
Repository::initBare(const StringRef path) {
  git2Throw(git_repository_init(&this->raw, path.data(), true));
  return *this;
}

bool
Repository::isIgnored(const StringRef path) const {
  int ignored = 0;
  git2Throw(git_ignore_path_is_ignored(&ignored, this->raw, path.data()));
  return static_cast<bool>(ignored);
}

Repository&
Repository::clone(
    const StringRef url, const StringRef path, const git_clone_options* opts
) {
  git2Throw(git_clone(&this->raw, url.data(), path.data(), opts));
  return *this;
}

Object
Repository::revparseSingle(const StringRef spec) const {
  git_object* obj = nullptr;
  git2Throw(git_revparse_single(&obj, this->raw, spec.data()));
  return git2::Object(obj);
}

Repository&
Repository::setHeadDetached(const Oid& oid) {
  git2Throw(git_repository_set_head_detached(this->raw, oid.raw));
  return *this;
}

git2::Config
Repository::config() const {
  git_config* cfg = nullptr;
  git2Throw(git_repository_config(&cfg, this->raw));
  return git2::Config(cfg);
}

} // end namespace git2

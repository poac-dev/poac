#include "Version.hpp"

#include "Exception.hpp"

namespace git2 {

Version::Version() : mFeatures(git2Throw(git_libgit2_features())) {
  git2Throw(git_libgit2_version(&mMajor, &mMinor, &mRev));
}

bool
Version::hasThread() const noexcept {
  return mFeatures & GIT_FEATURE_THREADS;
}

bool
Version::hasHttps() const noexcept {
  return mFeatures & GIT_FEATURE_HTTPS;
}

bool
Version::hasSsh() const noexcept {
  return mFeatures & GIT_FEATURE_SSH;
}

bool
Version::hasNsec() const noexcept {
  return mFeatures & GIT_FEATURE_NSEC;
}

std::ostream&
operator<<(std::ostream& os, const Version& version) {
  const auto flagStr = [](const bool flag) { return flag ? "on" : "off"; };
  return os << version.mMajor << '.' << version.mMinor << '.' << version.mRev
            << " (threads: " << flagStr(version.hasThread()) << ", "
            << "https: " << flagStr(version.hasHttps()) << ", "
            << "ssh: " << flagStr(version.hasSsh()) << ", "
            << "nsec: " << flagStr(version.hasNsec()) << ')';
}

} // namespace git2

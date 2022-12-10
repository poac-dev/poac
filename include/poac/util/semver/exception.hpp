#ifndef POAC_UTIL_SEMVER_EXCEPTION_HPP_
#define POAC_UTIL_SEMVER_EXCEPTION_HPP_

// std
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>

namespace semver {

struct Exception : public std::exception {
  explicit Exception(std::string what) : what_(std::move(what)) {}
  explicit Exception(const char* what) : what_(what) {}
  ~Exception() noexcept override = default;
  [[nodiscard]] inline auto what() const noexcept -> const char* override {
    return what_.c_str();
  }

  Exception(const Exception&) = default;
  auto operator=(const Exception&) -> Exception& = default;
  Exception(Exception&&) noexcept = default;
  auto operator=(Exception&&) noexcept -> Exception& = default;

private:
  std::string what_;
};

struct VersionError : public semver::Exception {
  explicit VersionError(const std::string& what_) : Exception(what_) {}
  explicit VersionError(const char* what_) : Exception(what_) {}
  ~VersionError() noexcept override = default;

  VersionError(const VersionError&) = default;
  auto operator=(const VersionError&) -> VersionError& = default;
  VersionError(VersionError&&) noexcept = default;
  auto operator=(VersionError&&) noexcept -> VersionError& = default;
};

struct InvalidIntervalError : public semver::Exception {
  explicit InvalidIntervalError(
      const std::string& interval_, const std::string& what_
  )
      : Exception("`" + interval_ + "` is invalid expression.\n" + what_) {}
  ~InvalidIntervalError() noexcept override = default;

  InvalidIntervalError(const InvalidIntervalError&) = default;
  auto operator=(const InvalidIntervalError&)
      -> InvalidIntervalError& = default;
  InvalidIntervalError(InvalidIntervalError&&) noexcept = default;
  auto operator=(InvalidIntervalError&&) noexcept
      -> InvalidIntervalError& = default;
};

struct RedundantIntervalError : public semver::Exception {
  explicit RedundantIntervalError(
      const std::string& interval_, const std::string& what_
  )
      : Exception("`" + interval_ + "` is redundant expression.\n" + what_) {}
  ~RedundantIntervalError() noexcept override = default;

  RedundantIntervalError(const RedundantIntervalError&) = default;
  auto operator=(const RedundantIntervalError&)
      -> RedundantIntervalError& = default;
  RedundantIntervalError(RedundantIntervalError&&) noexcept = default;
  auto operator=(RedundantIntervalError&&) noexcept
      -> RedundantIntervalError& = default;
};

struct StrangeIntervalError : public semver::Exception {
  explicit StrangeIntervalError(
      const std::string& interval_, const std::string& what_
  )
      : Exception("`" + interval_ + "` is strange.\n" + what_) {}
  ~StrangeIntervalError() noexcept override = default;

  StrangeIntervalError(const StrangeIntervalError&) = default;
  auto operator=(const StrangeIntervalError&)
      -> StrangeIntervalError& = default;
  StrangeIntervalError(StrangeIntervalError&&) noexcept = default;
  auto operator=(StrangeIntervalError&&) noexcept
      -> StrangeIntervalError& = default;
};

} // end namespace semver

#endif // POAC_UTIL_SEMVER_EXCEPTION_HPP_

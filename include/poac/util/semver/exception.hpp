#ifndef POAC_UTIL_SEMVER_EXCEPTION_HPP_
#define POAC_UTIL_SEMVER_EXCEPTION_HPP_

// std
#include <stdexcept>
#include <string>
#include <string_view>

namespace semver {

struct exception : public std::exception {
  explicit exception(const std::string& what) : what_(what) {}
  explicit exception(const char* what) : what_(what) {}
  ~exception() noexcept override = default;
  inline const char* what() const noexcept override { return what_.c_str(); }

  exception(const exception&) = default;
  exception& operator=(const exception&) = default;
  exception(exception&&) noexcept = default;
  exception& operator=(exception&&) noexcept = default;

protected:
  std::string what_;
};

struct version_error : public semver::exception {
  explicit version_error(const std::string& what_) : exception(what_) {}
  explicit version_error(const char* what_) : exception(what_) {}
  ~version_error() noexcept override = default;
  inline const char* what() const noexcept override { return what_.c_str(); }

  version_error(const version_error&) = default;
  version_error& operator=(const version_error&) = default;
  version_error(version_error&&) noexcept = default;
  version_error& operator=(version_error&&) noexcept = default;
};

struct invalid_interval_error : public semver::exception {
  explicit invalid_interval_error(
      const std::string& interval_, const std::string& what_
  )
      : exception("`" + interval_ + "` is invalid expression.\n" + what_) {}
  ~invalid_interval_error() noexcept override = default;
  inline const char* what() const noexcept override { return what_.c_str(); }

  invalid_interval_error(const invalid_interval_error&) = default;
  invalid_interval_error& operator=(const invalid_interval_error&) = default;
  invalid_interval_error(invalid_interval_error&&) noexcept = default;
  invalid_interval_error&
  operator=(invalid_interval_error&&) noexcept = default;
};

struct redundant_interval_error : public semver::exception {
  explicit redundant_interval_error(
      const std::string& interval_, const std::string& what_
  )
      : exception("`" + interval_ + "` is redundant expression.\n" + what_) {}
  ~redundant_interval_error() noexcept override = default;
  inline const char* what() const noexcept override { return what_.c_str(); }

  redundant_interval_error(const redundant_interval_error&) = default;
  redundant_interval_error&
  operator=(const redundant_interval_error&) = default;
  redundant_interval_error(redundant_interval_error&&) noexcept = default;
  redundant_interval_error&
  operator=(redundant_interval_error&&) noexcept = default;
};

struct strange_interval_error : public semver::exception {
  explicit strange_interval_error(
      const std::string& interval_, const std::string& what_
  )
      : exception("`" + interval_ + "` is strange.\n" + what_) {}
  ~strange_interval_error() noexcept override = default;
  inline const char* what() const noexcept override { return what_.c_str(); }

  strange_interval_error(const strange_interval_error&) = default;
  strange_interval_error& operator=(const strange_interval_error&) = default;
  strange_interval_error(strange_interval_error&&) noexcept = default;
  strange_interval_error&
  operator=(strange_interval_error&&) noexcept = default;
};

} // end namespace semver

#endif // POAC_UTIL_SEMVER_EXCEPTION_HPP_

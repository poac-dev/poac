#ifndef SEMVER_EXCEPTION_HPP
#define SEMVER_EXCEPTION_HPP

#include <stdexcept>
#include <string>

namespace semver {
    struct exception : public std::exception {
        explicit exception(const std::string& what_) : what_(what_) {}
        explicit exception(const char* what_)        : what_(what_) {}
        ~exception() noexcept override = default;
        const char* what() const noexcept override { return what_.c_str(); }

        exception(const exception&) = default;
        exception& operator=(const exception&) = default;
        exception(exception&&) noexcept = default;
        exception& operator=(exception&&) noexcept = default;

    protected:
        std::string what_;
    };

    struct version_error : public semver::exception {
        explicit version_error(const std::string& what_) : exception(what_) {}
        explicit version_error(const char* what_)        : exception(what_) {}
        ~version_error() noexcept override = default;
        const char* what() const noexcept override { return what_.c_str(); }

        version_error(const version_error&) = default;
        version_error& operator=(const version_error&) = default;
        version_error(version_error&&) noexcept = default;
        version_error& operator=(version_error&&) noexcept = default;
    };
} // end namespace semver

#endif // !SEMVER_EXCEPTION_HPP

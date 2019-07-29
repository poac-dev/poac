#ifndef GIT2_CPP_EXCEPTION_HPP
#define GIT2_CPP_EXCEPTION_HPP

#include <stdexcept>
#include <string>

#include <git2/errors.h>

namespace git2 {
    struct exception : std::exception {
        exception();
        virtual ~exception() noexcept override = default;
        virtual const char* what() const noexcept override;
        git_error_t category() const noexcept;

        exception(const exception&) = default;
        exception& operator=(const exception&) = delete;
        exception(exception&&) = default;
        exception& operator=(exception&&) = delete;

    private:
        std::string m_message = "git2-cpp: ";
        git_error_t m_category;
    };

    exception::exception() : m_category(GITERR_NONE) {
        const git_error* error = giterr_last();
        if (error != nullptr) {
            this->m_message += error->message;
            this->m_category = static_cast<git_error_t>(error->klass);
            giterr_clear();
        }
    }

    const char* exception::what() const noexcept {
        return this->m_message.c_str();
    }

    git_error_t exception::category() const noexcept {
        return this->m_category;
    }

    inline int git2_throw(const int ret) {
        if (ret < 0) {
            throw exception();
        }
        return ret;
    }
} // end namespace git2::describe

#endif	// !GIT2_CPP_EXCEPTION_HPP

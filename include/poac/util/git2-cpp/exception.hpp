#ifndef GIT2_CPP_EXCEPTION_HPP
#define GIT2_CPP_EXCEPTION_HPP

#include <stdexcept>
#include <string>
#include <git2/errors.h>
#include <git2/version.h>

namespace git2 {
#if LIBGIT2_VER_MINOR < 28
    inline const git_error* git_err_last() {
        return giterr_last();
    }

    inline void git_err_clear() {
        giterr_clear();
    }

    enum git_error_t {
        GIT_ERR_NONE = 0,
        GIT_ERR_NOMEMORY,
        GIT_ERR_OS,
        GIT_ERR_INVALID,
        GIT_ERR_REFERENCE,
        GIT_ERR_ZLIB,
        GIT_ERR_REPOSITORY,
        GIT_ERR_CONFIG,
        GIT_ERR_REGEX,
        GIT_ERR_ODB,
        GIT_ERR_INDEX,
        GIT_ERR_OBJECT,
        GIT_ERR_NET,
        GIT_ERR_TAG,
        GIT_ERR_TREE,
        GIT_ERR_INDEXER,
        GIT_ERR_SSL,
        GIT_ERR_SUBMODULE,
        GIT_ERR_THREAD,
        GIT_ERR_STASH,
        GIT_ERR_CHECKOUT,
        GIT_ERR_FETCHHEAD,
        GIT_ERR_MERGE,
        GIT_ERR_SSH,
        GIT_ERR_FILTER,
        GIT_ERR_REVERT,
        GIT_ERR_CALLBACK,
        GIT_ERR_CHERRYPICK,
        GIT_ERR_DESCRIBE,
        GIT_ERR_REBASE,
        GIT_ERR_FILESYSTEM,
        GIT_ERR_PATCH,
        GIT_ERR_WORKTREE,
        GIT_ERR_SHA1
    };
#endif

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

    exception::exception() : m_category(GIT_ERR_NONE) {
        const git_error* error = git_err_last();
        if (error != nullptr) {
            this->m_message += error->message;
            this->m_category = static_cast<git_error_t>(error->klass);
            git_err_clear();
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

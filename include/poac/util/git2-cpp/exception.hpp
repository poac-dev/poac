#ifndef GIT2_CPP_EXCEPTION_HPP
#define GIT2_CPP_EXCEPTION_HPP

#include <stdexcept>
#include <string>
#include <git2/errors.h>
#include <git2/version.h>

namespace git2 {
#if (LIBGIT2_VER_MAJOR < 1) && (LIBGIT2_VER_MINOR < 28)
    inline const git_error* git_error_last() {
        return giterr_last();
    }

    inline void git_error_clear() {
        giterr_clear();
    }

    enum git_error_t {
        GIT_ERROR_NONE = 0,
        GIT_ERROR_NOMEMORY,
        GIT_ERROR_OS,
        GIT_ERROR_INVALID,
        GIT_ERROR_REFERENCE,
        GIT_ERROR_ZLIB,
        GIT_ERROR_REPOSITORY,
        GIT_ERROR_CONFIG,
        GIT_ERROR_REGEX,
        GIT_ERROR_ODB,
        GIT_ERROR_INDEX,
        GIT_ERROR_OBJECT,
        GIT_ERROR_NET,
        GIT_ERROR_TAG,
        GIT_ERROR_TREE,
        GIT_ERROR_INDEXER,
        GIT_ERROR_SSL,
        GIT_ERROR_SUBMODULE,
        GIT_ERROR_THREAD,
        GIT_ERROR_STASH,
        GIT_ERROR_CHECKOUT,
        GIT_ERROR_FETCHHEAD,
        GIT_ERROR_MERGE,
        GIT_ERROR_SSH,
        GIT_ERROR_FILTER,
        GIT_ERROR_REVERT,
        GIT_ERROR_CALLBACK,
        GIT_ERROR_CHERRYPICK,
        GIT_ERROR_DESCRIBE,
        GIT_ERROR_REBASE,
        GIT_ERROR_FILESYSTEM,
        GIT_ERROR_PATCH,
        GIT_ERROR_WORKTREE,
        GIT_ERROR_SHA1
    };
#endif

    struct exception final : public std::exception {
        exception() : m_category(GIT_ERROR_NONE) {
            if (const git_error* error = git_error_last(); error != nullptr) {
                this->m_message += error->message;
                this->m_category = static_cast<git_error_t>(error->klass);
                git_error_clear();
            }
        }
        ~exception() noexcept override = default;

        const char* what() const noexcept override {
            return this->m_message.c_str();
        }
        git_error_t category() const noexcept {
            return this->m_category;
        }

        exception(const exception&) = default;
        exception& operator=(const exception&) = delete;
        exception(exception&&) = default;
        exception& operator=(exception&&) = delete;

    private:
        std::string m_message = "git2-cpp: ";
        git_error_t m_category;
    };

    inline int git2_throw(const int ret) {
        if (ret < 0) {
            throw exception();
        }
        return ret;
    }
} // end namespace git2::describe

#endif	// !GIT2_CPP_EXCEPTION_HPP

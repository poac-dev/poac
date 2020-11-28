#ifndef GIT2_CPP_GLOBAL_HPP
#define GIT2_CPP_GLOBAL_HPP

#include <mutex>
#include <git2/global.h>
#include <poac/util/git2-cpp/exception.hpp>

namespace git2 {
    namespace detail {
        void init() {
            git2_throw(git_libgit2_init());
        }
    }

    inline std::once_flag once;
    void init() {
        std::call_once(once, detail::init);
    }
} // end namespace git2

#endif	// !GIT2_CPP_GLOBAL_HPP

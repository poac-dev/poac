#ifndef GIT2_CPP_GLOBAL_HPP
#define GIT2_CPP_GLOBAL_HPP

#include <stdexcept>
#include <string>
#include <mutex>

#include <git2/errors.h>
#include <git2/global.h>

namespace git2 {
    namespace detail {
        void init() {
            int error = git_libgit2_init();
            if (error < 0) {
                throw std::runtime_error(
                    "Couldn't initialize the libgit2 library: " +
                    std::string(giterr_last()->message));
            }
        }
    }

    inline std::once_flag once;
    void init() {
        std::call_once(once, detail::init);
    }
} // end namespace git2

#endif	// !GIT2_CPP_GLOBAL_HPP

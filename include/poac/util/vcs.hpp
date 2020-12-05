#ifndef POAC_UTIL_VCS_HPP
#define POAC_UTIL_VCS_HPP

#include <poac/util/git2-cpp/git2.hpp>
#include <poac/util/shell.hpp>
#include <string>

namespace poac::util::vcs {
    namespace git_repo {
        void init(const std::string& path) {
            git2::repository().init(path);
        }
    }

    namespace hg_repo {
        void init() {
        }
    }

    namespace pijul_repo {
        void init() {
        }
    }

    namespace fossil_repo {
        void init() {
        }
    }
} // end namespace
#endif // !POAC_UTIL_VCS_HPP

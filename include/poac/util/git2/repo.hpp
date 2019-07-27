#ifndef GIT2_REPO_HPP
#define GIT2_REPO_HPP

#include <string>

#include <git2.h>

namespace git2::repository {
    int init(const std::string& path) {
        git_libgit2_init();

        git_repository* repo = nullptr;
        if (int error = git_repository_init(&repo, path.c_str(), false); error != 0) {
            return error;
        }
        git_repository_free(repo);

        git_libgit2_shutdown();
        return 0;
    }
} // end namespace git2

#endif	// !GIT2_REPO_HPP

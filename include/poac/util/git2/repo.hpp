#ifndef GIT2_REPO_HPP
#define GIT2_REPO_HPP

#include <string>
#include <optional>

#include <git2.h>

namespace git2::repository {
    /**
     * Creates a new Git repository in the given folder.
     *
     * TODO:
     *	- Reinit the repository
     *
     * @param path the path to the repository
     * @param is_bare if true, a Git repository without a working directory is
     *		created at the pointed path. If false, provided path will be
     *		considered as the working directory into which the .git directory
     *		will be created.
     *
     * @return 0 or an error code
     */
    int init(const std::string& path, const bool is_bare = false) {
        git_libgit2_init();

        git_repository* repo = nullptr;
        if (int error = git_repository_init(&repo, path.c_str(), is_bare); error != 0) {
            return error;
        }
        git_repository_free(repo);

        git_libgit2_shutdown();
        return 0;
    }
} // end namespace git2

#endif	// !GIT2_REPO_HPP

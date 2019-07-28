#ifndef GIT2_REPO_HPP
#define GIT2_REPO_HPP

#include <string>

#include <git2.h>

namespace git2 {
    class repository {
        git_repository* repo = nullptr;

    public:
        repository() = default;
        ~repository();

        repository(const repository&) = delete;
        repository& operator=(const repository&) = delete;
        repository(repository&&) = default;
        repository& operator=(repository&&) = default;

        void init(const std::string& path);
        void init_bare(const std::string& path);
    };

    repository::~repository() {
        git_repository_free(repo);
    }

    /// Creates a new repository in the specified folder.
    ///
    /// This by default will create any necessary directories to create the
    /// repository, and it will read any user-specified templates when creating
    /// the repository. This behavior can be configured through `init_opts`.
    void repository::init(const std::string& path) {
        git2::init();
        git_repository_init(&repo, path.c_str(), false);
    }

    /// Creates a new `--bare` repository in the specified folder.
    ///
    /// The folder must exist prior to invoking this function.
    void repository::init_bare(const std::string& path) {
        git2::init();
        git_repository_init(&repo, path.c_str(), true);
    }



    void hoge2() {
        git_libgit2_init();

        git_repository* repo = nullptr;
        git_repository_open(&repo, boost::filesystem::absolute(boost::filesystem::current_path()).string().c_str());
        git_config* cfg = nullptr;
        git_repository_config(&cfg, repo);
        git_buf strval = { nullptr, 0, 0 };
        git_config_get_string_buf(&strval, cfg, "remote.origin.url");

        std::cout << std::string(strval.ptr, strval.size) << std::endl;

        git_buf_dispose(&strval);
        git_config_free(cfg);
        git_libgit2_shutdown();
    }
} // end namespace git2

#endif	// !GIT2_REPO_HPP

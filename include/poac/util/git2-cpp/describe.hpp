#ifndef GIT2_CPP_DESCRIBE_HPP
#define GIT2_CPP_DESCRIBE_HPP

#include <string>

#include <git2.h>
#include <poac/util/git2-cpp/global.hpp>

namespace git2 {
    std::string to_string(const git_buf& buf) {
        return std::string(buf.ptr, buf.size);
    }

    class describe {
        git_describe_result* result = nullptr;

    public:
        describe() = default;
        ~describe();
//        std::string format();
    };

    describe::~describe() {
        git_describe_result_free(result);
    }

    int hoge() {
        git_libgit2_init();

//        giterr_last()->message;

        git_repository* repo = nullptr;
        if (int error = git_repository_open(&repo, boost::filesystem::absolute(boost::filesystem::current_path()).string().c_str()); error != 0) {
            return error;
        }

        git_describe_result* result;
        git_describe_options opts;
        if (int error = git_describe_init_options(&opts, GIT_DESCRIBE_OPTIONS_VERSION); error != 0) {
            return error;
        }
        opts.describe_strategy = GIT_DESCRIBE_TAGS;
        git_describe_workdir(&result, repo, &opts);

        git_buf strval = { nullptr, 0, 0 };
        git_describe_format_options opts2;
        git_describe_init_format_options(&opts2, GIT_DESCRIBE_FORMAT_OPTIONS_VERSION);
        opts2.abbreviated_size = 0;
        git_describe_format(&strval, result, &opts2);

        std::cout << to_string(strval) << std::endl;

        git_buf_dispose(&strval);

        git_libgit2_shutdown();
        return 0;
    }
} // end namespace git2::describe

#endif	// !GIT2_CPP_DESCRIBE_HPP

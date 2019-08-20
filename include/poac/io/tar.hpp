#ifndef POAC_IO_TAR_HPP
#define POAC_IO_TAR_HPP

//#include <libtar.h>

#include <iostream>
#include <string>
#include <vector>

#include <poac/io/filesystem.hpp>
#include <poac/util/shell.hpp>

namespace poac::io::tar {
    bool extract(const io::filesystem::path& filename, const std::string& options = "") {
        const std::string cmd = "tar xf " + filename.string() + " " + options;
        return util::shell(cmd).exec_ignore();
    }

    // ~/.poac/cache/package.tar.gz -> ~/.poac/cache/username-repository-tag/...
    bool extract_spec(const io::filesystem::path& input, const io::filesystem::path& output) {
        io::filesystem::create_directories(output);
        return extract(input, "-C " + output.string() + " --strip-components 1");
    }

    // It is almost the same behavior as --remove-files,
    //  but deleted in fs::remove because there is a possibility
    //   that it is not compatible with --remove-files.
    bool extract_spec_rm(const io::filesystem::path& input, const io::filesystem::path& output) {
        return extract_spec(input, output) && io::filesystem::remove(input);
    }

    bool compress_spec_exclude(
            const io::filesystem::path& input,
            const io::filesystem::path& output,
            const std::vector<std::string> opts
    ) {
        std::string exclude;
        for (const auto& v : opts) {
            exclude += "--exclude " + v + " ";
        }
        const std::string filepath = input.parent_path().relative_path().string();
        const std::string filename = input.filename().string();
        const std::string cmd = "cd " + filepath + " && " + "tar zcf " + output.string() + " " + exclude + filename;
        return util::shell(cmd).exec_ignore();
    }

    // https://gist.github.com/cat-in-136/5509961
//    bool archive(std::string source_file, std::string output_file_name) {
//        TAR* tar;
//        std::string tarFilename = "file.tar";
//        std::string srcDir = "hoge";
//        std::string extractTo = "./hoge";
//
//        int result = tar_open(&tar, tarFilename.data(), NULL, O_WRONLY | O_CREAT, 0644, TAR_GNU);
//        if (result != EXIT_SUCCESS) {
//            return EXIT_FAILURE;
//        }
//
////        tar_append_file(tar, srcDir.data(), srcDir.data());
//        result = tar_append_tree(tar, srcDir.data(), extractTo.data());
//        if (result != EXIT_SUCCESS) {
//            tar_close(tar);
//            return EXIT_FAILURE;
//        }
//
////        char* subfile1 = "./sub1/1.txt";
////        tar_append_file(tar_handle, subfile1,  subfile1);
////        char* subfile2 = "test1.c";
////        tar_append_file(tar_handle, subfile2,  subfile2);
//
//        result = tar_append_eof(tar);
//        if (result != EXIT_SUCCESS) {
//            tar_close(tar);
//            return EXIT_FAILURE;
//        }
//
//        result = tar_close(tar);
//        if (result != EXIT_SUCCESS) {
//            return EXIT_FAILURE;
//        }
//
//        return EXIT_SUCCESS;
//    }
//
//    bool archive_exclude(const fs::path path) {
//    }
//
//    bool extract() {
////        TAR* tar_handle;
////        char* tar_fname = "test.tar";
////
////        tar_open(&tar_handle, tar_fname, NULL,  O_RDONLY,  0644,  TAR_GNU);
////        char* savefold = "temp";
////        tar_extract_all(tar_handle, "temp");
////        tar_close(tar_handle);
//
//        return EXIT_SUCCESS;
//    }
} // end namespace
#endif // !POAC_IO_TAR_HPP

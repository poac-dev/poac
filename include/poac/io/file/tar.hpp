#ifndef POAC_IO_FILE_TARBALL_HPP
#define POAC_IO_FILE_TARBALL_HPP

#include <iostream>
#include <string>
#include <vector>

#include <boost/filesystem.hpp>


namespace poac::io::file::tar {
    namespace fs = boost::filesystem;

    bool extract(const fs::path& filename, const std::string& options = "") {
        const std::string cmd = "tar xf " + filename.string() + " " + options;
        return static_cast<bool>(std::system(cmd.data()));
    }

    // ~/.poac/cache/package.tar.gz -> ~/.poac/cache/username-repository-tag/...
    bool extract_spec(const fs::path& input, const fs::path& output) {
        boost::system::error_code error;
        fs::create_directories(output, error);
        return extract(input, "-C " + output.string() + " --strip-components 1");
    }

    // It is almost the same behavior as --remove-files,
    //  but deleted in fs::remove because there is a possibility
    //   that it is not compatible with --remove-files.
    bool extract_spec_rm(const fs::path& input, const fs::path& output) { // true == error
        // TODO: install.hpp用のエラー判定とfsのboolean値が逆
        return !(extract_spec(input, output) || fs::remove(input));
    }

    bool compress_spec_exclude(const fs::path& input, const fs::path& output, const std::vector<std::string> opts) {
        std::string exclude;
        for (const auto& v : opts) {
            exclude += "--exclude " + v + " ";
        }
        const std::string filepath = fs::relative(input.parent_path()).string();
        const std::string filename = input.filename().string();
        const std::string cmd = "cd " + filepath + " && " + "tar zcf " + output.string() + " " + exclude + filename;
        return static_cast<bool>(std::system(cmd.data()));
    }
} // end namespace
#endif // !POAC_IO_FILE_TARBALL_HPP

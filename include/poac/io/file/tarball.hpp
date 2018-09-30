#ifndef POAC_IO_FILE_TARBALL_HPP
#define POAC_IO_FILE_TARBALL_HPP

#include <iostream>
#include <string>
#include <vector>

#include <boost/filesystem.hpp>


namespace poac::io::file::tarball {
    bool extract(const boost::filesystem::path& filename, const std::string& options = "") {
        return static_cast<bool>(std::system(("tar -zxf " + filename.string() + " " + options).data()));
    }
    // ~/.poac/cache/package.tar.gz -> ~/.poac/cache/username-repository-tag/...
    bool extract_spec(const boost::filesystem::path& input, const boost::filesystem::path& output) {
        namespace fs = boost::filesystem;
        boost::system::error_code error;
        fs::create_directories(output, error);
        return extract(input, "-C " + output.string() + " --strip-components 1");
    }
    // It is almost the same behavior as --remove-files,
    //  but deleted in fs::remove because there is a possibility
    //   that it is not compatible with --remove-files.
    bool extract_rm_file(const boost::filesystem::path& filename, const std::string& options = "") {
        namespace fs = boost::filesystem;
        return extract(filename, options) || fs::remove(filename);
    }
    bool extract_spec_rm_file(const boost::filesystem::path& input, const boost::filesystem::path& output) {
        namespace fs = boost::filesystem;
        return !(extract_spec(input, output) || fs::remove(input)); // TODO: install.hpp用のエラー判定とfsのboolean値が逆
    }

    bool compress(const std::string& filename) {
        return static_cast<bool>(std::system(("tar -zcf " + filename + ".tar.gz " + filename).data()));
    }
    bool compress_spec(const boost::filesystem::path& input, const boost::filesystem::path& output) {
        return static_cast<bool>(std::system(("tar -zcf " + output.string() + " " + input.string()).data()));
    }
    bool compress_spec_exclude(const boost::filesystem::path& input, const boost::filesystem::path& output, const std::vector<std::string> opts) {
        namespace fs = boost::filesystem;

        std::string exclude;
        for (const auto& v : opts) {
            exclude += "--exclude " + v + " ";
        }
        const std::string filepath = fs::relative(input.parent_path()).string();
        const std::string filename = input.filename().string();
        return static_cast<bool>(std::system(("cd " + filepath + " && " + "tar -zcf " + output.string() + " " + exclude + filename).data()));
    }
} // end namespace
#endif // !POAC_IO_FILE_TARBALL_HPP

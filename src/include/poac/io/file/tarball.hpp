#ifndef POAC_IO_FILE_TARBALL_HPP
#define POAC_IO_FILE_TARBALL_HPP

#include <iostream>
#include <string>

#include <boost/filesystem.hpp>


namespace poac::io::file::tarball {
    bool extract(const std::string& filename, const std::string& options = "") {
        return static_cast<bool>(std::system(("tar -zxf " + filename + " " + options).data()));
    }
    bool extract_spec(const std::string& input, const std::string& output) {
        namespace fs = boost::filesystem;
        boost::system::error_code error;
        fs::create_directories(output, error);
        return extract(input, "-C " + output + " --strip-components 1");
    }
} // end namespace
#endif // !POAC_IO_FILE_TARBALL_HPP

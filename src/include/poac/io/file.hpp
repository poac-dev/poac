#ifndef POAC_IO_FILE_HPP
#define POAC_IO_FILE_HPP

#include <iostream>
#include <string>

#include <boost/filesystem.hpp>


namespace poac::io::file {
    // Inspired by https://stackoverflow.com/questions/4891006/how-to-create-a-folder-in-the-home-directory
    static std::string expand_user(std::string path) {
        if (!path.empty() && path[0] == '~') {
            assert(path.size() == 1 or path[1] == '/');  // or other error handling
            const char* home = std::getenv("HOME");
            if (home || ((home = std::getenv("USERPROFILE")))) {
                path.replace(0, 1, home);
            }
            else {
                const char *hdrive = std::getenv("HOMEDRIVE"),
                        *hpath  = std::getenv("HOMEPATH");
                assert(hdrive);  // or other error handling
                assert(hpath);
                path.replace(0, 1, std::string(hdrive) + hpath);
            }
        }
        return path;
    }


    // Variables
    // Config directory
    const boost::filesystem::path POAC_STATE_DIR(
            boost::filesystem::path(expand_user("~"))/boost::filesystem::path(".poac")
    );
    const boost::filesystem::path POAC_CACHE_DIR(
            POAC_STATE_DIR / boost::filesystem::path("cache")
    );
    const boost::filesystem::path POAC_TOKEN_DIR(
            POAC_STATE_DIR / boost::filesystem::path("token")
    );


    std::string basename(const std::string& s) {
        namespace fs = boost::filesystem;
        return fs::basename(fs::absolute(fs::path(s)).parent_path());;
    }

    // recursive
    bool copy_dir(const boost::filesystem::path& source,
                  const boost::filesystem::path& destination)
    {
        namespace fs = boost::filesystem;
        try
        {
            // Check whether the function call is valid
            if(
                    !fs::exists(source) ||
                    !fs::is_directory(source)
                    )
            {
                std::cerr << "Source directory " << source.string()
                          << " does not exist or is not a directory." << '\n'
                        ;
                return false;
            }
            if(fs::exists(destination))
            {
                std::cerr << "Destination directory " << destination.string()
                          << " already exists." << '\n'
                        ;
                return false;
            }
            // Create the destination directory
            if(!fs::create_directory(destination))
            {
                std::cerr << "Unable to create destination directory"
                          << destination.string() << '\n'
                        ;
                return false;
            }
        }
        catch(fs::filesystem_error const & e)
        {
            std::cerr << e.what() << '\n';
            return false;
        }
        // Iterate through the source directory
        for(
                fs::directory_iterator file(source);
                file != fs::directory_iterator(); ++file
                )
        {
            try
            {
                fs::path current(file->path());
                if(fs::is_directory(current))
                {
                    // Found directory: Recursion
                    if(
                            !copy_dir(
                                    current,
                                    destination / current.filename()
                            )
                            )
                    {
                        return false;
                    }
                }
                else
                {
                    // Found file: Copy
                    fs::copy_file(
                            current,
                            destination / current.filename()
                    );
                }
            }
            catch(fs::filesystem_error const & e)
            {
                std:: cerr << e.what() << '\n';
            }
        }
        return true;
    }

    bool extract_tar(const std::string& filename, const std::string& options="") {
        return static_cast<bool>(std::system(("tar -zxf " + filename + " " + options).data()));
    }
    bool extract_tar_spec(const std::string& input, const std::string& output) {
        namespace fs = boost::filesystem;
        boost::system::error_code error;
        fs::create_directories(output, error);
        return extract_tar(input, "-C "+output+" --strip-components 1");
    }

//    bool rename_file() {
//
//    }
} // end namespace
#endif // !POAC_IO_FILE_HPP

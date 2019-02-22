#ifndef STROITE_CORE_CMAKE_HPP
#define STROITE_CORE_CMAKE_HPP

#include <iostream>
#include <string>
#include <vector>
#include <optional>

#include <boost/filesystem.hpp>

#include "../../../core/exception.hpp"
#include "../../../io/file/path.hpp"
#include "../../command.hpp"


namespace stroite {
    struct cmake {
        boost::filesystem::path base_path;

        bool build() { // TODO: builderと同じinterfaceであるべき
            namespace fs = boost::filesystem;
            namespace util = poac::util;
            namespace path = poac::io::file::path;

            util::command cmd("cd " + base_path.string());
            if (!fs::exists(base_path / "_build")) {
                cmd &= "mkdir _build";
            }
            cmd &= "cd _build";
            util::command cmake_cmd("cmake ..");
//            for (const auto& [key, val] : cmake_args) {
//                cmake_cmd.env(key, val);
//            }
            cmd &= cmake_cmd;
            cmd &= util::command("make");
//            cmd &= util::command("make install").env("DESTDIR", "./").stderr_to_stdout();

            return cmd.exec_incontinent();

//            if (auto result = cmd.exec()) {
//                const std::string filepath_tmp = base_path.string() + "_tmp";
//                fs::rename(base_path, filepath_tmp);
//                fs::create_directories(base_path);
//
//                const fs::path built_dir(fs::path(filepath_tmp) / "_build" / "usr" / "local");
//
//                // Write to cache.yml and recurcive copy
//                for (const auto& s : std::vector<std::string>({ "bin", "include", "lib" })) {
//                    if (path::validate_dir(built_dir / s)) {
//                        path::recursive_copy(built_dir / s, fs::path(base_path) / s);
//                    }
//                }
//                fs::remove_all(filepath_tmp);
//
//                return EXIT_SUCCESS;
//            }
//            else { // error
//                return EXIT_FAILURE;
//            }
        }

        explicit cmake(const boost::filesystem::path& base_path = boost::filesystem::current_path())
        {
            namespace fs = boost::filesystem;
            namespace exception = poac::core::exception;

            if (!fs::exists(base_path / "CMakeLists.txt")) {
                throw exception::error((base_path / "CMakeLists.txt").string() + " does not exist");
            }
            this->base_path = base_path;
        }
    };
} // end namespace
#endif // STROITE_CORE_CMAKE_HPP

#ifndef POAC_CORE_STROITE_CHAIN_CMAKE_HPP
#define POAC_CORE_STROITE_CHAIN_CMAKE_HPP

#include <iostream>
#include <string>

#include <boost/filesystem.hpp>

#include "../../exception.hpp"
#include "../../../io/file/path.hpp"
#include "../../../util/command.hpp"


namespace poac::core::stroite::chain {
    struct cmake {
        boost::filesystem::path base_path;

        bool build() { // TODO: builderと同じinterfaceであるべき
            namespace fs = boost::filesystem;
            namespace path = io::file::path;

            util::command cmd("cd " + base_path.string());
            if (!fs::exists(base_path / "_build")) {
                cmd &= "mkdir _build";
            }
            cmd &= "cd _build";
            util::command cmake_cmd("cmake ..");
//            for (const auto& [key, val] : cmake_args) { // TODO: -Dhoge 渡したい
//                cmake_cmd.env(key, val);
//            }
            cmd &= cmake_cmd;
            cmd &= util::command("make");

            return cmd.exec_incontinent();
        }

        explicit cmake(const boost::filesystem::path& base_path = boost::filesystem::current_path())
        {
            namespace fs = boost::filesystem;

            if (!fs::exists(base_path / "CMakeLists.txt")) {
                throw exception::error((base_path / "CMakeLists.txt").string() + " does not exist");
            }
            this->base_path = base_path;
        }
    };
} // end namespace
#endif // POAC_CORE_STROITE_CHAIN_CMAKE_HPP

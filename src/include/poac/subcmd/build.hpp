#ifndef POAC_SUBCMD_BUILD_HPP
#define POAC_SUBCMD_BUILD_HPP

#include <iostream>
#include <string>
#include <regex>

#include <boost/filesystem.hpp>
#include <boost/range/iterator_range.hpp>

#include "../core/except.hpp"
#include "../io/file.hpp"
#include "../io/cli.hpp"
#include "../util/command.hpp"
#include "../util/package.hpp"


namespace poac::subcmd { struct build {
    static const std::string summary() { return "Beta: Compile all sources that depend on this project."; }
    static const std::string options() { return "[-v | --verbose]"; }

    template <typename VS>
    void operator()(VS&& argv) { _main(argv); }
    template <typename VS>
    void _main(VS&& argv) {
        namespace fs     = boost::filesystem;
        namespace except = core::except;

        check_arguments(argv);
        // TODO: --backend cmake

        const std::string project_name = io::file::yaml::get_node("name").as<std::string>();
        const std::string project_path = (io::file::path::current_build_bin_dir / project_name).string();

        fs::create_directories(io::file::path::current_build_bin_dir);

        // Generate object file
        const std::string compiler = "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++";
        util::command cmd(compiler);
        cmd += "-o " + project_path + ".o";
        cmd += "-std=c++17";

        /* This line is solved it
In file included from src/main.cpp:1:
In file included from /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/include/c++/v1/iostream:38:
In file included from /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/include/c++/v1/ios:215:
In file included from /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/include/c++/v1/iosfwd:90:
/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/include/c++/v1/wchar.h:119:15: fatal error:
      'wchar.h' file not found
#include_next <wchar.h>
              ^~~~~~~~~
1 error generated.
        */
        cmd += "-isysroot /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.13.sdk";
        cmd += "-I/usr/local/include -I/usr/local/lib/cmake/yaml-cpp/../../../include";
        cmd += R"(-DPOAC_ROOT=\"/Users/matken/Dropbox/Documents/project/poacpm/poac\")";
        cmd += R"(-DPOAC_VERSION=\"0.0.1\")";
//        cmd += " -pthread -DCURL_STATICLIB";
        cmd += "-c src/main.cpp";


        cmd &= "\n";

        // Link to executable file
        cmd += compiler;
        cmd += "-o " + project_path;
        /*
        const auto deps = io::file::yaml::get_node("deps");
        for (YAML::const_iterator itr = deps.begin(); itr != deps.end(); ++itr) {
            const std::string name = itr->first.as<std::string>();
            std::string src     = util::package::get_source(itr->second);
            const std::string version = util::package::get_version(itr->second, src);
            const std::string pkgname = util::package::cache_to_current(util::package::github_conv_pkgname(name, version));
            const fs::path pkgpath = io::file::path::current_deps_dir / pkgname;

            if (fs::exists(pkgpath / "include"))
                cmd += "-I" + (pkgpath / "include").string();
            if (fs::exists(pkgpath / "lib")) {
                cmd += "-L" + (pkgpath / "lib").string();
//                for ( const auto& e : boost::make_iterator_range( fs::directory_iterator( pkgpath / "lib" ), { } ) ) {
//                    if (!fs::is_directory(e)) {
//                        const std::string libname = e.path().filename().stem().string();
//                        cmd += "-l" + libname.substr(3);
//                    }
//                }
            }
        }
        cmd += "-lboost_system";
        cmd += "-lboost_filesystem";
        cmd += "-lboost_timer";
        cmd += "-lboost_chrono";

        cmd += "-lcurl";
        cmd += "-lyaml-cpp";
         */

//        cmd += "-Wl,-search_paths_first -Wl,-headerpad_max_install_names";
        cmd += "-Wl,-rpath,/usr/local/lib";
        cmd += "/usr/local/lib/libboost_system-mt.dylib";
        cmd += "/usr/local/lib/libboost_filesystem-mt.dylib";
        cmd += "/usr/local/lib/libboost_timer-mt.dylib";
        cmd += "/usr/local/lib/libboost_chrono-mt.dylib";
        cmd += "-lcurl";
        cmd += "/usr/local/lib/libyaml-cpp.0.6.2.dylib";

        cmd += project_path + ".o";


        std::cout << cmd << std::endl << std::endl;
        if (const auto ret = util::command(cmd).run()) {
            fs::remove(project_path + ".o");
            std::cout << io::cli::green << "Done:" << io::cli::reset
                      << " Please look at ./_build/bin/" + project_name
                      << std::endl;
        }
        else {
            // error
        }
    }

    void check_arguments(const std::vector<std::string>& argv) {
        namespace except = core::except;

        if (!argv.empty())
            throw except::invalid_second_arg("build");
    }
};} // end namespace
#endif // !POAC_SUBCMD_BUILD_HPP

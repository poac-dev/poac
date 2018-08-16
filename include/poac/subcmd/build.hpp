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
#include "../util/compiler.hpp"
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

        bool verbose = (argv.size() > 0 && (argv[0] == "-v" || argv[0] == "--verbose"));

        // TODO: --backend cmake

        const std::string project_name = io::file::yaml::get_node("name").as<std::string>();
        const std::string project_path = (io::file::path::current_build_bin_dir / project_name).string();

        fs::create_directories(io::file::path::current_build_bin_dir);

        // Generate object file
        util::compiler clang(project_name);
        clang.set_system("/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++");
        clang.set_version(17);
        clang.set_source_file("src/main.cpp");
        clang.set_output_path(io::file::path::current_build_bin_dir);
        clang.add_include_search_path("/usr/local/include");
        clang.add_include_search_path("/usr/local/lib/cmake/yaml-cpp/../../../include");
        // Countermeasure against 'wchar.h' file not found error.
        clang.add_other_args("-isysroot /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.13.sdk");
        clang.add_other_args(R"(-DPOAC_ROOT=\"/Users/matken/Dropbox/Documents/project/poacpm/poac\")");
        clang.add_other_args(R"(-DPOAC_VERSION=\"0.0.1\")");
////        cmd += " -pthread -DCURL_STATICLIB";

        const auto deps = io::file::yaml::get_node("deps");
        for (YAML::const_iterator itr = deps.begin(); itr != deps.end(); ++itr) {
            const std::string name = itr->first.as<std::string>();
            std::string src     = util::package::get_source(itr->second);
            const std::string version = util::package::get_version(itr->second, src);
            const std::string pkgname = util::package::cache_to_current(util::package::github_conv_pkgname(name, version));
            const fs::path pkgpath = io::file::path::current_deps_dir / pkgname;

            if (fs::exists(pkgpath / "include"))
                clang.add_include_search_path((pkgpath / "include").string());
            if (fs::exists(pkgpath / "lib")) {
                clang.add_library_search_path((pkgpath / "lib").string());
//                for ( const auto& e : boost::make_iterator_range( fs::directory_iterator( pkgpath / "lib" ), { } ) ) {
//                    if (!fs::is_directory(e)) {
//                        const std::string libname = e.path().filename().stem().string();
//                        cmd += "-l" + libname.substr(3);
//                    }
//                }
            }
        }
        clang.add_static_link_lib("boost_system");
        clang.add_static_link_lib("boost_filesystem");
        clang.add_static_link_lib("boost_timer");
        clang.add_static_link_lib("boost_chrono");
        clang.add_static_link_lib("curl");
        clang.add_static_link_lib("yaml-cpp");
//        cmd += "-Wl,-search_paths_first -Wl,-headerpad_max_install_names";
//        cmd += "-Wl,-rpath,/usr/local/lib";

        if (verbose)
            std::cout << clang << std::endl << std::endl;

        if (const auto ret = clang.compile()) {
            // TODO: compiler.hpp
            fs::remove(project_path + ".o");
            std::cout << io::cli::green << "Done: " << io::cli::reset
                      << "Output to `" + fs::relative(project_path).string() + "`"
                      << std::endl;
        }
        else {
            // error
        }
    }

    void check_arguments(const std::vector<std::string>& argv) {
        namespace except = core::except;

        if (argv.size() >= 2)
            throw except::invalid_second_arg("build");
    }
};} // end namespace
#endif // !POAC_SUBCMD_BUILD_HPP

#ifndef POAC_SUBCMD_ROOT_HPP
#define POAC_SUBCMD_ROOT_HPP

#include <iostream>
#include <string>
#include <cstdlib>

#include <boost/predef.h>
#include <boost/filesystem.hpp>

#if BOOST_OS_WINDOWS
#   include <libloaderapi.h>
#elif BOOST_OS_MACOS
#   include <mach-o/dyld.h>
#elif BOOST_OS_SOLARIS
#   include <stdlib.h>
#elif BOOST_OS_BSD_FREE
#   include <sys/sysctl.h>
#endif

#include "../core/except.hpp"


namespace poac::subcmd {
    struct root {
        static std::string summary() {
            return "Display the root installation directory";
        }
        static std::string options() {
            return "<Nothing>";
        }

        // Reference: https://stackoverflow.com/a/1024937
        template <typename VS>
        int operator()([[maybe_unused]] VS&& vs) {
            namespace fs = boost::filesystem;
            constexpr int PATH_SIZE = 1024;
            boost::system::error_code ec;

#if BOOST_OS_WINDOWS
            char path[PATH_SIZE];
            if (GetModuleFileNameA(NULL, path, sizeof(path) / sizeof(path[0])) != 0) {
                boost::filesystem::path p{ path };
                std::cout << p.parent_path().string() << std::endl;
                return EXIT_SUCCESS;
            }

#elif BOOST_OS_MACOS
            char path[PATH_SIZE];
            uint32_t size = sizeof(path);
            if (_NSGetExecutablePath(path, &size) == 0) {
                std::cout << fs::read_symlink(path, ec).parent_path().string() << std::endl;
                return EXIT_SUCCESS;
            }

#elif BOOST_OS_LINUX
            std::cout << fs::read_symlink("/proc/self/exe", ec).parent_path().string() << std::endl;
            return EXIT_SUCCESS;

#elif BOOST_OS_SOLARIS
            if (const char* path = getexecname(); path != 0) {
                boost::filesystem::path p{ path };
                std::cout << p.parent_path().string() << std::endl;
                return EXIT_SUCCESS;
            }

#elif BOOST_OS_BSD_FREE
            // Reference: https://reviews.llvm.org/file/data/yrq2xrvzohnbgsj7ougv/PHID-FILE-srzn7usmowtvqug74hnb/D5693.diff
            char path[MAXPATHLEN];
            size_t len = sizeof(path);
            int name[4] = { CTL_KERN, KERN_PROC, KERN_PROC_PATHNAME };
            name[3] = getpid();
            if (sysctl(name, 4, path, &len, NULL, 0) == 0) {
                boost::filesystem::path p{ path };
                std::cout << p.parent_path().string() << std::endl;
                return EXIT_SUCCESS;
            }

#elif BOOST_OS_BSD_DRAGONFLY
            std::cout << fs::read_symlink("/proc/curproc/file", ec).parent_path().string() << std::endl;
            return EXIT_SUCCESS;

#elif BOOST_OS_BSD_NET
            std::cout << fs::read_symlink("/proc/curproc/exe", ec).parent_path().string() << std::endl;
            return EXIT_SUCCESS;

#endif
            throw core::except::error("Could not get root installation directory");
        }
    };
} // end namespace
#endif // !POAC_SUBCMD_ROOT_HPP

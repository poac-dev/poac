#ifndef POAC_SUBCMD_CACHE_HPP
#define POAC_SUBCMD_CACHE_HPP

#include <iostream>
#include <string>
//#include <csignal>trap "rm to_TFRecord.py" 0

#include <boost/filesystem.hpp>
#include <boost/range/iterator_range.hpp>

#include "../core/except.hpp"
#include "../io/file.hpp"


namespace poac::subcmd { struct cache {
        static const std::string summary() { return "Manipulate cache files."; }
        static const std::string options() { return "<command>"; }

        template <typename VS>
        void operator()(VS&& vs) { _main(vs); }
        template <typename VS>
        void _main(VS&& argv) {
            namespace except = core::except;

            check_arguments(argv);
            if (argv[0] == "root" && argv.size() == 1)
                root();
            else if (argv[0] == "list")
                list(std::vector<std::string>(argv.begin()+1, argv.begin()+argv.size()));
            else if (argv[0] == "clean")
                clean(std::vector<std::string>(argv.begin()+1, argv.begin()+argv.size()));
            else
                throw except::invalid_second_arg("cache");
        }

        void clean(const std::vector<std::string>& argv) {
            namespace fs = boost::filesystem;
            if (argv.empty())
                fs::remove_all(io::file::path::poac_cache_dir);
        }

        void list([[maybe_unused]] const std::vector<std::string>& argv) {
            namespace fs = boost::filesystem;
            for (const auto& e : boost::make_iterator_range(fs::directory_iterator(io::file::path::poac_cache_dir), {}))
                std::cout << e.path().filename().string() << std::endl;
        }

        void root() {
            std::cout << io::file::path::poac_cache_dir.string() << std::endl;
        }

        void check_arguments(const std::vector<std::string>& argv) {
            namespace except = core::except;
            if (argv.empty()) throw except::invalid_second_arg("cache");
        }
    };} // end namespace
#endif // !POAC_SUBCMD_CACHE_HPP

#ifndef POAC_SUBCMD_TEST_HPP
#define POAC_SUBCMD_TEST_HPP

#include <iostream>
#include <string>

#include <boost/filesystem.hpp>
#include <boost/optional.hpp>
#include <boost/algorithm/string/replace.hpp>

#include "../core/exception.hpp"
#include "../io/file.hpp"
#include "../io/cli.hpp"
#include "../util/stroite.hpp"
#include "../util/argparse.hpp"


// TODO: エラーがあるならちゃんと，EXIT_FAILUREを返す
namespace poac::subcmd { struct test {
        static const std::string summary() { return "Execute tests."; }
        static const std::string options() { return "[-v | --verbose, --report, -- args]"; }

        template <typename VS, typename = std::enable_if_t<std::is_rvalue_reference_v<VS&&>>>
        void operator()(VS&& argv) { _main(std::move(argv)); }
        template <typename VS, typename = std::enable_if_t<std::is_rvalue_reference_v<VS&&>>>
        void _main(VS&& argv) {
            namespace fs     = boost::filesystem;
            namespace except = core::exception;

            check_arguments(argv);

            const auto node = io::file::yaml::load_config("test");
            const bool verbose = util::argparse::use(argv, "-v", "--verbose");

            const bool usemain = false;

            stroite::builder::config bs;
            bs.configure_compile(usemain, verbose);

            // You can use #include<> in test code.
            bs.compile_conf.include_search_path.push_back((fs::current_path() / "include").string());

            std::string static_link_lib;
            if (const auto test_framework = io::file::yaml::get<std::string>(node.at("test"), "framework")) {
                if (*test_framework == "boost") {
                    static_link_lib = "boost_unit_test_framework";
                }
                else if (*test_framework == "google") {
                    // TODO: select
                    static_link_lib = "gtest";
//                    static_link_lib = "gtest_main";
                }
                else {
                    throw except::error("Invalid test framework");
                }
            }

            for (const fs::path& p : fs::recursive_directory_iterator(fs::current_path() / "test")) {
                if (!fs::is_directory(p) && p.extension().string() == ".cpp") {
                    const std::string cpp_relative = fs::relative(p).string();
                    const std::string bin_name = fs::path(boost::replace_all_copy(fs::relative(cpp_relative, "test").string(), "/", "-")).stem().string();
                    const std::string bin_path = (io::file::path::current_build_test_bin_dir / bin_name).string();

                    bs.compile_conf.source_files = bs.hash_source_files({ cpp_relative }, usemain);
                    if (bs.compile_conf.source_files.empty()) { // No need for compile and link
                        std::cout << io::cli::yellow << "Warning: " << io::cli::reset
                                  << "There is no change. Binary exists in `" +
                                     fs::relative(bin_path).string() + "`."
                                  << std::endl;
//                        continue;
                    }
                    else {
                        if (const auto obj_files_path = bs._compile()) {
                            bs.configure_link(*obj_files_path, verbose);
                            bs.link_conf.project_name = bin_name;
                            bs.link_conf.output_root = io::file::path::current_build_test_bin_dir;
                            bs.link_conf.static_link_libs.push_back(static_link_lib);
                            if (bs._link()) {
                                std::cout << io::cli::green << "Compiled: " << io::cli::reset
                                          << "Output to `" +
                                             fs::relative(bin_path).string() +
                                             "`"
                                          << std::endl;
                            }
                            else { // Link failure
                                continue;
                            }
                        }
                        else { // Compile failure
                            continue;
                        }
                    }

                    //
                    // execute binary
                    //
                    util::command cmd(fs::relative(bin_path).string());
                    // poac test -v -- -h build
                    if (const auto result = std::find(argv.begin(), argv.end(), "--"); result != argv.end()) {
                        // -h build
                        std::vector<std::string> test_args(result+1, argv.end());
                        for (const auto& s : test_args) {
                            cmd += s;
                        }
                    }
                    else if (const auto test_args = io::file::yaml::get<std::vector<std::string>>(node.at("test"), "args")) {
                        for (const auto& s : *test_args) {
                            cmd += s;
                        }
                    }
                    if (util::argparse::use(argv, "--report")) {
                        fs::create_directories(io::file::path::current_build_test_report_dir);
                        cmd += ">";
                        cmd += (io::file::path::current_build_test_report_dir / bin_name).string() + ".xml";
                    }
                    else if (const auto test_report = io::file::yaml::get<bool>(node.at("test"), "report")) {
                        if (*test_report) {
                            fs::create_directories(io::file::path::current_build_test_report_dir);
                            cmd += ">";
                            cmd += (io::file::path::current_build_test_report_dir / bin_name).string() + ".xml";
                        }
                    }
                    // TODO: echo => Output .xml ...

                    std::cout << io::cli::green << "Running: " << io::cli::reset
                              << "`" + fs::relative(bin_path).string() + "`"
                              << std::endl;
                    if (const auto ret = cmd.exec())
                        std::cout << *ret;
                    else
                        std::cout << std::endl
                                  << fs::relative(bin_path).string() + " returned 1" << std::endl;

                    std::cout << "----------------------------------------------------------------" << std::endl;
                }
            }
        }


        void check_arguments([[maybe_unused]] const std::vector<std::string>& argv) {
            namespace except = core::exception;
//            if (argv.size() >= 2)
//                throw except::invalid_second_arg("test");
        }
    };} // end namespace
#endif // !POAC_SUBCMD_TEST_HPP

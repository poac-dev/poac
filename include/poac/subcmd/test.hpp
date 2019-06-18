#ifndef POAC_SUBCMD_TEST_HPP
#define POAC_SUBCMD_TEST_HPP

#include <iostream>
#include <string>
#include <cstdlib>

#include <boost/filesystem.hpp>
#include <boost/algorithm/string/replace.hpp>

#include "../core/except.hpp"
#include "../core/stroite.hpp"
#include "../io/path.hpp"
#include "../io/tar.hpp"
#include "../io/yaml.hpp"
#include "../io/cli.hpp"
#include "../util/argparse.hpp"


namespace poac::subcmd {
    namespace _test {
        template<typename VS>
        int _main(VS&& argv) {
            namespace fs = boost::filesystem;
            namespace except = core::except;
            namespace stroite = core::stroite;

            using namespace io::cli::color_literals;

            const auto node = io::yaml::load_config("test");
            const bool verbose = util::argparse::use(argv, "-v", "--verbose");

            const bool usemain = false;


            // {
            //   stroite::core::Builder bs(fs::current_directory());
            //   bs.test(verbose); -> build system内で，testディレクトリを掘って，どんどんテストしていく．
            //   -> つまり，build sysytem使用側が，for文回すとかは無い．また，------------------------こういう区切り線も，test内で行われる．
            //   -> ただし，その区切り線は，もちろん，quiteがtrueだと表示しない．verboseがtrueだと情報を増やす．
            // }


            stroite::core::builder bs(verbose);
            bs.configure_compile(usemain);



            /// TODO: これは重要！忘れない！！！！！！！！！！！！！！！！
            // You can use #include<> in test code. // TODO: これは，builder.hpp: 255で書いたように，build.hppでもできるようにすべき．
            bs.compile_conf.include_search_path.push_back((fs::current_path() / "include").string());




            // TODO: buildsystemで，testモード実行なら，以下の内容が付与される．
            // TODO: つまり，bs.build(), bs.test()みたいな感じ？
            std::string static_link_lib;
            if (const auto test_framework = io::yaml::get<std::string>(node.at("test"), "framework")) {
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
                    const std::string bin_name = fs::path(
                            boost::replace_all_copy(
                                    fs::relative(cpp_relative, "test").string(), "/", "-")).stem().string();
                    const std::string extension = core::stroite::utils::absorb::binary_extension;
                    const std::string bin_path = (io::path::current_build_test_bin_dir / bin_name).string() + extension;

                    // TODO: こちらでハンドリングしようと思ったのは，ioへの依存を無くそうとしたから．
                    // TODO: 無くす必要は無い．それよりも美しく書く方が重要である．
                    bs.compile_conf.source_files = bs.hash_source_files({cpp_relative}, usemain);
                    if (bs.compile_conf.source_files.empty()) { // No need for compile and link
                        std::cout << "Warning: "_yellow
                                  << "There is no change. Binary exists in `" +
                                     fs::relative(bin_path).string() + "`."
                                  << std::endl;
//                        continue;
                    }
                    else {
                        if (const auto obj_files_path = bs.compile()) {
                            bs.configure_link(*obj_files_path);
                            bs.link_conf.project_name = bin_name;
                            bs.link_conf.output_root = io::path::current_build_test_bin_dir;
                            bs.link_conf.static_link_libs.push_back(static_link_lib);
                            if (bs.link()) {
                                std::cout << "Compiled: "_green
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
                    util::shell cmd(fs::relative(bin_path).string());
                    // poac test -v -- -h build
                    if (const auto result = std::find(argv.begin(), argv.end(), "--"); result != argv.end()) {
                        // -h build
                        std::vector<std::string> test_args(result + 1, argv.end());
                        for (const auto &s : test_args) {
                            cmd += s;
                        }
                    }
                    else if (const auto test_args = io::yaml::get<std::vector<std::string>>(
                            node.at("test"), "args"))
                    {
                        for (const auto &s : *test_args) {
                            cmd += s;
                        }
                    }
                    if (util::argparse::use(argv, "--report")) {
                        fs::create_directories(io::path::current_build_test_report_dir);
                        cmd += ">";
                        cmd += (io::path::current_build_test_report_dir / bin_name).string() + ".xml";
                    }
                    else if (const auto test_report = io::yaml::get<bool>(node.at("test"), "report")) {
                        if (*test_report) {
                            fs::create_directories(io::path::current_build_test_report_dir);
                            cmd += ">";
                            cmd += (io::path::current_build_test_report_dir / bin_name).string() + ".xml";
                        }
                    }
                    // TODO: echo => Output .xml ...

                    std::cout << "Running: "_green
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
            return EXIT_SUCCESS;
        }
    }

    struct test {
        static std::string summary() {
            return "Execute tests";
        }
        static std::string options() {
            return "[-v | --verbose, --report, -- args]";
        }
        template <typename VS>
        int operator()(VS&& argv) {
            return _test::_main(std::forward<VS>(argv));
        }
    };
} // end namespace
#endif // !POAC_SUBCMD_TEST_HPP

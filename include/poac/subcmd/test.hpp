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
#include "../util/buildsystem.hpp"
#include "./build.hpp"


// TODO: 特定のテストのみcacheを無視するoption
// TODO: testに失敗した時，EXIT_FAILUREを返す
// TODO: デフォルトでは，変更がなくても，実行のみが行われるが，一つだけ変更した場合，outputが邪魔になる．
// TODO: そのため，--change-onlyオプションが必要．
namespace poac::subcmd { struct test {
        static const std::string summary() { return "Beta: Execute tests."; }
        static const std::string options() { return "[-v | --verbose, --report, -- args]"; }

        template <typename VS, typename = std::enable_if_t<std::is_rvalue_reference_v<VS&&>>>
        void operator()(VS&& argv) { _main(std::move(argv)); }
        template <typename VS, typename = std::enable_if_t<std::is_rvalue_reference_v<VS&&>>>
        void _main(VS&& argv) {
            namespace fs     = boost::filesystem;
            namespace except = core::exception;

            check_arguments(argv);

//            // TODO: buildsystemが，外部から，static_libなどを弄れるようにならならないとできない！
//            const auto node = io::file::yaml::load_setting_file("name", "version", "cpp_version", "deps", "test");
//
//            const auto first = argv.begin(), last = argv.end();
//            const bool verbose = (std::find(first, last, "-v") != last || std::find(first, last, "--verbose") != last);
//
//            // TODO: buildコマンドとの共通化
//            util::buildsystem bs;
//            subcmd::build hoge{};
//            hoge.configure(compiler, project_name, node);
//            // TODO: testは絶対に実行するな！！！
//
//            if (const auto test_framework = io::file::yaml::get1<std::string>(node.at("test"), "framework")) {
//                if (*test_framework == "boost") {
//                    compiler.add_static_link_lib("boost_unit_test_framework");
//                }
//                else if (*test_framework == "google") {
//                    // TODO: 調べる
//                    // TODO: libgtest_main.aを使う場合はmain関数が不要です。
//                    compiler.add_static_link_lib("gtest");
////                    compiler.add_static_link_lib("gtest_main");
//                }
//                else {
//                    throw except::error("Invalid test framework");
//                }
//            }
//            else {
//                throw except::error("framework key does not exists in poac.yml\n"
//                                    "```:poac.yml\n"
//                                    "test:\n"
//                                    "  framework: boost\n"
//                                    "```");
//            }
//            compiler.output_path = io::file::path::current_build_test_bin_dir;
//            // You can use #include<> in test code.
//            compiler.add_include_search_path(fs::current_path() / "include");
//
//            for (const fs::path& p : fs::recursive_directory_iterator(fs::current_path() / "test")) {
//                if (!fs::is_directory(p) && p.extension().string() == ".cpp") {
//                    const std::string bin_relative = fs::relative(p).string();
//                    const std::string bin_name = fs::path(boost::replace_all_copy(fs::relative(bin_relative, "test").string(), "/", "-")).stem().string();
//                    const std::string bin_path = (io::file::path::current_build_test_bin_dir / bin_name).string();
//
//                    compiler.project_name = bin_name;
//                    compiler.add_source_file(bin_relative);
//
//                    fs::create_directories(io::file::path::current_build_test_bin_dir);
//                    if (compiler._compile(verbose) && compiler.link(verbose)) {
//                        std::cout << io::cli::green << "Compiled: " << io::cli::reset
//                                  << "Output to `" + fs::relative(bin_path).string() + "`"
//                                  << std::endl;
//                    }
//                    else { /* error */ // TODO: compileに失敗時も表示されてしまう．
//                        std::cout << io::cli::yellow << "Warning: " << io::cli::reset
//                                  << "There is no change. Binary exists in `" + fs::relative(bin_path).string() + "`."
//                                  << std::endl;
//                    }
//
//
//                    util::command cmd(bin_path);
//
//                    // poac test -v -- -h build
//                    if (const auto result = std::find(argv.begin(), argv.end(), "--"); result != argv.end()) {
//                        // -h build
//                        std::vector<std::string> test_args(result+1, argv.end());
//                        for (const auto& s : test_args) {
//                            cmd += s;
//                        }
//                    }
//                    else if (const auto test_args = io::file::yaml::get1<std::vector<std::string>>(node.at("test"), "args")) {
//                        for (const auto& s : *test_args) {
//                            cmd += s;
//                        }
//                    }
//                    if (std::find(argv.begin(), argv.end(), "--report") != argv.end()) {
//                        fs::create_directories(io::file::path::current_build_test_report_dir);
//                        cmd += ">";
//                        cmd += (io::file::path::current_build_test_report_dir / bin_name).string() + ".xml";
//                    }
//                    else if (const auto test_report = io::file::yaml::get1<bool>(node.at("test"), "report")) {
//                        if (*test_report) {
//                            fs::create_directories(io::file::path::current_build_test_report_dir);
//                            cmd += ">";
//                            cmd += (io::file::path::current_build_test_report_dir / bin_name).string() + ".xml";
//                        }
//                    }
//                    // TODO: echo => Output .xml ...
//
//                    // TODO: compileに失敗時も実行しようとしてしまう．run.hppも同様．
//                    std::cout << io::cli::green << "Running: " << io::cli::reset
//                              << "`" + bin_path + "`"
//                              << std::endl;
//                    if (const auto ret = cmd.exec())
//                        std::cout << *ret;
//                    else // TODO: errorの時も文字列が欲しい．
//                        std::cout << std::endl
//                                  << project_name + " returned 1" << std::endl;
//
//                    // Do not use the previous object file
//                    compiler.obj_files.clear();
//                    compiler.source_files.clear();
//                    std::cout << "----------------------------------------------------------------" << std::endl;
//                }
//            }
        }


        void check_arguments([[maybe_unused]] const std::vector<std::string>& argv) {
            namespace except = core::exception;
            // TODO: optionsのが含まれてなかったら，throw
//            if (argv.size() >= 2)
//                throw except::invalid_second_arg("test");
        }
    };} // end namespace
#endif // !POAC_SUBCMD_TEST_HPP

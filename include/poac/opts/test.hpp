#ifndef POAC_OPTS_TEST_HPP
#define POAC_OPTS_TEST_HPP

#include <iostream>
#include <string>
#include <cstdlib>

#include <boost/filesystem.hpp>
#include <boost/algorithm/string/replace.hpp>

#include <poac/core/except.hpp>
#include <poac/core/builder.hpp>
#include <poac/io/path.hpp>
#include <poac/io/tar.hpp>
#include <poac/io/config.hpp>
#include <poac/io/term.hpp>
#include <poac/util/argparse.hpp>
#include <poac/util/termcolor2/termcolor2.hpp>


// args:
//    - "--log_level=test_suite" -> testのオプションとして引き受ける

namespace poac::opts::test {
    const clap::subcommand cli =
            clap::subcommand("test")
                .about("Run the tests")
                .arg(clap::opt("verbose", "Use verbose output").short_("v"))
                .arg(clap::opt("report", "Report the test"))
                .arg(clap::arg("<args>..."))
            ;

    struct Options {
        bool verbose;
        bool report;
        std::vector<std::string> program_args;
    };

//    [[nodiscard]] std::optional<core::except::Error>
//    execute_test_binary(const test::Options& opts, const std::string& bin_name, const std::string& bin_path) {
//        namespace fs = boost::filesystem;
//        using termcolor2::color_literals::operator""_green;
//
//        util::shell cmd(fs::relative(bin_path).string());
//        for (const auto& s : opts.program_args) {
//            cmd += s;
//        }
//        if (opts.report) {
//            fs::create_directories(io::path::current_build_test_report_dir);
//            cmd += ">";
//            cmd += (io::path::current_build_test_report_dir / bin_name).string() + ".xml";
//        }
//        // TODO: echo => Output .xml ...
//
//        std::cout << "Running: "_green
//                  << "`" + fs::relative(bin_path).string() + "`"
//                  << std::endl;
//        if (const auto ret = cmd.exec()) {
//            std::cout << ret.value();
//            return std::nullopt;
//        } else {
//            return core::except::Error::General{
//                fs::relative(bin_path).string(), " returned 1" // TODO: 1じゃない可能性がある
//            };
//        }
//    }

    [[nodiscard]] std::optional<core::except::Error>
    test(std::optional<io::config::Config>&&, test::Options&&) {
        namespace fs = boost::filesystem;
        using namespace termcolor2::color_literals;

//        const bool usemain = false;

        // {
        //   builder::core::Builder bs(fs::current_directory());
        //   bs.test(verbose); -> build system内で，testディレクトリを掘って，どんどんテストしていく．
        //   -> つまり，build sysytem使用側が，for文回すとかは無い．また，------------------------こういう区切り線も，test内で行われる．
        //   -> ただし，その区切り線は，もちろん，quiteがtrueだと表示しない．verboseがtrueだと情報を増やす．
        // }

//        core::Builder bs(opts.verbose);
//        bs.configure_compile(usemain);
//
//        /// TODO: これは重要！忘れない！！！！！！！！！！！！！！！！
//        // You can use #include<> in test code. // TODO: これは，builder.hpp: 255で書いたように，build.hppでもできるようにすべき．
//        bs.compile_conf.include_search_path.push_back((fs::current_path() / "include").string());
//
//        // TODO: buildsystemで，testモード実行なら，以下の内容が付与される．
//        // TODO: つまり，bs.build(), bs.test()
//        std::string static_link_lib;
//        if (const auto test_framework = config->test->framework) {
//            switch (test_framework.value()) {
//                case io::config::Config::Test::Framework::Boost:
//                    static_link_lib = "boost_unit_test_framework";
//                    break;
//                case io::config::Config::Test::Framework::Google:
//                    static_link_lib = "gtest"; // TODO: or "gtest_main"
//                    break;
//            }
//        } else {
//            return core::except::Error::General{
//                "Invalid test framework"
//            };
//        }
//
//        for (const fs::path& p : fs::recursive_directory_iterator(fs::current_path() / "test")) {
//            if (!fs::is_directory(p) && p.extension().string() == ".cpp") {
//                const std::string cpp_relative = fs::relative(p).string();
//                const std::string bin_name = fs::path(
//                        boost::replace_all_copy(
//                                fs::relative(cpp_relative, "test").string(), "/", "-")).stem().string();
//                const std::string extension = core::builder::absorb::binary_extension;
//                const std::string bin_path = (io::path::current_build_test_bin_dir / bin_name).string() + extension;
//
//                // TODO: こちらでハンドリングしようと思ったのは，ioへの依存を無くそうとしたから．
//                // TODO: 無くす必要は無い．それよりも美しく書く方が重要である．
//                bs.compile_conf.source_files = bs.hash_source_files({cpp_relative}, usemain);
//                if (bs.compile_conf.source_files.empty()) { // No need for compile and link
//                    std::cout << "Warning: "_yellow
//                              << "There is no change. Binary exists in `" +
//                                 fs::relative(bin_path).string() + "`."
//                              << std::endl;
////                    continue;
//                } else {
//                    if (const auto obj_files_path = bs.compile()) {
//                        bs.configure_link(*obj_files_path);
//                        bs.link_conf.project_name = bin_name;
//                        bs.link_conf.output_root = io::path::current_build_test_bin_dir;
//                        bs.link_conf.static_link_libs.push_back(static_link_lib);
//                        if (bs.link()) {
//                            std::cout << "Compiled: "_green
//                                      << "Output to `" +
//                                         fs::relative(bin_path).string() +
//                                         "`"
//                                      << std::endl;
//                        } else { // Link failure
//                            continue;
//                        }
//                    } else { // Compile failure
//                        continue;
//                    }
//                }
//
//                if (const auto error = execute_test_binary(opts, bin_name, bin_path)) {
//                    return error;
//                }
//                std::cout << "----------------------------------------------------------------" << std::endl;
//            }
//        }
        return std::nullopt;
    }

    [[nodiscard]] std::optional<core::except::Error>
    exec(std::optional<io::config::Config>&& config, std::vector<std::string>&& args) {
        test::Options opts{};
        opts.verbose = util::argparse::use(args, "-v", "--verbose");
        opts.report = util::argparse::use(args, "--report");
        // poac test -v -- -h build
        auto found = std::find(args.begin(), args.end(), "--");
        if (found != args.end()) {
            // -h build
            opts.program_args = std::vector<std::string>(found + 1, args.end());
        }
        return test::test(std::move(config), std::move(opts));
    }
} // end namespace
#endif // !POAC_OPTS_TEST_HPP

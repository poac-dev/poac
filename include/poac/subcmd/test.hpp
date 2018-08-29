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
#include "../util/compiler.hpp"
#include "./build.hpp"


namespace poac::subcmd { struct test {
        static const std::string summary() { return "Beta: Execute tests."; }
        static const std::string options() { return "-v | --verbose"; }

        template <typename VS, typename = std::enable_if_t<std::is_rvalue_reference_v<VS&&>>>
        void operator()(VS&& argv) { _main(std::move(argv)); }
        template <typename VS, typename = std::enable_if_t<std::is_rvalue_reference_v<VS&&>>>
        void _main(VS&& argv) {
            namespace fs     = boost::filesystem;
            namespace except = core::exception;

            check_arguments(argv);

            const bool verbose = (argv.size() > 0 && (argv[0] == "-v" || argv[0] == "--verbose"));
            const std::string project_name = io::file::yaml::get_node("name").as<std::string>();
            const auto project_path = (io::file::path::current_build_test_dir / project_name).string();
            // TODO: pathにnameを含んでいるのは汚い？？？もっと変数名を整える．

            // TODO: buildコマンドとの共通化
            util::compiler compiler;
            subcmd::build hoge{};
            hoge.configure(compiler, project_name);
            // TODO: if (test: framework: == boost) or google
            // TODO: それ以外 -> throw
            compiler.add_static_link_lib("boost_unit_test_framework");
            compiler.output_path = io::file::path::current_build_test_dir;
            // TODO: ここは，compilerが受け持っても良いかも
            fs::create_directories(io::file::path::current_build_test_dir);

            for (const fs::path& p : fs::recursive_directory_iterator(fs::current_path() / "test")) {
                if (!fs::is_directory(p) && p.extension().string() == ".cpp") {
                    const std::string bin_relative = fs::relative(p).string();
                    const std::string bin_name = fs::path(boost::replace_all_copy(fs::relative(bin_relative, "test").string(), "/", "-")).stem().string();
                    const std::string bin_path = (io::file::path::current_build_test_dir / bin_name).string();

                    compiler.project_name = bin_name;
                    compiler.main_cpp = bin_relative;

                    if (compiler.link(verbose)) {
                        std::cout << io::cli::green << "Compiled: " << io::cli::reset
                                  << "Output to `" + fs::relative(bin_path).string() + "`"
                                  << std::endl;

                        const std::string executable = fs::relative(io::file::path::current_build_test_dir / bin_name).string();
                        util::command cmd(executable);

                        // TODO: poac.yml -> test: args:
                        std::vector<std::string> program_args{};
                        for (const auto& s : program_args) {
                            cmd += s;
                        }
                        cmd.std_err();

                        std::cout << io::cli::green << "Running: " << io::cli::reset
                                  << "`" + executable + "`"
                                  << std::endl;
                        if (const auto ret = cmd.run())
                            std::cout << *ret;
                        else // TODO: errorの時も文字列が欲しい．
                            std::cout << project_name + " returned 1" << std::endl;
                    }
                    else { /* error */ // TODO: compileに失敗時も表示されてしまう．
                        std::cout << io::cli::yellow << "Warning: " << io::cli::reset
                                  << "There is no change. Binary exists in `" + fs::relative(bin_path).string() + "`."
                                  << std::endl;
                    }
                    // Do not use the previous object file
                    compiler.obj_files.clear();
                    std::cout << "----------------------------------------------------------------" << std::endl;
                }
            }
        }


        void check_arguments(const std::vector<std::string>& argv) {
            namespace except = core::exception;
            if (argv.size() >= 2)
                throw except::invalid_second_arg("test");
        }
    };} // end namespace
#endif // !POAC_SUBCMD_TEST_HPP

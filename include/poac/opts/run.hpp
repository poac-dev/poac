#ifndef POAC_OPTS_RUN_HPP
#define POAC_OPTS_RUN_HPP

#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

#include <poac/core/except.hpp>
#include <poac/util/argparse.hpp>
#include <poac/util/termcolor2.hpp>

namespace poac::opts::run {
    constexpr auto summary = termcolor2::make_string("Build project and exec it");
    constexpr auto options = termcolor2::make_string("[-v, --verbose | -- <program args>]");

    struct Options {
        bool verbose;
        std::vector<std::string> program_args;
    };

    [[nodiscard]] std::optional<core::except::Error>
    run([[maybe_unused]] run::Options&& opts) {
        return core::except::Error::General{
                "Sorry, you cannot use run command currently."
        };

        // builder::core::Builder bs(fs::current_directory());
        // bs.build(verbose); -> if EXIT_SUCCESS ->
        // return bs.build(verbose) && bs.run(verbose);

        // -v
//        build::check_arguments(std::vector<std::string>(args.begin(), result));
//        if (const auto result = build::exec(std::vector<std::string>{})) {
//            return result;
//        }

        // TODO: このexecutableなパスをもう一度取ってくるのが二度手間感がある．
        //  TODO: -> build systemに，runも付ける 31行目を参照 -> そうすれば，下のログ表示も，二分されないので，便利
//        const std::string project_name = io::yaml::load_config("name").as<std::string>();
//        const std::string bin_name = project_name + core::builder::absorb::binary_extension;
//        const fs::path executable_path = fs::relative(io::path::current_build_bin_dir / bin_name);
//        const std::string executable = executable_path.string();
//        util::shell cmd(executable);
//        for (const auto& s : opts.program_args) {
//            cmd += s;
//        }
//
//        std::cout << "Running: "_green
//                  << "`" + executable + "`"
//                  << std::endl;
//        if (const auto ret = cmd.exec()) {
//            std::cout << ret.value();
//        } else {
//            std::cout << project_name + " returned 1" << std::endl;
//        }
//
//        return std::nullopt;
    }

    [[nodiscard]] std::optional<core::except::Error>
    exec(std::optional<io::yaml::Config>&&, std::vector<std::string>&& args) {
        run::Options opts{};
        opts.verbose = util::argparse::use(args, "-v", "--verbose");
        // poac run -v -- -h build
        auto found = std::find(args.begin(), args.end(), "--");
        if (found != args.end()) {
            // -h build
            opts.program_args = std::vector<std::string>(found + 1, args.end());
        }
        return run::run(std::move(opts));
    }
} // end namespace
#endif // !POAC_OPTS_RUN_HPP

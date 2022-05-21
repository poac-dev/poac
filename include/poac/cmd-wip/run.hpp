#ifndef POAC_OPTS_RUN_HPP
#define POAC_OPTS_RUN_HPP

#include <algorithm>
#include <future>
#include <iostream>
#include <poac/core/except.hpp>
#include <string>
#include <utility> // std::move
#include <vector>

namespace poac::opts::run {
inline const clap::subcommand cli =
    clap::subcommand("run")
        .about("Build project and exec it")
        .arg(clap::opt("verbose", "Use verbose output").short_("v"))
        .arg(clap::arg("args").multiple(true));

struct Options {
  bool verbose;
  Vec<std::string> program_args;
};

[[nodiscard]] Option<core::except::Error>
run([[maybe_unused]] run::Options&& opts) {
  return core::except::Error::General{
      "Sorry, you cannot use run command currently."};

  // builder::core::Builder bs(fs::current_directory());
  // bs.build(verbose); -> if EXIT_SUCCESS ->
  // return bs.build(verbose) && bs.run(verbose);

  // -v
  //        build::check_arguments(Vec<std::string>(args.begin(), result));
  //        if (const auto result = build::exec(Vec<std::string>{})) {
  //            return result;
  //        }

  // TODO: このexecutableなパスをもう一度取ってくるのが二度手間感がある．
  //  TODO: -> build systemに，runも付ける 31行目を参照 ->
  //  そうすれば，下のログ表示も，二分されないので，便利
  //        const std::string project_name =
  //        io::yaml::load_config("name").as<std::string>(); const std::string
  //        bin_name = project_name + core::builder::absorb::binary_extension;
  //        const fs::path executable_path =
  //        fs::relative(io::path::current_build_bin_dir / bin_name); const
  //        std::string executable = executable_path.string(); util::shell
  //        cmd(executable); for (const auto& s : opts.program_args) {
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
  //        return None;
}

[[nodiscard]] Option<core::except::Error>
exec(std::future<Option<io::config::Config>>&&, Vec<std::string>&& args) {
  run::Options opts{};
  opts.verbose = util::argparse::use(args, "-v", "--verbose");
  // poac run -v -- -h build
  auto found = std::find(args.begin(), args.end(), "--");
  if (found != args.end()) {
    // -h build
    opts.program_args = Vec<std::string>(found + 1, args.end());
  }
  return run::run(std::move(opts));
}
} // namespace poac::opts::run
#endif // !POAC_OPTS_RUN_HPP

#include "Help.hpp"

#include "../Cli.hpp"
#include "../Rustify.hpp"

#include <span>

static int helpMain(std::span<const StringRef> args) noexcept;

const Subcmd HELP_CMD = // for better format
    Subcmd{ "help" }
        .setDesc("Displays help for a poac subcommand")
        .setArg(Arg{ "COMMAND" }.setRequired(false))
        .setMainFn(helpMain);

static int
helpMain(const std::span<const StringRef> args) noexcept {
  return getCmd().printHelp(args);
}

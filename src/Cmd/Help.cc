#include "Help.hpp"

#include "Global.hpp"

static const auto HELP_CLI =
    Subcmd("help").setDesc(helpDesc).setArg(Arg{ "[COMMAND]" }).finalize();

void
helpHelp() noexcept {
  HELP_CLI.printHelp();
}

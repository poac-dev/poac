#include "Help.hpp"

#include "Global.hpp"

static const auto HELP_CMD =
    Subcmd("help").setDesc(helpDesc).setArg(Arg{ "[COMMAND]" }).finalize();

void
helpHelp() noexcept {
  HELP_CMD.printHelp();
}

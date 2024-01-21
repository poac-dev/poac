#include "Help.hpp"

#include "Global.hpp"

static constexpr auto HELP_CLI =
    Subcmd<0>("help").setDesc(helpDesc).setArg(Arg{ "[COMMAND]" });

void
helpHelp() noexcept {
  HELP_CLI.printHelp();
}

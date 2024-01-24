#include "../Cli.hpp"
#include "Cmd.hpp"

// TODO: should be in Cli.cc?
const Subcmd helpCmd = Subcmd{ "help" }
                           .setDesc("Displays help for a poac subcommand")
                           .setArg(Arg{ "COMMAND" }.setRequired(false))
                           .setMainFn(helpMain);

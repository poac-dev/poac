#include "Help.hpp"

#include "../Cli.hpp"

const Subcmd helpCmd = Subcmd{ "help" }
                           .setDesc("Displays help for a poac subcommand")
                           .setArg(Arg{ "COMMAND" }.setRequired(false))
                           .setMainFn(helpMain);

#include "Help.hpp"

#include "Global.hpp"

const Subcmd helpCmd = Subcmd{ "help" }
                           .setDesc("Displays help for a poac subcommand")
                           .setArg(Arg{ "COMMAND" }.setRequired(false));

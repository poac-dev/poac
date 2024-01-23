#include "Help.hpp"

#include "Global.hpp"

const auto helpCmd = Subcmd{ "help" }
                         .setDesc("Displays help for a poac subcommand")
                         .setArg(Arg{ "[COMMAND]" });

#include "../Cli.hpp"

// TODO: should be in Cli.cc?
const Subcmd helpCmd = // for better format
    Subcmd{ "help" }
        .setDesc("Displays help for a poac subcommand")
        .setArg(Arg{ "COMMAND" }.setRequired(false))
        .setMainFn(helpMain);

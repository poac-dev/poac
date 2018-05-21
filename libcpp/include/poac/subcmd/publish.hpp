//
// Summary: Uninstall packages.
// Options: [<pkg-name>]
//
#ifndef POAC_SUBCMD_PUBLISH_HPP
#define POAC_SUBCMD_PUBLISH_HPP

#include <iostream>


namespace poac::subcmd { struct publish {
    static const std::string summary() { return "Uninstall packages."; }
    static const std::string options() { return "[<pkg-name>]"; }

    void operator()() { _publish(); }

    void _publish() {
        // Validate yaml, directory, ...
        // Ignore deps/
        // Compress to tar
        // Post to API
    }
};} // end namespace
#endif

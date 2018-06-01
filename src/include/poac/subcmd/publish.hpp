#ifndef POAC_SUBCMD_PUBLISH_HPP
#define POAC_SUBCMD_PUBLISH_HPP

#include <iostream>


namespace poac::subcmd { struct publish {
    static const std::string summary() { return "Publish a package."; }
    static const std::string options() { return "[<pkg-name>]"; }

    template <typename VS>
    void operator()(VS&& vs) { _main(vs); }
    template <typename VS>
    void _main([[maybe_unused]] VS&& vs) {
        // Validate yaml, directory, ...
        // Ignore deps/
        // Compress to tar
        // Post to API
    }
};} // end namespace
#endif // !POAC_SUBCMD_PUBLISH_HPP
#ifndef POAC_CMD_VERSION_HPP
#define POAC_CMD_VERSION_HPP

// std
#include <iostream>

// external
#include <mitama/result/result.hpp>

namespace poac::cmd::version {
    [[nodiscard]] mitama::result<void, std::string>
    exec() {
        std::cout << "poac " << POAC_VERSION << std::endl;
        return mitama::success();
    }
} // end namespace

#endif // !POAC_CMD_VERSION_HPP

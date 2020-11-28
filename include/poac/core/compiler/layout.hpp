#ifndef POAC_CORE_COMPILER_LAYOUT_HPP
#define POAC_CORE_COMPILER_LAYOUT_HPP

#include <string_view>

namespace poac::core::compiler {

bool
is_bad_artifact_name(std::string_view name) {
    for (const auto reserved : {"deps", "examples", "build", "incremental"}) {
        if (reserved == name) {
            return true;
        }
    }
    return false;
}

} // namespace poac::core::compiler

#endif // POAC_CORE_COMPILER_LAYOUT_HPP

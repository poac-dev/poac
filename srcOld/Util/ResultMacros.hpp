//
// NOTE: Do not include this unless you want to use Try.
//

#pragma once

// external
#include <mitama/result/result.hpp>

//
// Macros
//
// NOLINTNEXTLINE(readability-identifier-naming)
#define Try(...) MITAMA_TRY(__VA_ARGS__)

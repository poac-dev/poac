#pragma once

#include "Rustify.hpp"

#include <string>

usize numThreads() noexcept;
inline const std::string NUM_DEFAULT_THREADS = std::to_string(numThreads());

void setParallelism(usize numThreads) noexcept;
usize getParallelism() noexcept;
bool isParallel() noexcept;

#pragma once

#include "Rustify.hpp"

usize numThreads() noexcept;
inline const String NUM_DEFAULT_THREADS = std::to_string(numThreads());

void setParallelism(usize numThreads) noexcept;
usize getParallelism() noexcept;
bool isParallel() noexcept;

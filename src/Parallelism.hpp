#pragma once

#include <cstddef>
#include <string>

size_t numThreads() noexcept;
inline const std::string NUM_DEFAULT_THREADS = std::to_string(numThreads());

void setParallelism(size_t numThreads) noexcept;
size_t getParallelism() noexcept;
bool isParallel() noexcept;

#include "Parallel.hpp"

struct ParallelState {
  // ParallelState is a singleton
  ParallelState(const ParallelState&) = delete;
  ParallelState& operator=(const ParallelState&) = delete;
  ParallelState(ParallelState&&) noexcept = delete;
  ParallelState& operator=(ParallelState&&) noexcept = delete;
  ~ParallelState() noexcept = default;

  void set(const bool val) noexcept {
    isParallel = val;
  }
  bool get() const noexcept {
    return isParallel;
  }

  static ParallelState& instance() noexcept {
    static ParallelState INSTANCE;
    return INSTANCE;
  }

private:
  bool isParallel = true;

  ParallelState() noexcept = default;
};

void
setParallel(const bool isParallel) noexcept {
  ParallelState::instance().set(isParallel);
}

bool
isParallel() noexcept {
  return ParallelState::instance().get();
}

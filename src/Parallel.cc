#include "Parallel.hpp"

struct ParallelState {
  // ParallelState is a singleton
  ParallelState(const ParallelState&) = delete;
  ParallelState& operator=(const ParallelState&) = delete;
  ParallelState(ParallelState&&) noexcept = delete;
  ParallelState& operator=(ParallelState&&) noexcept = delete;
  ~ParallelState() noexcept = default;

  void set(const bool isParallel) noexcept {
    state = isParallel;
  }
  bool get() const noexcept {
    return state;
  }

  static ParallelState& instance() noexcept {
    static ParallelState instance;
    return instance;
  }

private:
  bool state = true;

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

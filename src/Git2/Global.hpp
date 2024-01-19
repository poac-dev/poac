#pragma once

namespace git2 {

struct GlobalState {
  GlobalState();
  ~GlobalState();

  GlobalState(const GlobalState&) = default;
  GlobalState& operator=(const GlobalState&) = default;
  GlobalState(GlobalState&&) = default;
  GlobalState& operator=(GlobalState&&) = default;
};

} // namespace git2

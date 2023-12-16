#include "Cmd/Build.hpp"

#include <cassert>
#include <sstream>
#include <stdexcept>

void test_cycle_vars() {
  BuildConfig config;
  config.defineVariable("a", "b", {"b"});
  config.defineVariable("b", "c", {"c"});
  config.defineVariable("c", "a", {"a"});
  try {
    std::stringstream ss;
    config.emitMakefile(ss);
  } catch (const std::runtime_error& e) {
    return;
  }

  assert(false && "should not reach here");
}

void test_simple_vars() {
  BuildConfig config;
  config.defineVariable("a", "b");
  config.defineVariable("b", "c");
  config.defineVariable("c", "d");
  std::stringstream ss;
  config.emitMakefile(ss);
  assert(ss.str() == "d = d\n"
                     "c = c\n"
                     "b = b\n"
                     "a = a\n\n");
}

int main() {
  test_cycle_vars();
  test_simple_vars();
}

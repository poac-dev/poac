#include "Test.hpp"

#include "../BuildConfig.hpp"

#include <iostream>

int test(Vec<String> args) {
  const String outDir = emitMakefile(args.empty() ? "" : args[0]);
  return std::system(("make -C " + outDir + " test").c_str());
}

void testHelp() {
  std::cout << testDesc << '\n';
  std::cout << '\n';
  std::cout << "Usage: poac test [OPTIONS]" << '\n';
  std::cout << '\n';
  std::cout << "Options:" << '\n';
  std::cout << "    -d, --debug\t\tTest with debug information (default)"
            << '\n';
  std::cout << "    -r, --release\tTest with optimizations" << '\n';
}

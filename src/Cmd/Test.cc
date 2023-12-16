#include "Test.hpp"

#include "../BuildConfig.hpp"

#include <iostream>

int test(Vec<String> args) {
  const String outDir = emitMakefile(args);
  return std::system(("make -C " + outDir + " test").c_str());
}

void testHelp() {
  std::cout << "poac-test" << '\n';
  std::cout << '\n';
  std::cout << "USAGE:" << '\n';
  std::cout << "    poac test [OPTIONS]" << '\n';
  std::cout << '\n';
  std::cout << "OPTIONS:" << '\n';
  std::cout << "    -d, --debug\t\tTest with debug information (default)"
            << '\n';
  std::cout << "    -r, --release\tTest with optimizations" << '\n';
}

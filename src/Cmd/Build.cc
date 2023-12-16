#include "Build.hpp"

#include "../BuildConfig.hpp"

#include <iostream>

int build(Vec<String> args) {
  const String outDir = emitMakefile(args);
  return std::system(("make -C " + outDir).c_str());
}

void buildHelp() {
  std::cout << buildDesc << '\n';
  std::cout << '\n';
  std::cout << "Usage: poac build [OPTIONS]" << '\n';
  std::cout << '\n';
  std::cout << "Options:" << '\n';
  std::cout << "    -d, --debug\t\tBuild with debug information (default)"
            << '\n';
  std::cout << "    -r, --release\tBuild with optimizations" << '\n';
}

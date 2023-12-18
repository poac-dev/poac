#include "Test.hpp"

#include "../BuildConfig.hpp"
#include "../Logger.hpp"

#include <iostream>

int testCmd(Vec<String> args) {
  const bool isDebug = isDebugMode(args.empty() ? "" : args[0]);
  if (!isDebug) {
    Logger::warn("The `--release` flag can disable assert macros.");
  }

  const String outDir = emitMakefile(isDebug);
  return std::system(("make -C " + outDir + " test").c_str());
}

void testHelp() {
  std::cout << testDesc << '\n';
  std::cout << '\n';
  std::cout << "Usage: poac test [OPTIONS]" << '\n';
  std::cout << '\n';
  std::cout << "Options:" << '\n';
  std::cout << "    -d, --debug\t\tTest with debug information [default]"
            << '\n';
  std::cout << "    -r, --release\tTest with optimizations" << '\n';
}

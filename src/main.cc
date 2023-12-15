#include "Rustify.hpp"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <queue>

struct Target {
  Vec<String> commands;
  Vec<String> dependsOn;
};

struct BuildConfig {
  Map<String, String> variables;
  HashMap<String, Vec<String>> varDeps;
  Map<String, Target> targets;
  HashMap<String, Vec<String>> targetDeps;

  void defineVariable(String name, String value, Vec<String> dependsOn = {}) {
    variables[name] = value;
    for (const auto& dep : dependsOn) {
      // reverse dependency
      varDeps[dep].push_back(name);
    }
  }

  void defineTarget(
      String name, const Vec<String>& commands, Vec<String> dependsOn = {}
  ) {
    targets[name] = {commands, dependsOn};
    for (const auto& dep : dependsOn) {
      // reverse dependency
      targetDeps[dep].push_back(name);
    }
  }

  template <typename T>
  Vec<String>
  topoSort(const Map<String, T>& list, HashMap<String, Vec<String>>& adjList) {
    HashMap<String, u32> inDegree;
    for (const auto& var : list) {
      inDegree[var.first] = 0;
    }
    for (const auto& edge : adjList) {
      if (inDegree.count(edge.first) == 0) {
        inDegree[edge.first] = 0;
      }
      for (const auto& neighbor : edge.second) {
        inDegree[neighbor]++;
      }
    }

    std::queue<String> zeroInDegree;
    for (const auto& var : inDegree) {
      if (var.second == 0) {
        zeroInDegree.push(var.first);
      }
    }

    Vec<String> res;
    while (!zeroInDegree.empty()) {
      const String node = zeroInDegree.front();
      zeroInDegree.pop();
      res.push_back(node);

      for (const String& neighbor : adjList[node]) {
        inDegree[neighbor]--;
        if (inDegree[neighbor] == 0) {
          zeroInDegree.push(neighbor);
        }
      }
    }

    if (res.size() != list.size()) {
      std::cerr << "Cycle detected" << '\n';
      exit(1);
    }
    return res;
  }

  void emit(std::ostream& os = std::cout) {
    Vec<String> sortedVars = topoSort(variables, varDeps);
    for (const auto& var : sortedVars) {
      os << var << " = " << variables[var] << '\n';
    }
    os << '\n';

    Vec<String> sortedTargets = topoSort(targets, targetDeps);

    for (auto itr = sortedTargets.rbegin(); itr != sortedTargets.rend();
         itr++) {
      os << *itr << ": ";
      for (const auto& dep : targets[*itr].dependsOn) {
        os << dep << " ";
      }
      os << '\n';

      for (const auto& cmd : targets[*itr].commands) {
        os << "\t" << cmd << '\n';
      }
      os << '\n';
    }
  }
};

int main() {
  if (!std::filesystem::exists("src/main.cc")) {
    std::cerr << "src/main.cc not found" << '\n';
    exit(1);
  }
  if (!std::filesystem::exists("poac-out")) {
    std::filesystem::create_directory("poac-out");
  }

  BuildConfig config;

  // Compiler settings
  config.defineVariable("CC", "clang++");
  config.defineVariable(
      "CFLAGS",
      "-Wall -Wextra -Werror -fdiagnostics-color -pedantic-errors -std=c++20"
  );
  config.defineVariable("LDFLAGS", "-L.");
  config.defineVariable("DEBUG_FLAGS", "-g -O0 -DDEBUG");
  config.defineVariable("RELEASE_FLAGS", "-O3 -DNDEBUG");
  // Archiver settings
  config.defineVariable("AR", "ar");
  config.defineVariable("ARFLAGS", "rcs");
  // Directories
  config.defineVariable("SRC_DIR", "../src");
  // Project settings
  config.defineVariable("PROJ_NAME", "poac");
  config.defineVariable("MAIN", "$(SRC_DIR)/main.cc", {"SRC_DIR"});
  config.defineVariable("MAIN_OBJ", "main.o");

  // Build rules
  config.defineTarget("all", {}, {"$(PROJ_NAME)"});
  config.defineTarget(
      "$(PROJ_NAME)", {"$(CC) $(CFLAGS) $^ -o $@"}, {"$(MAIN_OBJ)"}
  );
  config.defineTarget("$(MAIN_OBJ)", {"$(CC) $(CFLAGS) -c $(MAIN) -o $@"});

  std::ofstream ifs("poac-out/Makefile");
  config.emit(ifs);
  return 0;
}

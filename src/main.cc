#include "Build.hpp"
#include "Rustify.hpp"

#include <array>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <queue>
#include <sstream>
#include <stdexcept>
#include <string>

String exec(const char* cmd) {
  std::array<char, 128> buffer;
  String result;
  std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
  if (!pipe) {
    throw std::runtime_error("popen() failed!");
  }
  while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
    result += buffer.data();
  }
  return result;
}

struct Target {
  Vec<String> commands;
  Vec<String> dependsOn;
};

template <typename T>
Vec<String> topoSort(
    const Map<String, T>& list, const HashMap<String, Vec<String>>& adjList
) {
  HashMap<String, u32> inDegree;
  for (const auto& var : list) {
    inDegree[var.first] = 0;
  }
  for (const auto& edge : adjList) {
    if (list.count(edge.first) == 0) {
      continue; // Ignore nodes not in list
    }
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

    if (adjList.count(node) == 0) {
      // No dependencies
      continue;
    }
    for (const String& neighbor : adjList.at(node)) {
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

struct BuildConfig {
  Map<String, String> variables;
  HashMap<String, Vec<String>> varDeps;
  Map<String, Target> targets;
  HashMap<String, Vec<String>> targetDeps;

  void defineVariable(String name, String value, Vec<String> dependsOn = {}) {
    variables[name] = value;
    for (const String& dep : dependsOn) {
      // reverse dependency
      varDeps[dep].push_back(name);
    }
  }

  void defineTarget(
      String name, const Vec<String>& commands, Vec<String> dependsOn = {}
  ) {
    targets[name] = {commands, dependsOn};
    for (const String& dep : dependsOn) {
      // reverse dependency
      targetDeps[dep].push_back(name);
    }
  }

  void emitMakefile(std::ostream& os = std::cout) const {
    Vec<String> sortedVars = topoSort(variables, varDeps);
    for (const auto& var : sortedVars) {
      os << var << " = " << variables.at(var) << '\n';
    }
    os << '\n';

    Vec<String> sortedTargets = topoSort(targets, targetDeps);
    for (auto itr = sortedTargets.rbegin(); itr != sortedTargets.rend();
         itr++) {
      os << *itr << ": ";
      for (const auto& dep : targets.at(*itr).dependsOn) {
        os << dep << " ";
      }
      os << '\n';

      for (const auto& cmd : targets.at(*itr).commands) {
        os << "\t" << cmd << '\n';
      }
      os << '\n';
    }
  }
};

Vec<String> listSourceFiles(const String& directory) {
  Vec<String> sourceFiles;
  for (const auto& entry :
       std::filesystem::recursive_directory_iterator(directory)) {
    if (entry.path().extension() == ".cc") {
      sourceFiles.push_back(entry.path().string());
    }
  }
  return sourceFiles;
}

int main() {
  if (!std::filesystem::exists("src")) {
    std::cerr << "src directory not found" << '\n';
    exit(1);
  }
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
      "CFLAGS", "-Wall -Wextra -fdiagnostics-color -pedantic-errors -std=c++20"
  );
  config.defineVariable("LDFLAGS", "-L.");
  config.defineVariable("DEBUG_FLAGS", "-g -O0 -DDEBUG");
  config.defineVariable("RELEASE_FLAGS", "-O3 -DNDEBUG");
  // Directories
  config.defineVariable("SRC_DIR", "../src");
  // Project settings
  config.defineVariable("PROJ_NAME", "poac");
  config.defineVariable("MAIN", "$(SRC_DIR)/main.cc", {"SRC_DIR"});

  // Build rules
  config.defineTarget("all", {}, {"$(PROJ_NAME)"});

  Vec<String> sourceFiles = listSourceFiles("src");
  Vec<String> objectFiles;
  for (String sourceFile : sourceFiles) {
    sourceFile = "../" + sourceFile;
    String command = "cd src && clang++ -MM " + sourceFile;
    String output = exec(command.c_str());

    std::istringstream iss(output);
    String target;
    std::getline(iss, target, ':');
    objectFiles.push_back(target);
    std::cout << target << ':';

    Vec<String> dependencies;
    String dependency;
    while (std::getline(iss, dependency, ' ')) {
      if (!dependency.empty()) {
        // Remove trailing newline if it exists
        if (dependency.back() == '\n') {
          dependency.pop_back();
        }
        dependencies.push_back(dependency);
        std::cout << ' ' << dependency;
      }
    }
    std::cout << '\n';

    config.defineTarget(
        target, {"$(CC) $(CFLAGS) -c " + sourceFile + " -o $@"}, dependencies
    );
  }

  // The project binary
  config.defineTarget(
      "$(PROJ_NAME)", {"$(CC) $(CFLAGS) $^ -o $@"}, objectFiles
  );

  std::ofstream ifs("poac-out/Makefile");
  config.emitMakefile(ifs);
  return 0;
}

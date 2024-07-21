#include "Add.hpp"

#include "../Logger.hpp"
#include "../Manifest.hpp"
#include "../Rustify.hpp"
#include "Common.hpp"

#include <cstdlib>
#include <fstream>
#include <string>
#include <toml.hpp>

static int addMain(std::span<const StringRef> args);

const Subcmd ADD_CMD =
    Subcmd{ "add" }
        .setDesc("Add dependencies to poac.toml")
        .setArg(Arg{ "args" }
                    .setDesc("Dependencies to add")
                    .setRequired(true)
                    .setVariadic(true))
        .addOpt(Opt{ "--sys" }.setDesc("Use system dependency"))
        .addOpt(Opt{ "--version" }.setDesc(
            "Dependency version (Only used with system-dependencies)"
        ))
        .addOpt(
            Opt{ "--tag" }.setDesc("Specify a git tag").setPlaceholder("<TAG>")
        )
        .addOpt(Opt{ "--rev" }
                    .setDesc("Specify a git revision")
                    .setPlaceholder("<REVISION>"))
        .addOpt(Opt{ "--branch" }
                    .setDesc("Specify a branch of the git repository")
                    .setPlaceholder("<BRANCH_NAME>"))
        .setMainFn(addMain);

static Option<int>
handleNextArg(
    std::span<const StringRef>::iterator& itr,
    const std::span<const StringRef>::iterator& end, std::string& arg
) {
  ++itr;
  if (itr == end) {
    return Subcmd::missingArgumentForOpt(*--itr);
  }
  arg = std::string(*itr);
  return None;
}

static void
handleDependency(HashSet<StringRef>& newDeps, const StringRef dep) {
  if (newDeps.contains(dep)) {
    logger::warn("The dependency `", dep, "` is already in the poac.toml");
    return;
  }
  newDeps.insert(dep);
}

static std::string
getDependencyGitUrl(const StringRef dep) {
  if (dep.find("://") == std::string::npos) {
    // check if atleast in "user/repo" format
    if (dep.find('/') == std::string::npos) {
      logger::error("Invalid dependency: " + std::string(dep));
      return "";
    }

    return "https://github.com/" + std::string(dep) + ".git";
  }
  return std::string(dep);
}

static std::string
getDependencyName(const StringRef dep) {
  std::string name;
  if (dep.find("://") == std::string::npos) {
    name = dep.substr(dep.find_last_of('/') + 1);
  } else {
    name = dep.substr(
        dep.find_last_of('/') + 1, dep.find(".git") - dep.find_last_of('/') - 1
    );
  }

  // Remove trailing '.git' if it exists
  if (name.ends_with(".git")) {
    name = name.substr(0, name.size() - 4);
  }

  return name;
}

static int
addDependencyToManifest(
    const HashSet<StringRef>& newDeps, bool isSystemDependency,
    std::string& version, std::string& tag, std::string& rev,
    std::string& branch
) {
  toml::value depData = toml::table{};

  if (isSystemDependency) {
    if (version.empty()) {
      logger::error("The `--version` option is required for system dependencies"
      );
      return EXIT_FAILURE;
    }
    depData["version"] = version;
    depData["system"] = true;
  } else {
    if (!tag.empty()) {
      depData["tag"] = tag;
    }
    if (!rev.empty()) {
      depData["rev"] = rev;
    }
    if (!branch.empty()) {
      depData["branch"] = branch;
    }
  }

  auto data = toml::parse(getManifestPath());
  auto& deps = toml::find<toml::table>(data, "dependencies");

  for (const auto& dep : newDeps) {

    if (!isSystemDependency) {
      const std::string gitUrl = getDependencyGitUrl(dep);
      const std::string depName = getDependencyName(dep);

      if (gitUrl.empty() || depName.empty()) {
        return EXIT_FAILURE;
      }

      deps[depName] = depData;
      deps[depName]["git"] = gitUrl;
    } else {
      deps[std::string(dep)] = depData;
    }
  }

  std::ofstream ofs(getManifestPath());
  ofs << toml::format(data);

  logger::info("Added", "to the poac.toml");
  return EXIT_SUCCESS;
}

static int
addMain(const std::span<const StringRef> args) {
  if (args.empty()) {
    logger::error("No dependencies to add");
    return EXIT_FAILURE;
  }

  HashSet<StringRef> newDeps = {};

  bool isSystemDependency = false;
  std::string version; // Only used with system-dependencies

  std::string tag;
  std::string rev;
  std::string branch;

  // clang-format off
  HashMap<
    StringRef,
    Fn<Option<int>(decltype(args)::iterator&, decltype(args)::iterator)>
  >
  handlers = {
    {
      "--sys", [&](auto&, auto) {
        isSystemDependency = true;
        return None;
      }
    },
    {
      "--version", [&](auto& itr, const auto end) {
        return handleNextArg(itr, end, version);
      }
    },
    {
      "-v", [&](auto& itr, const auto end) {
        return handleNextArg(itr, end, version);
      }
    },
    {
      "--tag", [&](auto& itr, const auto end) {
        return handleNextArg(itr, end, tag);
      }
    },
    {
      "--rev", [&](auto& itr, const auto end) {
        return handleNextArg(itr, end, rev);
      }
    },
    {
      "--branch", [&](auto& itr, const auto end) {
        return handleNextArg(itr, end, branch);
      }
    },
  };
  // clang-format on

  for (auto itr = args.begin(); itr != args.end(); ++itr) {
    if (const auto res = Cli::handleGlobalOpts(itr, args.end(), "add")) {
      if (res.value() == Cli::CONTINUE) {
        continue;
      } else {
        return res.value();
      }
    } else {
      auto handler = handlers.find(*itr);
      if (handler != handlers.end()) {
        if (auto res = handler->second(itr, args.end()); res.has_value()) {
          return res.value();
        }
      } else {
        handleDependency(newDeps, *itr);
      }
    }
  }

  return addDependencyToManifest(
      newDeps, isSystemDependency, version, tag, rev, branch
  );
}

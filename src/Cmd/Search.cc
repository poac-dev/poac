#include "../Cli.hpp"
#include "../Logger.hpp"
#include "../Rustify.hpp"
#include "Cmd.hpp"

#include <cstdlib>
#include <curl/curl.h>
#include <iomanip>
#include <iostream>
#include <nlohmann/json.hpp>
#include <span>
#include <string>

static int searchMain(std::span<const StringRef> args);

const Subcmd SEARCH_CMD =
    Subcmd{ "search" }
        .setDesc("Search for packages in the registry")
        .addOpt(Opt{ "--per-page" }
                    .setDesc("Number of results to show per page")
                    .setPlaceholder("<NUM>")
                    .setDefault("10"))
        .addOpt(Opt{ "--page" }
                    .setDesc("Page number of results to show")
                    .setPlaceholder("<NUM>")
                    .setDefault("1"))
        .setArg(Arg{ "name" })
        .setMainFn(searchMain);

struct SearchArgs {
  String name;
  usize perPage = 10;
  usize page = 1;
};

static usize
writeCallback(void* contents, usize size, usize nmemb, String* userp) {
  userp->append(static_cast<char*>(contents), size * nmemb);
  return size * nmemb;
}

static nlohmann::json
searchPackages(const SearchArgs& args) {
  nlohmann::json req;
  req["query"] =
#include "../GraphQL/SearchPackages.gql"
      ;
  req["variables"]["name"] = "%" + args.name + "%";
  req["variables"]["limit"] = args.perPage;
  req["variables"]["offset"] = (args.page - 1) * args.perPage;

  const String reqStr = req.dump();
  String resStr;

  CURL* curl = curl_easy_init();
  if (!curl) {
    Logger::error("curl_easy_init() failed");
    return EXIT_FAILURE;
  }

  curl_easy_setopt(curl, CURLOPT_URL, "https://poac.hasura.app/v1/graphql");
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, &resStr);
  curl_easy_setopt(curl, CURLOPT_POST, 1L);
  curl_easy_setopt(curl, CURLOPT_POSTFIELDS, reqStr.c_str());
  curl_easy_perform(curl); // TODO: Handle CURLCode

  curl_easy_cleanup(curl);

  const nlohmann::json res = nlohmann::json::parse(resStr);
  const nlohmann::json packages = res["data"]["packages"];
  return packages;
}

static void
printTable(const nlohmann::json& packages) {
  constexpr int TABLE_WIDTH = 80;
  constexpr int NAME_WIDTH = 30;
  constexpr int VER_WIDTH = 10;

  std::cout << std::left << std::setw(NAME_WIDTH) << "Name"
            << std::setw(VER_WIDTH) << "Version"
            << "Description" << '\n';
  std::cout << String(TABLE_WIDTH, '-') << '\n';
  for (const auto& package : packages) {
    const String name = package["name"];
    const String version = package["version"];
    const String description = package["description"];

    std::cout << std::left << std::setw(NAME_WIDTH) << name
              << std::setw(VER_WIDTH) << version << description << '\n';
  }
}

static int
searchMain(const std::span<const StringRef> args) {
  SearchArgs searchArgs;
  for (usize i = 0; i < args.size(); ++i) {
    const StringRef arg = args[i];
    HANDLE_GLOBAL_OPTS({ { "search" } })

    else if (arg == "--per-page") {
      if (i + 1 < args.size()) {
        searchArgs.perPage = std::stoul(String(args[++i]));
      } else {
        Logger::error("missing argument for `--per-page`");
        return EXIT_FAILURE;
      }
    }
    else if (arg == "--page") {
      if (i + 1 < args.size()) {
        searchArgs.page = std::stoul(String(args[++i]));
      } else {
        Logger::error("missing argument for `--page`");
        return EXIT_FAILURE;
      }
    }
    else if (searchArgs.name.empty()) {
      searchArgs.name = arg;
    }
    else {
      return SEARCH_CMD.noSuchArg(arg);
    }
  }

  if (searchArgs.name.empty()) {
    Logger::error("missing package name");
    return EXIT_FAILURE;
  }

  const nlohmann::json packages = searchPackages(searchArgs);
  if (packages.empty()) {
    Logger::warn("no packages found");
    return EXIT_SUCCESS;
  }

  printTable(packages);
  return EXIT_SUCCESS;
}

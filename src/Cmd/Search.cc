#include "Search.hpp"

#include "../Rustify.hpp"
#include "Global.hpp"

#include <cstdlib>
#include <curl/curl.h>
#include <iomanip>
#include <iostream>
#include <nlohmann/json.hpp>
#include <span>
#include <string>

static usize
writeCallback(void* contents, usize size, usize nmemb, String* userp) {
  userp->append(static_cast<char*>(contents), size * nmemb);
  return size * nmemb;
}

int searchMain(const std::span<const StringRef> args) {
  // Parse args
  String packageName;
  usize perPage = 10;
  usize page = 1;
  for (usize i = 0; i < args.size(); ++i) {
    const StringRef arg = args[i];
    HANDLE_GLOBAL_OPTS({ { "search" } })

    else if (arg == "--per-page") {
      if (i + 1 < args.size()) {
        perPage = std::stoul(String(args[++i]));
      } else {
        Logger::error("missing argument for `--per-page`");
        return EXIT_FAILURE;
      }
    }
    else if (arg == "--page") {
      if (i + 1 < args.size()) {
        page = std::stoul(String(args[++i]));
      } else {
        Logger::error("missing argument for `--page`");
        return EXIT_FAILURE;
      }
    }
    else if (packageName.empty()) {
      packageName = arg;
    }
    else {
      Logger::error("too many arguments: ", arg);
      return EXIT_FAILURE;
    }
  }

  if (packageName.empty()) {
    Logger::error("missing package name");
    return EXIT_FAILURE;
  }

  nlohmann::json req;
  req["query"] =
#include "../GraphQL/SearchPackages.gql"
      ;
  req["variables"]["name"] = "%" + packageName + "%";
  req["variables"]["limit"] = perPage;
  req["variables"]["offset"] = (page - 1) * perPage;

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
  curl_easy_perform(curl);

  curl_easy_cleanup(curl);

  const nlohmann::json res = nlohmann::json::parse(resStr);
  const nlohmann::json packages = res["data"]["packages"];

  if (packages.empty()) {
    Logger::warn("no packages found");
    return EXIT_SUCCESS;
  }

  std::cout << std::left << std::setw(30) << "Name" << std::setw(10)
            << "Version" << std::setw(50) << "Description" << '\n';
  std::cout << String(80, '-') << '\n';
  for (const auto& package : packages) {
    const String name = package["name"];
    const String version = package["version"];
    const String description = package["description"];

    std::cout << std::left << std::setw(30) << name << std::setw(10) << version
              << std::setw(50) << description << '\n';
  }

  return EXIT_SUCCESS;
}

void searchHelp() noexcept {
  std::cout << searchDesc << '\n';
  std::cout << '\n';
  printUsage("search", "[OPTIONS] <name>");
  std::cout << '\n';
  printHeader("Options:");
  printGlobalOpts();
  printOption(
      "--per-page",
      "",
      "Number of results to show per page [default: 10]",
      "<NUM>"
  );
  printOption(
      "--page", "", "Page number of results to show [default: 1]", "<NUM>"
  );
  std::cout << '\n';
  printHeader("Arguments:");
  std::cout << "  <name>" << '\n';
}

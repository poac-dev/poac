#ifndef POAC_SOURCES_GITHUB_HPP
#define POAC_SOURCES_GITHUB_HPP

#include <iostream>
#include <vector>
#include <string>
#include <string_view>
#include <thread>
#include <chrono>
#include <cstdio>
#include <unistd.h>


namespace poac::sources::github {
    std::string resolve(const std::string& name, const std::string& tag) {
        // TODO: libcurl can not read redirect destination.
//        "https://github.com/curl/curl/archive/curl-7_61_0.tar.gz"
//    "https://github.com/boostorg/optional/archive/boost-1.67.0.tar.gz"
//    "https://github.com/opencv/opencv/archive/3.4.2.tar.gz"
//        "https://github.com/" + name + "/archive/" + tag + ".tar.gz"
//        -> redirect to
        return "https://codeload.github.com/" + name + "/tar.gz/" + tag;
    }
} // end namespace

#endif // !POAC_SOURCES_GITHUB_HPP

#ifndef POAC_SUBCMD_INSTALL_HPP
#define POAC_SUBCMD_INSTALL_HPP

#include <iostream>
#include <algorithm>
#include <iterator>
#include <vector>
#include <string>
#include <string_view>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <tuple>
#include <stdexcept>

#include <boost/filesystem.hpp>

#include "../utility/requests.hpp"


namespace poac::subcmd { struct install {
    static const std::string summary() { return "Install packages."; }
    static const std::string options() { return "[<pkg-names>]"; }

    template <typename VS>
    void operator()(VS&& vs) { _main(vs); }
    template <typename VS>
    void _main([[maybe_unused]] VS&& vs) {
        poac::utility::requests::get();
        // poac install hoge
        // 無かったら，hoge does not exists
        // If you want hage ?
        // みたいに最有力候補を出したい．なるべく一文で．理由は，複数のパッケージのダウンロード毎に複数行はうざいから．
    }
//namespace fs = boost::filesystem;
//
//std::tuple<std::string, std::string> analyze(std::string tmp) {
//    if (const std::size_t pos = tmp.find('@'); pos != std::string::npos)
//        return std::make_tuple(std::string(tmp, 0, pos), std::string(tmp, pos+1));
//    else
//        return std::make_tuple(tmp, std::string("newest"));
//}
//
//void _install(const std::string& input) {
//    const std::string endpoint = "https://re.poac.pm";
//    const auto [pkg_name, version] = analyze(input);
//    const std::string filename = version + ".tar.gz";
//    const std::string url  = endpoint + "/" + pkg_name + "/" + filename;
//    const std::string curl = "curl -fsSLO "+ url;
//    const std::string trap = "trap \"rm -f " + filename + "\" 0";
//    const std::string tar  = "tar xf " + filename + " -C ./deps";
//    if (!std::system((curl + " && " + trap + " && " + tar).c_str()))
//        std::cout << pkg_name << std::endl;
//}
//
//void exec_install(int argc, const char** s) {
//    std::vector<std::string> tmp(s+1, s+argc);
//    for (const std::string& n : tmp) _install(n);
//}
//
//void call_help(std::string_view argv) {
//    using namespace std::literals::string_literals;
//    std::system((poac::root::get() + "/libexec/poac---help "s + argv.data()).c_str());
//}
//
//void run([[maybe_unused]] std::vector<std::string>&& argv) {
////    if (argc > 1) {
////        if (!std::strcmp(argv[1], "--help") || !std::strcmp(argv[1], "-h")) call_help("");
////        else exec_install(argc, argv);
////    }
////    else {
////        std::cerr << "poac install: illegal option -- $@" << std::endl;
////        call_help("install");
////        std::exit(EXIT_FAILURE);
////    }
//}
};} // end namespace
#endif // !POAC_SUBCMD_INSTALL_HPP
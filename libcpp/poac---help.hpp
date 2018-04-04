//
// Summary: Display help for a command.
// Options: <Nothing>
//
#ifndef __POAC___HELP_HPP__
#define __POAC___HELP_HPP__

#include <iostream>

//struct path_leaf_string {
//    const std::string operator()(const boost::filesystem::directory_entry& entry) {
//        return entry.path().leaf().string();
//    }
//};
//void read_directory(const std::string& name, string_v& v) {
//    boost::filesystem::path p(name);
//    boost::filesystem::directory_iterator start(p);
//    boost::filesystem::directory_iterator end;
//    std::transform(start, end, std::back_inserter(v), path_leaf_string());
//}


namespace poac { namespace help {
    static constexpr int illegal = 1;
    void run([[maybe_unused]] std::vector<std::string>&& argv) {
        std::cout << "help" << std::endl;
    }
    void run() {

    }
}} // end help namespace // end poac namespace
#endif

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN
#include <boost/test/unit_test.hpp>
#include <boost/filesystem.hpp>

#include <string>
#include <vector>
#include <map>
#include <fstream>

#include <poac/io/config.hpp>
#include <poac/core/except.hpp>

// std::optional<T> get(const YAML::Node& node)
BOOST_AUTO_TEST_CASE( poac_io_config_get_test )
{
//    using poac::io::config::detail::get;
//
//    // std::optional<T> get(const YAML::Node& node)
//    {
//        YAML::Node node = YAML::Load("bar");
//        BOOST_CHECK( get<std::string>(node).value() == "bar" );
//    }

//    const auto c = config.get();
//
////        std::cout << "fds" << std::endl;
////        for (const auto& op : hoge.options.value()) {
////            std::cout << op << std::endl;
////        }
////        std::cout << "fds" << std::endl;
////
//    std::cout << "dev:" << std::endl;
//    for (const auto& op : c->profile->dev->options.value()) {
//        std::cout << op << std::endl;
//    }
//    std::cout << "bench:" << std::endl;
//    for (const auto& op : c->profile->bench->options.value()) {
//        std::cout << op << std::endl;
//    }
////        std::cout << std::endl <<  c->profile->dev.compiler.value() << std::endl;
}

#define BOOST_TEST_MAIN
#include <boost/test/included/unit_test.hpp>
#include <fstream>
#include <poac/io/path.hpp>

// bool copy_recursive(const boost::filesystem::path& from, const boost::filesystem::path& dest)
BOOST_AUTO_TEST_CASE( poac_io_filesystem_copy_recursive_test )
{
    using poac::io::path::copy_recursive;

    const std::filesystem::path test_from_dir = "test_from_dir";
    std::filesystem::create_directory(test_from_dir);
    for (int i = 0; i < 3; ++i) {
        std::ofstream((test_from_dir / "test_file").string() + std::to_string(i));
    }

    const std::filesystem::path test_dest_dir = "test_dest_dir";
    BOOST_CHECK( copy_recursive(test_from_dir, test_dest_dir) );
    for (int i = 0; i < 3; ++i) {
        std::filesystem::exists((test_dest_dir / "test_file").string() + std::to_string(i));
    }

    std::filesystem::remove_all(test_from_dir);
    std::filesystem::remove_all(test_dest_dir);
}

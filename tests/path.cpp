#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN
#include <boost/test/unit_test.hpp>
#include <fstream>
#include <poac/io/filesystem.hpp>

// bool validate_dir(const boost::filesystem::path& path)
BOOST_AUTO_TEST_CASE( poac_io_filesystem_validate_dir_test )
{
    using poac::io::filesystem::validate_dir;

    const poac::io::filesystem::path test_dir = "test_dir";

    BOOST_CHECK( !validate_dir(test_dir) );

    poac::io::filesystem::create_directory(test_dir);
    BOOST_CHECK( !validate_dir(test_dir) );

    std::ofstream((test_dir / "test_file").string());
    BOOST_CHECK( validate_dir(test_dir) );

    poac::io::filesystem::remove_all(test_dir);
}

// bool copy_recursive(const boost::filesystem::path& from, const boost::filesystem::path& dest)
BOOST_AUTO_TEST_CASE( poac_io_filesystem_copy_recursive_test )
{
    using poac::io::filesystem::copy_recursive;
    using poac::io::filesystem::validate_dir;

    const poac::io::filesystem::path test_from_dir = "test_from_dir";
    poac::io::filesystem::create_directory(test_from_dir);
    for (int i = 0; i < 3; ++i) {
        std::ofstream((test_from_dir / "test_file").string() + std::to_string(i));
    }

    const poac::io::filesystem::path test_dest_dir = "test_dest_dir";
    BOOST_CHECK( copy_recursive(test_from_dir, test_dest_dir) );
    BOOST_CHECK( validate_dir(test_dest_dir) );
    for (int i = 0; i < 3; ++i) {
        poac::io::filesystem::exists((test_dest_dir / "test_file").string() + std::to_string(i));
    }

    poac::io::filesystem::remove_all(test_from_dir);
    poac::io::filesystem::remove_all(test_dest_dir);
}

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN
#include <boost/test/unit_test.hpp>
#include <boost/filesystem.hpp>

#include <fstream>

#include <poac/io/path.hpp>

// bool validate_dir(const boost::filesystem::path& path)
BOOST_AUTO_TEST_CASE( poac_io_path_validate_dir_test )
{
    using poac::io::path::validate_dir;

    const boost::filesystem::path test_dir = "test_dir";

    BOOST_CHECK( !validate_dir(test_dir) );

    boost::filesystem::create_directory(test_dir);
    BOOST_CHECK( !validate_dir(test_dir) );

    std::ofstream((test_dir / "test_file").string());
    BOOST_CHECK( validate_dir(test_dir) );

    boost::filesystem::remove_all(test_dir);
}

// bool recursive_copy(const boost::filesystem::path& from, const boost::filesystem::path& dest)
BOOST_AUTO_TEST_CASE( poac_io_path_recursive_copy_test )
{
    using poac::io::path::recursive_copy;
    using poac::io::path::validate_dir;

    const boost::filesystem::path test_from_dir = "test_from_dir";
    boost::filesystem::create_directory(test_from_dir);
    for (int i = 0; i < 3; ++i) {
        std::ofstream((test_from_dir / "test_file").string() + std::to_string(i));
    }

    const boost::filesystem::path test_dest_dir = "test_dest_dir";
    BOOST_CHECK( recursive_copy(test_from_dir, test_dest_dir) );
    BOOST_CHECK( validate_dir(test_dest_dir) );
    for (int i = 0; i < 3; ++i) {
        boost::filesystem::exists((test_dest_dir / "test_file").string() + std::to_string(i));
    }

    boost::filesystem::remove_all(test_from_dir);
    boost::filesystem::remove_all(test_dest_dir);
}

// void write_to_file(std::ofstream& ofs, const std::string& fname, const std::string& text)
BOOST_AUTO_TEST_CASE( poac_io_path_write_to_file_test )
{
    using poac::io::path::write_to_file;

    std::ofstream ofs;
    write_to_file(ofs, "test_file", "test");
    boost::filesystem::exists("test_file");
    write_to_file(ofs, "test_file2", "test");
    boost::filesystem::exists("test_file2");

    boost::filesystem::remove("test_file");
    boost::filesystem::remove("test_file2");
}

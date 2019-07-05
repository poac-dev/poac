#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN
#include <boost/test/unit_test.hpp>
#include <boost/test/output_test_stream.hpp>

#include <cstddef>
#include <vector>
#include <string>

#include <poac/io/term.hpp>

// std::string to_spinner(const size_t& index)
BOOST_AUTO_TEST_CASE( poac_io_term_to_spinner_test )
{
    using poac::io::term::to_spinner;
    using poac::io::term::spinners;

    for (int i = 0; i < 100; ++i) {
        for (int j = 0; j < spinners.size(); ++j) {
            BOOST_CHECK( spinners[j] == to_spinner(j + (i * spinners.size())) );
        }
    }
}

// std::string to_pointer(const size_t& index)
BOOST_AUTO_TEST_CASE( poac_io_term_to_pointer_test )
{
    using poac::io::term::to_pointer;
    using poac::io::term::pointers;

    constexpr std::size_t num = 10;
    std::vector<std::string> result(num);
    for (int i = 0; i < num; ++i) {
        result[i] = to_pointer(i);
    }

    std::vector<std::string> answer = {
            pointers[0], pointers[0], pointers[0],
            pointers[1], pointers[1], pointers[1],
            pointers[2], pointers[2], pointers[2], pointers[2]
    };

    BOOST_CHECK( result == answer );
}

// std::string to_progress(const int& max_count, int now_count)
BOOST_AUTO_TEST_CASE( poac_io_term_to_progress_test )
{
    using poac::io::term::to_progress;

    BOOST_CHECK( to_progress(50, 0) ==   "[                                                  ]" );
    BOOST_CHECK( to_progress(50, 1) ==   "[>                                                 ]" );
    BOOST_CHECK( to_progress(50, 10) ==  "[=========>                                        ]" );
    BOOST_CHECK( to_progress(50, 20) ==  "[===================>                              ]" );
    BOOST_CHECK( to_progress(50, 30) ==  "[=============================>                    ]" );
    BOOST_CHECK( to_progress(50, 40) ==  "[=======================================>          ]" );
    BOOST_CHECK( to_progress(50, 50) ==  "[=================================================>]" );
    BOOST_CHECK( to_progress(50, 100) == "[=================================================>]" );

    BOOST_CHECK( to_progress(100, 0) ==   "[                                                  ]" );
    BOOST_CHECK( to_progress(100, 1) ==   "[                                                  ]" );
    BOOST_CHECK( to_progress(100, 2) ==   "[>                                                 ]" );
    BOOST_CHECK( to_progress(100, 3) ==   "[>                                                 ]" );
    BOOST_CHECK( to_progress(100, 4) ==   "[=>                                                ]" );
    BOOST_CHECK( to_progress(100, 10) ==  "[====>                                             ]" );
    BOOST_CHECK( to_progress(100, 20) ==  "[=========>                                        ]" );
    BOOST_CHECK( to_progress(100, 30) ==  "[==============>                                   ]" );
    BOOST_CHECK( to_progress(100, 40) ==  "[===================>                              ]" );
    BOOST_CHECK( to_progress(100, 50) ==  "[========================>                         ]" );
    BOOST_CHECK( to_progress(100, 100) == "[=================================================>]" );
}

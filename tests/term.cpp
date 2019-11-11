#define BOOST_TEST_MAIN
#include <boost/test/included/unit_test.hpp>
#include <boost/test/output_test_stream.hpp>

#include <cstddef>
#include <vector>
#include <string>

#include <poac/io/term.hpp>

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

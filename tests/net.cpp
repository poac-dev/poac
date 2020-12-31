#define BOOST_TEST_MAIN
#include <boost/test/included/unit_test.hpp>

#include <poac/util/net.hpp>

// std::string to_progress(const int& max_count, int now_count)
BOOST_AUTO_TEST_CASE( poac_util_net_to_progress_test )
{
    using poac::util::net::to_progress;

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

BOOST_AUTO_TEST_CASE( poac_io_util_to_byte_progress_test )
{
    using poac::util::net::to_byte_progress;

    BOOST_CHECK( to_byte_progress(50, 0) ==   "[                                                  ] 0.00B/50.00B" );
    BOOST_CHECK( to_byte_progress(50, 1) ==   "[>                                                 ] 1.00B/50.00B" );
    BOOST_CHECK( to_byte_progress(50, 10) ==  "[=========>                                        ] 10.00B/50.00B" );
    BOOST_CHECK( to_byte_progress(50, 20) ==  "[===================>                              ] 20.00B/50.00B" );
    BOOST_CHECK( to_byte_progress(50, 30) ==  "[=============================>                    ] 30.00B/50.00B" );
    BOOST_CHECK( to_byte_progress(50, 40) ==  "[=======================================>          ] 40.00B/50.00B" );
    BOOST_CHECK( to_byte_progress(50, 50) ==  "[=================================================>] 50.00B/50.00B" );
    BOOST_CHECK( to_byte_progress(50, 100) == "[=================================================>] 50.00B/50.00B" );

    BOOST_CHECK( to_byte_progress(100, 0) ==   "[                                                  ] 0.00B/100.00B" );
    BOOST_CHECK( to_byte_progress(100, 1) ==   "[                                                  ] 1.00B/100.00B" );
    BOOST_CHECK( to_byte_progress(100, 2) ==   "[>                                                 ] 2.00B/100.00B" );
    BOOST_CHECK( to_byte_progress(100, 3) ==   "[>                                                 ] 3.00B/100.00B" );
    BOOST_CHECK( to_byte_progress(100, 4) ==   "[=>                                                ] 4.00B/100.00B" );
    BOOST_CHECK( to_byte_progress(100, 10) ==  "[====>                                             ] 10.00B/100.00B" );
    BOOST_CHECK( to_byte_progress(100, 20) ==  "[=========>                                        ] 20.00B/100.00B" );
    BOOST_CHECK( to_byte_progress(100, 30) ==  "[==============>                                   ] 30.00B/100.00B" );
    BOOST_CHECK( to_byte_progress(100, 40) ==  "[===================>                              ] 40.00B/100.00B" );
    BOOST_CHECK( to_byte_progress(100, 50) ==  "[========================>                         ] 50.00B/100.00B" );
    BOOST_CHECK( to_byte_progress(100, 100) == "[=================================================>] 100.00B/100.00B" );
}

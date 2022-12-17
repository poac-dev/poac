#include <boost/ut.hpp>
#include <poac/util/net.hpp>

auto main() -> int {
  using namespace std::literals::string_literals;
  using namespace boost::ut;

  // std::string to_progress(const int& max_count, int now_count)
  "test to_progress"_test = [] {
    using poac::util::net::to_progress;

    expect(
        eq(to_progress(50, 0),
           "[                                                  ]"s)
    );
    expect(
        eq(to_progress(50, 1),
           "[>                                                 ]"s)
    );
    expect(
        eq(to_progress(50, 10),
           "[=========>                                        ]"s)
    );
    expect(
        eq(to_progress(50, 20),
           "[===================>                              ]"s)
    );
    expect(
        eq(to_progress(50, 30),
           "[=============================>                    ]"s)
    );
    expect(
        eq(to_progress(50, 40),
           "[=======================================>          ]"s)
    );
    expect(
        eq(to_progress(50, 50),
           "[=================================================>]"s)
    );
    expect(
        eq(to_progress(50, 100),
           "[=================================================>]"s)
    );

    expect(
        eq(to_progress(100, 0),
           "[                                                  ]"s)
    );
    expect(
        eq(to_progress(100, 1),
           "[                                                  ]"s)
    );
    expect(
        eq(to_progress(100, 2),
           "[>                                                 ]"s)
    );
    expect(
        eq(to_progress(100, 3),
           "[>                                                 ]"s)
    );
    expect(
        eq(to_progress(100, 4),
           "[=>                                                ]"s)
    );
    expect(
        eq(to_progress(100, 10),
           "[====>                                             ]"s)
    );
    expect(
        eq(to_progress(100, 20),
           "[=========>                                        ]"s)
    );
    expect(
        eq(to_progress(100, 30),
           "[==============>                                   ]"s)
    );
    expect(
        eq(to_progress(100, 40),
           "[===================>                              ]"s)
    );
    expect(
        eq(to_progress(100, 50),
           "[========================>                         ]"s)
    );
    expect(
        eq(to_progress(100, 100),
           "[=================================================>]"s)
    );
  };

  "test to_byte_progress"_test = [] {
    using poac::util::net::to_byte_progress;

    expect(
        eq(to_byte_progress(50, 0),
           "[                                                  ] 0.00B/50.00B"s)
    );
    expect(
        eq(to_byte_progress(50, 1),
           "[>                                                 ] 1.00B/50.00B"s)
    );
    expect(eq(
        to_byte_progress(50, 10),
        "[=========>                                        ] 10.00B/50.00B"s
    ));
    expect(eq(
        to_byte_progress(50, 20),
        "[===================>                              ] 20.00B/50.00B"s
    ));
    expect(eq(
        to_byte_progress(50, 30),
        "[=============================>                    ] 30.00B/50.00B"s
    ));
    expect(eq(
        to_byte_progress(50, 40),
        "[=======================================>          ] 40.00B/50.00B"s
    ));
    expect(eq(
        to_byte_progress(50, 50),
        "[=================================================>] 50.00B/50.00B"s
    ));
    expect(eq(
        to_byte_progress(50, 100),
        "[=================================================>] 50.00B/50.00B"s
    ));

    expect(eq(
        to_byte_progress(100, 0),
        "[                                                  ] 0.00B/100.00B"s
    ));
    expect(eq(
        to_byte_progress(100, 1),
        "[                                                  ] 1.00B/100.00B"s
    ));
    expect(eq(
        to_byte_progress(100, 2),
        "[>                                                 ] 2.00B/100.00B"s
    ));
    expect(eq(
        to_byte_progress(100, 3),
        "[>                                                 ] 3.00B/100.00B"s
    ));
    expect(eq(
        to_byte_progress(100, 4),
        "[=>                                                ] 4.00B/100.00B"s
    ));
    expect(eq(
        to_byte_progress(100, 10),
        "[====>                                             ] 10.00B/100.00B"s
    ));
    expect(eq(
        to_byte_progress(100, 20),
        "[=========>                                        ] 20.00B/100.00B"s
    ));
    expect(eq(
        to_byte_progress(100, 30),
        "[==============>                                   ] 30.00B/100.00B"s
    ));
    expect(eq(
        to_byte_progress(100, 40),
        "[===================>                              ] 40.00B/100.00B"s
    ));
    expect(eq(
        to_byte_progress(100, 50),
        "[========================>                         ] 50.00B/100.00B"s
    ));
    expect(eq(
        to_byte_progress(100, 100),
        "[=================================================>] 100.00B/100.00B"s
    ));
  };
}

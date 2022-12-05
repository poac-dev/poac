// std
#include <fstream>

// external
#include <boost/ut.hpp>

// internal
#include <poac/util/sha256.hpp>

auto main() -> int {
  using namespace std::literals::string_literals;
  using namespace poac;
  using namespace boost::ut;

  "test sha256::sum"_test = [] {
    using util::sha256::sum;

    fs::path const out_path = "test.txt";
    std::ofstream out(out_path);
    out << "Hello, Poac!";
    out.close();

    const auto result = sum(out_path);
    expect(result.is_ok());
    expect(
        eq(result.unwrap(),
           "d6c7708eeb83d2ac74075d77cdb233bbc982d8f7d88a62da7764e00d46e683bf"s)
    );

    fs::remove(out_path);
  };
}

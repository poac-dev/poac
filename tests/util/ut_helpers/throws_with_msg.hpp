#ifndef TESTS_UTIL_UT_HELPERS_THROWS_WITH_MSG_HPP_
#define TESTS_UTIL_UT_HELPERS_THROWS_WITH_MSG_HPP_

// std
#include <string>

// external
#include <boost/ut.hpp>

namespace poac::test::ut_helpers {

#if defined(__cpp_exceptions)
template <class TException, class TExpr>
constexpr void
throws_with_msg(const TExpr& expr, const std::string& msg) {
  using namespace boost::ut;

  expect(throws<TException>(expr));
  expect(eq(msg, [&expr] {
    try {
      expr();
    } catch (const TException& e) {
      return std::string(e.what());
    }
    return std::string();
  }()));
}
#endif

} // namespace poac::test::ut_helpers

#endif // TESTS_UTIL_UT_HELPERS_THROWS_WITH_MSG_HPP_

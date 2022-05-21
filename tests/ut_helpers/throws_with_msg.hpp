#ifndef TESTS_UT_HELPERS_THROWS_WITH_MSG_HPP_
#define TESTS_UT_HELPERS_THROWS_WITH_MSG_HPP_

// std
#include <string>

// external
// clang-format off
// to avoid reporting errors with inline namespace on only the dry-run mode. (IDK why)
#include <boost/ut.hpp>
// clang-format on

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

#endif // TESTS_UT_HELPERS_THROWS_WITH_MSG_HPP_

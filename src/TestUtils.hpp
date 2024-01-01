#pragma once

#include "TermColor.hpp"

#include <cstdlib>
#include <iostream>
#include <optional>
#include <ostream>
#include <string>

#define TEST_PREFIX(name) "test " << __FILE__ << "::" << #name << " ... "
#define TEST_PREFIX2 "test " << __FILE__ << "::" << __func__ << " ... "

#define REGISTER_TEST(name) \
  name();                   \
  std::cout << TEST_PREFIX(name) << green("ok") << std::endl

#define TEST_ERROR                                     \
  std::cerr << TEST_PREFIX2 << red("FAILED") << "\n\n" \
            << __FILE__ << "::" << __func__ << ':' << __LINE__ << ": "

#define ASSERT_TRUE(cond)             \
  if (!(cond)) {                      \
    TEST_ERROR << #cond << std::endl; \
    std::exit(EXIT_FAILURE);          \
  }

#define ASSERT_FALSE(cond) ASSERT_TRUE(!(cond))

#define ASSERT_EQ(lhs, rhs)                              \
  if ((lhs) != (rhs)) {                                  \
    TEST_ERROR << (lhs) << " == " << (rhs) << std::endl; \
    std::exit(EXIT_FAILURE);                             \
  }

#define ASSERT_NE(lhs, rhs)                              \
  if ((lhs) == (rhs)) {                                  \
    TEST_ERROR << (lhs) << " != " << (rhs) << std::endl; \
    std::exit(EXIT_FAILURE);                             \
  }

#define ASSERT_LT(lhs, rhs)                             \
  if ((lhs) >= (rhs)) {                                 \
    TEST_ERROR << (lhs) << " < " << (rhs) << std::endl; \
    std::exit(EXIT_FAILURE);                            \
  }

#define ASSERT_EXCEPTION(statements, exception, msg)                       \
  try {                                                                    \
    statements;                                                            \
    TEST_ERROR << "expected exception `" << #exception                     \
               << "` not "                                                 \
                  "thrown"                                                 \
               << std::endl;                                               \
    std::exit(EXIT_FAILURE);                                               \
  } catch (const exception& e) {                                           \
    if (e.what() != std::string(msg)) {                                    \
      TEST_ERROR << "expected exception message `" << msg << "` but got `" \
                 << e.what() << "`" << std::endl;                          \
      std::exit(EXIT_FAILURE);                                             \
    }                                                                      \
  } catch (...) {                                                          \
    TEST_ERROR << "expected exception `" << #exception                     \
               << "` but got another exception" << std::endl;              \
    std::exit(EXIT_FAILURE);                                               \
  }

template <typename T>
std::ostream& operator<<(std::ostream& os, const std::optional<T>& opt) {
  if (opt.has_value()) {
    os << opt.value();
  } else {
    os << "None";
  }
  return os;
}

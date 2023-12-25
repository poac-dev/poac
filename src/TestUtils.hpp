#pragma once

#include "TermColor.hpp"

#include <cstdlib>
#include <iostream>
#include <optional>
#include <ostream>
#include <string>

#define ASSERT_TRUE(cond)                                                     \
  if (!(cond)) {                                                              \
    std::cerr << bold(red("FAIL: ")) << __FILE__ << ":" << __LINE__ << ": "   \
              << #cond << std::endl;                                          \
    std::exit(EXIT_FAILURE);                                                  \
  } else {                                                                    \
    std::cout << bold(green("PASS: ")) << __FILE__ << ":" << __LINE__ << ": " \
              << #cond << std::endl;                                          \
  }

#define ASSERT_FALSE(cond) ASSERT_TRUE(!(cond))

#define ASSERT_EQ(lhs, rhs)                                                   \
  if ((lhs) != (rhs)) {                                                       \
    std::cerr << bold(red("FAIL: ")) << __FILE__ << ":" << __LINE__ << ": "   \
              << (lhs) << " == " << (rhs) << std::endl;                       \
    std::exit(EXIT_FAILURE);                                                  \
  } else {                                                                    \
    std::cout << bold(green("PASS: ")) << __FILE__ << ":" << __LINE__ << ": " \
              << #lhs << " == " << #rhs << std::endl;                         \
  }

#define ASSERT_NE(lhs, rhs)                                                   \
  if ((lhs) == (rhs)) {                                                       \
    std::cerr << bold(red("FAIL: ")) << __FILE__ << ":" << __LINE__ << ": "   \
              << (lhs) << " != " << (rhs) << std::endl;                       \
    std::exit(EXIT_FAILURE);                                                  \
  } else {                                                                    \
    std::cout << bold(green("PASS: ")) << __FILE__ << ":" << __LINE__ << ": " \
              << #lhs << " != " << #rhs << std::endl;                         \
  }

#define ASSERT_LT(lhs, rhs)                                                   \
  if ((lhs) >= (rhs)) {                                                       \
    std::cerr << bold(red("FAIL: ")) << __FILE__ << ":" << __LINE__ << ": "   \
              << (lhs) << " < " << (rhs) << std::endl;                        \
    std::exit(EXIT_FAILURE);                                                  \
  } else {                                                                    \
    std::cout << bold(green("PASS: ")) << __FILE__ << ":" << __LINE__ << ": " \
              << #lhs << " < " << #rhs << std::endl;                          \
  }

#define ASSERT_EXCEPTION(statements, exception, msg)                          \
  try {                                                                       \
    statements;                                                               \
    std::cerr << bold(red("FAIL: ")) << __FILE__ << ":" << __LINE__ << ": "   \
              << "expected exception `" << #exception << "` not thrown"       \
              << std::endl;                                                   \
    std::exit(EXIT_FAILURE);                                                  \
  } catch (const exception& e) {                                              \
    if (e.what() != std::string(msg)) {                                       \
      std::cerr << bold(red("FAIL: ")) << __FILE__ << ":" << __LINE__ << ": " \
                << "expected exception message `" << msg << "` but got `"     \
                << e.what() << "`" << std::endl;                              \
      std::exit(EXIT_FAILURE);                                                \
    } else {                                                                  \
      std::cout << bold(green("PASS: ")) << __FILE__ << ":" << __LINE__       \
                << ": " << #statements << std::endl;                          \
    }                                                                         \
  } catch (...) {                                                             \
    std::cerr << bold(red("FAIL: ")) << __FILE__ << ":" << __LINE__ << ": "   \
              << "expected exception `" << #exception << "` thrown"           \
              << std::endl;                                                   \
    std::exit(EXIT_FAILURE);                                                  \
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

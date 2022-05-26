#ifndef POAC_UTIL_TERMCOLOR2_CONFIG_HPP_
#define POAC_UTIL_TERMCOLOR2_CONFIG_HPP_

#ifdef __cplusplus
#  ifndef TERMCOLOR2_STD_VER
#    if __cplusplus <= 201703L
#      error \
          "termcolor2 requires a compiler that (fully) implements C++20 or later."
#    else
#      define TERMCOLOR2_STD_VER 20
#    endif
#  endif // !TERMCOLOR2_STD_VER
#else
#  error "__cplusplus is not defined"
#endif // !__cplusplus

//#if defined(__cpp_lib_constexpr_dynamic_alloc) && !defined(__clang__) &&
// TERMCOLOR2_STD_VER > 17
#if false // needs support of constexpr std::string
#  define TERMCOLOR2_CXX20_CONSTEVAL consteval
#  define TERMCOLOR2_CXX20_CONSTEVAL_FN consteval
#  define TERMCOLOR2_CXX20_CONSTINIT constinit
#else
#  define TERMCOLOR2_CXX20_CONSTEVAL
#  define TERMCOLOR2_CXX20_CONSTEVAL_FN constexpr
#  define TERMCOLOR2_CXX20_CONSTINIT
#endif

#endif // POAC_UTIL_TERMCOLOR2_CONFIG_HPP_

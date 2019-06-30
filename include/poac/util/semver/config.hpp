#ifndef SEMVER_CONFIG_HPP
#define SEMVER_CONFIG_HPP

#ifdef __cplusplus
#ifndef SEMVER_STD_VER
#  if __cplusplus <= 201103L
#    define TERMCOLOR2_STD_VER 11
#  elif __cplusplus <= 201402L
#    define TERMCOLOR2_STD_VER 14
#  elif __cplusplus <= 201703L
#    define SEMVER_STD_VER 17
#  else
#    define SEMVER_STD_VER 18  // current year, or date of c++2a ratification
#  endif
#endif  // !SEMVER_STD_VER
#endif // !__cplusplus

#if SEMVER_STD_VER > 11
#  define SEMVER_AFTER_CXX11
#endif

#if SEMVER_STD_VER > 14
#  define SEMVER_AFTER_CXX14
#endif

#if SEMVER_STD_VER > 17
#  define SEMVER_AFTER_CXX17
#endif

#endif	// !SEMVER_CONFIG_HPP

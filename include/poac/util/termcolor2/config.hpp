#ifndef TERMCOLOR2_CONFIG_HPP
#define TERMCOLOR2_CONFIG_HPP

#ifdef __cplusplus
#  ifndef TERMCOLOR2_STD_VER
#    if __cplusplus < 201103L
#      error "termcolor2 requires C++11 or later."
#    elif __cplusplus == 201103L
#      define TERMCOLOR2_STD_VER 11
#    elif __cplusplus <= 201402L
#      define TERMCOLOR2_STD_VER 14
#    elif __cplusplus <= 201703L
#      define TERMCOLOR2_STD_VER 17
#    else
#      define TERMCOLOR2_STD_VER 19 // current year, or date of c++2a ratification
#    endif
#  endif // !TERMCOLOR2_STD_VER
#else
#  error "__cplusplus is not defined"
#endif // !__cplusplus

#if TERMCOLOR2_STD_VER > 11
#  define TERMCOLOR2_CHAR_TRAITS_AFTER_CXX11 true
#else
#  define TERMCOLOR2_CHAR_TRAITS_AFTER_CXX11 false
#endif

#if TERMCOLOR2_STD_VER > 14
#  define TERMCOLOR2_CHAR_TRAITS_AFTER_CXX14 true
#  define TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 inline
#else
#  define TERMCOLOR2_CHAR_TRAITS_AFTER_CXX14 false
#  define TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14
#endif

#if TERMCOLOR2_STD_VER > 17
#  define TERMCOLOR2_CHAR_TRAITS_AFTER_CXX17 true
#else
#  define TERMCOLOR2_CHAR_TRAITS_AFTER_CXX17 false
#endif

#ifdef TERMCOLOR2_USE_GNU_STRING_LITERAL_OPERATOR_TEMPLATE
#  ifdef _MSC_VER
//   Does not implement constexpr color literals for MSVC.
#    define COLOR_LITERALS_CONSTEXPR const
#    undef TERMCOLOR2_USE_GNU_STRING_LITERAL_OPERATOR_TEMPLATE
#    ifndef TERMCOLOR2_NO_WARNING
#      pragma message ( "You cannot enable gnu-string-literal-operator-template on MSVC." )
#    endif // !TERMCOLOR2_NO_WARNING
#  elif !defined(TERMCOLOR2_NO_WARNING)
#    warning "gnu-string-literal-operator-template is enabled." \
             "You will see a warning associated with it." \
             "If you want to hide warning, please pass -Wno-gnu-string-literal-operator-template to compiler option."
#    define COLOR_LITERALS_CONSTEXPR constexpr
#  else
#    define COLOR_LITERALS_CONSTEXPR constexpr
#  endif // !_MSC_VER
#else
#  define COLOR_LITERALS_CONSTEXPR const
#endif // !TERMCOLOR2_USE_GNU_STRING_LITERAL_OPERATOR_TEMPLATE

#endif	// !TERMCOLOR2_CONFIG_HPP

#ifndef TERMCOLOR2_CONFIG_HPP
#define TERMCOLOR2_CONFIG_HPP

#ifdef TERMCOLOR2_USE_GNU_STRING_LITERAL_OPERATOR_TEMPLATE
#  ifdef _MSC_VER
//   Does not implement constexpr color literals for MSVC.
#    define COLOR_LITERALS_CONSTEXPR const
#    undef TERMCOLOR2_USE_GNU_STRING_LITERAL_OPERATOR_TEMPLATE
#    ifndef TERMCOLOR2_NO_WARNING
#      pragma message ( "You cannot enable gnu-string-literal-operator-template on MSVC." )
#    endif
#  elif !defined(TERMCOLOR2_NO_WARNING)
#    warning "gnu-string-literal-operator-template is enabled." \
             "You will see a warning associated with it." \
             "If you want to hide warning, please pass -Wno-gnu-string-literal-operator-template to compiler option."
#    define COLOR_LITERALS_CONSTEXPR constexpr
#  else
#    define COLOR_LITERALS_CONSTEXPR constexpr
#  endif
#else
#  define COLOR_LITERALS_CONSTEXPR const
#endif

#endif	// !TERMCOLOR2_CONFIG_HPP

#ifndef POAC_IO_FILE_MARKDOWN_HPP
#define POAC_IO_FILE_MARKDOWN_HPP

#include <iostream>
#include <string>
#include <vector>
#include <cstring>

#include <boost/filesystem.hpp>
#include <boost/optional.hpp>
#include <boost/spirit/home/x3.hpp>

#define BOOST_SPIRIT_X3_DEBUG


namespace poac::io::file::markdown {
    namespace detail {
        decltype(auto) assign() {
            using boost::spirit::x3::_val;
            using boost::spirit::x3::_attr;
            return [](auto&& ctx) { _val(ctx) = _attr(ctx); };
        }
        template <typename F>
        decltype(auto) assign(F&& f) {
            using boost::spirit::x3::_val;
            using boost::spirit::x3::_attr;
            return [f](auto&& ctx) { _val(ctx) = f(_attr(ctx)); };
        }
    }

    struct primary;
    struct h1_expr;
    struct h2_expr;
    struct expression;

    const boost::spirit::x3::rule<primary, std::string> primary;
    const boost::spirit::x3::rule<h1_expr, std::string> h1_expr;
    const boost::spirit::x3::rule<h2_expr, std::string> h2_expr;
    const boost::spirit::x3::rule<expression, std::string> expression;

    const auto primary_def =
            (*boost::spirit::x3::char_)[detail::assign()]
    ;
    const auto h1_expr_def =
            ("#" >> primary >> boost::spirit::x3::eol)[detail::assign()]
            // | ("#" >> primary)[detail::assign()]
            | primary[detail::assign()]
    ;
    const auto h2_expr_def =
        h1_expr[detail::assign()]
        >> *(
            ("##" >> h1_expr >> boost::spirit::x3::eol)[detail::assign()]
            // | ("##" >> h1_expr)[detail::assign()]
        )
    ;
    const auto expression_def =
            h2_expr[detail::assign()]
    ;

    // primary -> primary_def
    BOOST_SPIRIT_DEFINE(
            primary,
            h1_expr,
            h2_expr,
            expression
            );


    boost::optional<std::string> to_json(const std::string& md) {
        namespace x3 = boost::spirit::x3;

        std::string result;
        auto first = std::cbegin(md);
        const auto last = std::cend(md);

//        bool success = x3::phrase_parse(first, last, x3::lit('#') >> *x3::char_, x3::space, result);
        bool success = x3::phrase_parse(first, last, expression, x3::blank, result);

        if (!success || first != last) { // parse failed
            return boost::none;
        } else {
            return result;
        }

//        md_grammar<const std::string> grammar;


//        const qi::rule<std::string::const_iterator, std::string(), qi::space_type> rule =
//                qi::lit('#') >> *qi::char_;
//        std::string value;
//        double result;
//
//        if (qi::phrase_parse(first, last, grammar, qi::space, result))
//            return std::to_string(result);
//        else // parse error
//            return boost::none;
    }
} // end namespace
#endif // !POAC_IO_FILE_MARKDOWN_HPP

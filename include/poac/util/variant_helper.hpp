#ifndef POAC_UTIL_VARIANT_HELPER_HPP
#define POAC_UTIL_VARIANT_HELPER_HPP

#include <utility>
#include <variant>

#include <boost/variant/variant.hpp>
#include <boost/variant/static_visitor.hpp>
#include <boost/variant/apply_visitor.hpp>

namespace poac::util {
    namespace detail {
        template <typename Visitor, typename Result>
        struct visitor : public boost::static_visitor<Result>, public Visitor {
            explicit visitor(Visitor&& v) noexcept
                : Visitor(std::forward<Visitor>(v))
            {}
        };
    }

    template <typename Visitor, typename Head, typename... Tail>
    inline auto
    visit(Visitor&& visitor, boost::variant<Head, Tail...>&& v) {
        return boost::apply_visitor(detail::visitor<
                    Visitor, decltype(std::declval<Visitor>()(std::declval<const Head&>()))
                >{std::forward<Visitor>(visitor)}, std::move(v));
    }

    template <typename Visitor, typename Head, typename... Tail>
    inline auto
    visit(Visitor&& visitor, const boost::variant<Head, Tail...>& v) {
        return boost::apply_visitor(detail::visitor<
                    Visitor, decltype(std::declval<Visitor>()(std::declval<const Head&>()))
                >{std::forward<Visitor>(visitor)}, v);
    }

    template <typename Visitor, typename Head, typename... Tail>
    inline auto
    visit(Visitor&& visitor, boost::variant<Head, Tail...>& v) {
        return boost::apply_visitor(detail::visitor<
                    Visitor, decltype(std::declval<Visitor>()(std::declval<Head &>()))
                >{std::forward<Visitor>(visitor)}, v);
    }

    template <typename Visitor, typename Head, typename... Tail, typename Head2, typename... Tail2>
    inline auto
    visit(Visitor&& visitor, const boost::variant<Head, Tail...>& v, const boost::variant<Head2, Tail2...>& v2) {
        return boost::apply_visitor(detail::visitor<
                    Visitor, decltype(std::declval<Visitor>()(
                            std::declval<const Head&>(),
                            std::declval<const Head2&>()))
                >{std::forward<Visitor>(visitor)}, v, v2);
    }

    template <typename Visitor, typename Head, typename... Tail, typename Head2, typename... Tail2>
    inline auto
    visit(Visitor&& visitor, const boost::variant<Head, Tail...>& v, boost::variant<Head2, Tail2...>& v2) {
        return boost::apply_visitor(detail::visitor<
                    Visitor, decltype(std::declval<Visitor>()(
                            std::declval<Head&>(),
                            std::declval<Head2>()))
                >{std::forward<Visitor>(visitor)}, v, v2);
    }
}

#endif // !POAC_UTIL_VARIANT_HELPER_HPP

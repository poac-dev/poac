#ifndef POAC_UTIL_TYPES_HPP
#define POAC_UTIL_TYPES_HPP

#include <optional>
#include <stack>
#include <type_traits>
#include <utility>
#include <unordered_map>
#include <vector>

namespace poac::util::types {
    // If the type T is a reference type, provides the member typedef type
    //  which is the type referred to by T with its topmost cv-qualifiers removed.
    // Otherwise type is T with its topmost cv-qualifiers removed.
    // C++20, std::remove_cvref_t<T>
    template<typename T>
    using remove_cvref_t = std::remove_cv_t<std::remove_reference_t<T>>;

    // std::conditional for non-type template
    template <auto Value>
    struct value_holder { static constexpr auto value = Value; };
    template <bool B, auto T, auto F>
    using non_type_conditional = std::conditional<B, value_holder<T>, value_holder<F>>;
    template <bool B, auto T, auto F>
    using non_type_conditional_t = std::conditional_t<B, value_holder<T>, value_holder<F>>;
    template <bool B, auto T, auto F>
    static constexpr auto non_type_conditional_v = non_type_conditional_t<B, T, F>::value;

    template <class SinglePassRange, class T>
    std::optional<std::size_t>
    index_of(const SinglePassRange& rng, const T& t) {
        auto first = std::cbegin(rng);
        auto last = std::cend(rng);
        const auto result = std::find(first, last, t);
        if (result == last) {
            return std::nullopt;
        }
        else {
            return std::distance(first, result);
        }
    }
    template <typename InputIterator, typename T>
    inline auto index_of(InputIterator first, InputIterator last, const T& value) {
        return std::distance(first, std::find(first, last, value));
    }
    template <typename InputIterator, typename Predicate>
    inline auto index_of_if(InputIterator first, InputIterator last, Predicate pred) {
        return std::distance(first, std::find_if(first, last, pred));
    }

    // Check if it has duplicate elements.
    template <class SinglePassRange>
    bool duplicate(const SinglePassRange& rng) {
        auto first = std::cbegin(rng);
        auto last = std::cend(rng);
        for (const auto& r : rng) {
            int c = std::count(first, last, r);
            if (c > 1) {
                return true;
            }
        }
        return false;
    }

    // boost::property_tree::ptree : {"key": ["array", "...", ...]}
    //  -> std::vector<T> : ["array", "...", ...]
    template <typename T, typename U, typename K=typename U::key_type>
    std::vector<T> ptree_to_vector(const U& pt, const K& key) { // ptree_to_vector(pt, "key")
        std::vector<T> r;
        for (const auto& item : pt.get_child(key)) {
            r.push_back(item.second.template get_value<T>());
        }
        return r;
    }
    // boost::property_tree::ptree : ["array", "...", ...]
    //  -> std::vector<T> : ["array", "...", ...]
    template <typename T, typename U>
    std::vector<T> ptree_to_vector(const U &pt) {
        std::vector<T> r;
        for (const auto& item : pt) {
            r.push_back(item.second.template get_value<T>());
        }
        return r;
    }

    // boost::property_tree::ptree :
    //   {"key1": "value1",
    //    "key2": "value2", ...}
    // -> std::unordered_map<std::string, T>
    template <typename T>
    std::unordered_map<std::string, T>
    ptree_to_unordered_map(const boost::property_tree::ptree& pt, const std::string& key) {
        std::unordered_map<std::string, T> m{};
        for (const auto& [k, v] : pt.get_child(key)) {
            m.emplace(k, v.get_value<T>());
        }
        return m;
    }

    template<typename Tuple, typename I, I... Indices>
    std::array<
        std::tuple_element_t<0, remove_cvref_t<Tuple>>,
        std::tuple_size_v<remove_cvref_t<Tuple>>
    >
    tuple_to_array(Tuple&& tuple, std::integer_sequence<I, Indices...>) {
        return { std::get<Indices>(std::forward<Tuple>(tuple))... };
    }
    template<typename Tuple>
    auto tuple_to_array(Tuple&& tuple) {
        return tuple_to_array(
                std::forward<Tuple>(tuple),
                std::make_index_sequence<std::tuple_size_v<remove_cvref_t<Tuple>>>());
    }
} // end namespace
#endif // !POAC_UTIL_TYPES_HPP

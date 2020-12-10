#ifndef POAC_UTIL_META_HPP
#define POAC_UTIL_META_HPP

#include <optional>
#include <stack>
#include <type_traits>
#include <utility>
#include <unordered_map>
#include <vector>

namespace poac::util::meta {
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
        auto result = std::find(first, last, t);
        if (result == last) {
            return std::nullopt;
        } else {
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
        auto result = std::find_if(first, last, [&](const auto& v){
            return std::count(first, last, v) > 1;
        });
        return result != last;
    }

    // boost::property_tree::ptree : {"key": ["array", "...", ...]}
    //  -> std::vector<T> : ["array", "...", ...]
    template <typename T, typename U, typename K=typename U::key_type>
    auto to_vector(const U& value, const K& key)
        -> std::enable_if_t<
               std::is_same_v<
                   remove_cvref_t<U>,
                   boost::property_tree::ptree
               >,
               std::vector<T>>
    {
        std::vector<T> r;
        for (const auto& item : value.get_child(key)) {
            r.push_back(item.second.template get_value<T>());
        }
        return r;
    }

    // boost::property_tree::ptree : ["array", "...", ...]
    //  -> std::vector<T> : ["array", "...", ...]
    template <typename T, typename U>
    auto to_vector(const U& value)
        -> std::enable_if_t<
            std::is_same_v<
                remove_cvref_t<U>,
                boost::property_tree::ptree
            >,
            std::vector<T>>
    {
        std::vector<T> r;
        for (const auto& item : value) {
            r.push_back(item.second.template get_value<T>());
        }
        return r;
    }

    // boost::property_tree::ptree :
    //   {"key1": "value1",
    //    "key2": "value2", ...}
    // -> std::unordered_map<std::string, T>
    template <typename T, typename U>
    auto to_unordered_map(const U& value, const std::string& key)
        -> std::enable_if_t<
               std::is_same_v<
                   remove_cvref_t<U>,
                   boost::property_tree::ptree
               >,std::unordered_map<std::string, T>>
    {
        std::unordered_map<std::string, T> m{};
        for (const auto& [k, v] : value.get_child(key)) {
            m.emplace(k, v.template get_value<T>());
        }
        return m;
    }

    template <typename T, typename... Ts>
    struct are_all_same : std::conjunction<std::is_same<T, Ts>...> {};

    template <typename T, typename... Ts>
    inline constexpr bool are_all_same_v = are_all_same<T, Ts...>::value;

    template <typename T, template <typename...> class Container>
    struct is_specialization : std::false_type {};

    template <template<typename...> class Container, typename... Args>
    struct is_specialization<Container<Args...>, Container>: std::true_type {};

    template <typename T>
    struct is_tuple : is_specialization<T, std::tuple> {};

    template <typename T>
    inline constexpr bool is_tuple_v = is_tuple<T>::value;

    template <typename T, std::size_t... Indices>
    constexpr auto to_array(T&& tuple, std::index_sequence<Indices...>)
        noexcept(
            std::is_nothrow_constructible_v<
                std::array<
                    std::tuple_element_t<0, remove_cvref_t<T>>,
                    std::tuple_size_v<remove_cvref_t<T>>
                >,
                std::tuple_element_t<Indices, remove_cvref_t<T>>...>
        )
        -> std::enable_if_t<
             std::conjunction_v<
                 is_tuple<remove_cvref_t<T>>,
                 are_all_same<
                     std::tuple_element_t<Indices, remove_cvref_t<T>>...
                 >
             >,
             std::array<
                 std::tuple_element_t<0, remove_cvref_t<T>>,
                 std::tuple_size_v<remove_cvref_t<T>>
             >>
    {
        return { std::get<Indices>(std::forward<T>(tuple))... };
    }

    template <
        typename T,
        std::enable_if_t<
            is_tuple_v<remove_cvref_t<T>>,
            std::nullptr_t
        > = nullptr>
    constexpr auto to_array(T&& tuple)
    {
        return to_array(
            std::forward<T>(tuple),
            std::make_index_sequence<
                std::tuple_size_v<remove_cvref_t<T>>
            >{}
        );
    }
} // end namespace
#endif // !POAC_UTIL_MATA_HPP

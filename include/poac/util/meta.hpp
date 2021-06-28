#ifndef POAC_UTIL_META_HPP
#define POAC_UTIL_META_HPP

// std
#include <chrono>
#include <optional>
#include <stack>
#include <string>
#include <type_traits>
#include <utility>
#include <unordered_map>
#include <vector>

// external
#include <boost/property_tree/ptree.hpp>

namespace poac::util::meta {
    // std::conditional for non-type template
    template <auto Value>
    struct value_holder { static constexpr auto value = Value; };
    template <bool B, auto T, auto F>
    using non_type_conditional = std::conditional<B, value_holder<T>, value_holder<F>>;
    template <bool B, auto T, auto F>
    using non_type_conditional_t = std::conditional_t<B, value_holder<T>, value_holder<F>>;
    template <bool B, auto T, auto F>
    static constexpr auto non_type_conditional_v = non_type_conditional_t<B, T, F>::value;

    template <class InputIterator, class T>
    inline auto index_of(InputIterator first, InputIterator last, const T& value) {
        return std::distance(first, std::find(first, last, value));
    }

    template <class InputIterator, class Predicate>
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

    // found: true
    template <class SinglePassRange, class T>
    bool find(const SinglePassRange& rng, const T& value) {
        auto first = std::cbegin(rng);
        auto last = std::cend(rng);
        return std::find(first, last, value) != last;
    }

    // found: true
    template <class SinglePassRange, class Predicate>
    bool find_if(const SinglePassRange& rng, Predicate pred) {
        auto first = std::cbegin(rng);
        auto last = std::cend(rng);
        return std::find_if(first, last, pred) != last;
    }

    // boost::property_tree::ptree : {"key": ["array", "...", ...]}
    //  -> std::vector<T> : ["array", "...", ...]
    template <class T, class U, class K=typename U::key_type>
    auto to_vector(const U& value, const K& key)
        -> std::enable_if_t<
               std::is_same_v<
                   std::remove_cvref_t<U>,
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
    template <class T, class U>
    auto to_vector(const U& value)
        -> std::enable_if_t<
            std::is_same_v<
                std::remove_cvref_t<U>,
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
    template <class T, class U>
    auto to_unordered_map(const U& value, const std::string& key)
        -> std::enable_if_t<
               std::is_same_v<
                   std::remove_cvref_t<U>,
                   boost::property_tree::ptree
               >,
               std::unordered_map<std::string, T>>
    {
        std::unordered_map<std::string, T> m{};
        const auto child = value.get_child_optional(key);
        if (child.has_value()) {
            for (const auto& [k, v] : child.value()) {
                m.emplace(k, v.template get_value<T>());
            }
        }
        return m;
    }

    template <class T, class... Ts>
    struct are_all_same : std::conjunction<std::is_same<T, Ts>...> {};

    template <class T, class... Ts>
    inline constexpr bool are_all_same_v = are_all_same<T, Ts...>::value;

    template <class T, template <class...> class Container>
    struct is_specialization : std::false_type {};

    template <template<class...> class Container, class... Args>
    struct is_specialization<Container<Args...>, Container>: std::true_type {};

    template <class T>
    struct is_tuple : is_specialization<T, std::tuple> {};

    template <class T>
    inline constexpr bool is_tuple_v = is_tuple<T>::value;

    template <class T, std::size_t... Indices>
    constexpr auto to_array(T&& tuple, std::index_sequence<Indices...>)
        noexcept(
            std::is_nothrow_constructible_v<
                std::array<
                    std::tuple_element_t<0, std::remove_cvref_t<T>>,
                    std::tuple_size_v<std::remove_cvref_t<T>>
                >,
                std::tuple_element_t<Indices, std::remove_cvref_t<T>>...>
        )
        -> std::enable_if_t<
             std::conjunction_v<
                 is_tuple<std::remove_cvref_t<T>>,
                 are_all_same<
                     std::tuple_element_t<Indices, std::remove_cvref_t<T>>...
                 >
             >,
             std::array<
                 std::tuple_element_t<0, std::remove_cvref_t<T>>,
                 std::tuple_size_v<std::remove_cvref_t<T>>
             >>
    {
        return { std::get<Indices>(std::forward<T>(tuple))... };
    }

    template <
        class T,
        std::enable_if_t<
            is_tuple_v<std::remove_cvref_t<T>>,
            std::nullptr_t
        > = nullptr>
    constexpr auto to_array(T&& tuple)
    {
        return to_array(
            std::forward<T>(tuple),
            std::make_index_sequence<
                std::tuple_size_v<std::remove_cvref_t<T>>
            >{}
        );
    }

    inline std::string
    time_to_string(const std::chrono::seconds& time) {
        return std::to_string(time.count());
    }

    // ref: https://qiita.com/rinse_/items/f00bb2a78d14c3c2f9fa
    template <class Range>
    class containerizer {
        Range range;

    public:
        explicit containerizer(Range&& r) noexcept
            : range{std::forward<Range>(r)} {}

        template <class To>
        operator To() const {
            return To(std::begin(range), std::end(range));
        }
    };

    template <class Range>
    inline containerizer<Range> containerize(Range&& range) {
        return containerizer<Range>(std::forward<Range>(range));
    }

    struct containerized_tag {};
    constexpr containerized_tag containerized;

    template <class Range>
    inline containerizer<Range> operator|(Range&& range, containerized_tag) {
        return containerize(std::forward<Range>(range));
    }
} // end namespace
#endif // !POAC_UTIL_MATA_HPP

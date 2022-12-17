#pragma once

// std
#include <chrono>
#include <optional>
#include <stack>
#include <type_traits>
#include <utility>

// external
#include <boost/property_tree/ptree.hpp>

// internal
#include "poac/util/rustify.hpp"

namespace poac::util::meta {

// std::conditional for non-type template
template <auto Value>
struct ValueHolder {
  static constexpr auto VALUE = Value;
};
template <bool B, auto T, auto F>
using NonTypeConditional = std::conditional<B, ValueHolder<T>, ValueHolder<F>>;
template <bool B, auto T, auto F>
using NonTypeConditionalT =
    std::conditional_t<B, ValueHolder<T>, ValueHolder<F>>;
template <bool B, auto T, auto F>
static constexpr auto NON_TYPE_CONDITIONAL_V =
    NonTypeConditionalT<B, T, F>::VALUE;

template <class InputIterator, class T>
inline Fn index_of(InputIterator first, InputIterator last, const T& value) {
  return std::distance(first, std::find(first, last, value));
}

template <class InputIterator, class Predicate>
inline Fn index_of_if(InputIterator first, InputIterator last, Predicate pred) {
  return std::distance(first, std::find_if(first, last, pred));
}

// Check if it has duplicate elements.
template <class SinglePassRange>
Fn duplicate(const SinglePassRange& rng)->bool {
  LetMut first = std::cbegin(rng);
  LetMut last = std::cend(rng);
  LetMut result = std::find_if(first, last, [&](Let& v) {
    return std::count(first, last, v) > 1;
  });
  return result != last;
}

// found: true
template <class SinglePassRange, class T>
Fn find(const SinglePassRange& rng, const T& value)->bool {
  LetMut first = std::cbegin(rng);
  LetMut last = std::cend(rng);
  return std::find(first, last, value) != last;
}

// found: true
template <class SinglePassRange, class Predicate>
Fn find_if(const SinglePassRange& rng, Predicate pred)->bool {
  LetMut first = std::cbegin(rng);
  LetMut last = std::cend(rng);
  return std::find_if(first, last, pred) != last;
}

// boost::property_tree::ptree : {"key": ["array", "...", ...]}
//  -> std::vector<T> : ["array", "...", ...]
template <class T, class U, class K = typename U::key_type>
Fn to_vec(const U& value, const K& key)
    ->std::enable_if_t<
        std::is_same_v<std::remove_cvref_t<U>, boost::property_tree::ptree>,
        Vec<T>> {
  Vec<T> r;
  for (Let& item : value.get_child(key)) {
    r.push_back(item.second.template get_value<T>());
  }
  return r;
}

// boost::property_tree::ptree : ["array", "...", ...]
//  -> std::vector<T> : ["array", "...", ...]
template <class T, class U>
Fn to_vec(const U& value)
    ->std::enable_if_t<
        std::is_same_v<std::remove_cvref_t<U>, boost::property_tree::ptree>,
        Vec<T>> {
  Vec<T> r;
  for (Let& item : value) {
    r.push_back(item.second.template get_value<T>());
  }
  return r;
}

// boost::property_tree::ptree :
//   {"key1": "value1",
//    "key2": "value2", ...}
// -> std::unordered_map<String, T>
template <class T, class U>
Fn to_hash_map(const U& value, const String& key)
    ->std::enable_if_t<
        std::is_same_v<std::remove_cvref_t<U>, boost::property_tree::ptree>,
        HashMap<String, T>> {
  HashMap<String, T> m{};
  Let child = value.get_child_optional(key);
  if (child.has_value()) {
    for (Let & [ k, v ] : child.value()) {
      m.emplace(k, v.template get_value<T>());
    }
  }
  return m;
}

template <class T, class... Ts>
struct AreAllSame : std::conjunction<std::is_same<T, Ts>...> {};

template <class T, class... Ts>
inline constexpr bool ARE_ALL_SAME_V = AreAllSame<T, Ts...>::value;

template <class T, template <class...> class Container>
struct IsSpecialization : std::false_type {};

template <template <class...> class Container, class... Args>
struct IsSpecialization<Container<Args...>, Container> : std::true_type {};

template <class T>
struct IsTuple : IsSpecialization<T, std::tuple> {};

template <class T>
inline constexpr bool IS_TUPLE_V = IsTuple<T>::value;

// clang-format off
template <class T, usize... Indices>
constexpr Fn to_array(T&& tuple, std::index_sequence<Indices...> /*unused*/)
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
             IsTuple<std::remove_cvref_t<T>>,
             AreAllSame<
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
// clang-format on

template <
    class T, std::enable_if_t<
                 IS_TUPLE_V<std::remove_cvref_t<T>>, std::nullptr_t> = nullptr>
constexpr Fn to_array(T&& tuple) {
  return to_array(
      std::forward<T>(tuple),
      std::make_index_sequence<std::tuple_size_v<std::remove_cvref_t<T>>>{}
  );
}

inline Fn time_to_string(const std::chrono::seconds& time)->String {
  return std::to_string(time.count());
}

// ref: https://qiita.com/rinse_/items/f00bb2a78d14c3c2f9fa
template <class Range>
class Containerizer {
  Range range;

public:
  explicit Containerizer(Range&& r) noexcept : range{std::forward<Range>(r)} {}

  template <class To>
  operator To() const { // NOLINT(google-explicit-constructor)
    return To(std::begin(range), std::end(range));
  }
};

template <class Range>
inline Fn containerize(Range&& range)->Containerizer<Range> {
  return Containerizer<Range>(std::forward<Range>(range));
}

struct ContainerizedTag {};
constexpr ContainerizedTag CONTAINERIZED;

template <class Range>
inline Fn operator|(Range&& range, ContainerizedTag /*unused*/)
    ->Containerizer<Range> {
  return containerize(std::forward<Range>(range));
}

} // namespace poac::util::meta

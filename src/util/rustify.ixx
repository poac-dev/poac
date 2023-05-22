module;

// std
#include <array>
#include <cstddef> // std::size_t
#include <cstdint>
#include <filesystem>
#include <iterator> // std::begin, std::end
#include <map>
#include <optional>
#include <string>
#include <string_view>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <variant> // std::monostate
#include <vector>

// external
#include <boost/container_hash/hash.hpp>

export module poac.util.rustify;

export namespace poac {

//
// Namespaces
//
namespace fs = std::filesystem;

//
// Data types
//
// NOLINTBEGIN(readability-identifier-naming)
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using u64 = std::uint64_t;

using i8 = std::int8_t;
using i16 = std::int16_t;
using i32 = std::int32_t;
using i64 = std::int64_t;

using usize = std::size_t;

using f32 = float;
using f64 = double;
// NOLINTEND(readability-identifier-naming)

using String = std::string;
using StringRef = std::string_view;
// static_assert(String::npos == StringRef::npos, "npos should be the same");

using Path = fs::path;

template <typename T, usize N>
using Arr = std::array<T, N>;
template <typename T>
using Vec = std::vector<T>;

template <typename T>
using Option = std::optional<T>;

struct NoneT : protected std::monostate {
  constexpr auto operator==(const usize rhs) const -> bool {
    return String::npos == rhs;
  }

  // NOLINTNEXTLINE(google-explicit-constructor)
  constexpr operator std::nullopt_t() const { return std::nullopt; }

  template <typename T>
  constexpr operator Option<T>() const { // NOLINT(google-explicit-constructor)
    return std::nullopt;
  }
};
inline constexpr NoneT None; // NOLINT(readability-identifier-naming)

template <typename K, typename V>
using Map = std::map<K, V>;
template <typename K, typename V, typename H = boost::hash<K>>
using HashMap = std::unordered_map<K, V, H>;

template <typename K>
using HashSet = std::unordered_set<K, boost::hash<K>>;

//
// String literals
//
// using namespace std::literals::string_literals;
// using namespace std::literals::string_view_literals;

inline auto operator""_path(const char* str, usize /*unused*/) -> Path {
  return str;
}

//
// Utilities
//
template <typename T, typename U>
inline void append(Vec<T>& a, const Vec<U>& b) {
  a.insert(a.end(), b.cbegin(), b.cend());
}
template <typename K, typename V, typename H>
inline void append(HashMap<K, V, H>& a, const HashMap<K, V, H>& b) {
  a.insert(b.cbegin(), b.cend());
}

template <typename T, usize N, typename U>
constexpr auto contains(const Arr<T, N>& a, const U& b) -> bool {
  return std::find(std::begin(a), std::end(a), b) != std::end(a);
}

} // namespace poac

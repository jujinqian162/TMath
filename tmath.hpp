#ifndef TMATH_HPP
#define TMATH_HPP

#include <array>
#include <cmath>
#include <ranges>
#include <vector>

template <typename T>
constexpr bool enable_math_v = false;

template <typename T>
constexpr bool enable_math_v<std::vector<T>> = true;

template <typename T, std::size_t N>
constexpr bool enable_math_v<std::array<T, N>> = true;

namespace concepts {
template <typename T>
// concept math_arr = enable_math_v<std::decay_t<T>> || std::ranges::view<T>;
concept math_arr =
    enable_math_v<std::decay_t<T>> || std::ranges::view<std::decay_t<T>>;
}

namespace detail {
template <typename T, typename U>
constexpr bool check_size_equal(T& lhs, U& rhs) {
    if (std::ranges::size(lhs) != std::ranges::size(rhs)) {
        throw std::invalid_argument(
            "Sizes of operands must be equal for element-wise operation");
    }
    return true;
}

} // namespace detail

inline namespace operators {

//////////////////////////////////////////////////////////////////////////////////////////
#define ENABLE_MATH_RIGHT_OPERATORS(_op)                                       \
    template <typename T, typename U>                                          \
    requires(concepts::math_arr<T> && !concepts::math_arr<U>)                  \
    constexpr auto operator _op(T&& lhs, U&& rhs) {                            \
        return std::forward<T>(lhs) |                                          \
               std::ranges::views::transform(                                  \
                   [rhs](auto&& x) { return x _op rhs; });                     \
    }
#define ENABLE_MATH_LEFT_OPERATORS(_op)                                        \
    template <typename T, typename U>                                          \
    requires(concepts::math_arr<T> && !concepts::math_arr<U>)                  \
    constexpr auto operator _op(U&& lhs, T&& rhs) {                            \
        return std::forward<T>(rhs) |                                          \
               std::ranges::views::transform(                                  \
                   [lhs](auto&& x) { return lhs _op x; });                     \
    }

#define ENABLE_MATH_OPERATORS(_op)                                             \
    ENABLE_MATH_RIGHT_OPERATORS(_op)                                           \
    ENABLE_MATH_LEFT_OPERATORS(_op)

#define ENABLE_MATH_BINARY_OPERATORS(_op)                                      \
    template <typename T, typename U>                                          \
    constexpr auto operator _op(T&& lhs, U&& rhs)                              \
    requires(concepts::math_arr<std::decay_t<T>> &&                            \
             concepts::math_arr<std::decay_t<U>>)                              \
    {                                                                          \
        detail::check_size_equal(lhs, rhs);                                    \
        return std::ranges::views::zip(std::forward<T>(lhs),                   \
                                       std::forward<U>(rhs)) |                 \
               std::ranges::views::transform([](auto&& pair) {                 \
                   auto&& [l, r] = pair;                                       \
                   return l _op r;                                             \
               });                                                             \
    }

/////////////////////////////////////////////////////////////////////////////////////////
ENABLE_MATH_OPERATORS(*)
ENABLE_MATH_BINARY_OPERATORS(*)

//////////////////////////////////////////////////////////////////////////////////////////
ENABLE_MATH_OPERATORS(+)
ENABLE_MATH_BINARY_OPERATORS(+)

//////////////////////////////////////////////////////////////////////////////////////////
ENABLE_MATH_OPERATORS(-)
ENABLE_MATH_BINARY_OPERATORS(-)

template <concepts::math_arr T>
constexpr auto operator-(T&& lhs)
requires(concepts::math_arr<T>)
{
    return std::forward<T>(lhs) |
           std::views::transform([](auto&& x) { return -x; });
}

//////////////////////////////////////////////////////////////////////////////////////////
ENABLE_MATH_OPERATORS(/)
ENABLE_MATH_BINARY_OPERATORS(/)

//////////////////////////////////////////////////////////////////////////////////////////
ENABLE_MATH_OPERATORS(%)
ENABLE_MATH_BINARY_OPERATORS(%)

} // namespace operators

namespace tmath {


template <concepts::math_arr T>
constexpr decltype(auto) foreach (T&& x, auto&& func)
{
    return std::forward<T>(x) |
           std::ranges::views::transform([func]<typename U>(U&& ele) { return foreach(std::forward<U>(ele), func); });
}

template <typename T>
constexpr decltype(auto) foreach (T&& x, auto&& func)
{
    return func(std::forward<T>(x));
}

template <typename T, typename U>
constexpr decltype(auto) pow(T&& lhs, U&& rhs) {
    return foreach(std::forward<T>(lhs), [rhs](auto&& x) { return std::pow(x, rhs); });
}

template <typename T>
constexpr decltype(auto) exp(T&& x) {
    return foreach(std::forward<T>(x), [](auto&& x) { return std::exp(x); });
}

template <typename T>
constexpr decltype(auto) log(T&& x) {
    return foreach(std::forward<T>(x), [](auto&& x) { return std::log(x); });
}

template <typename T>
constexpr decltype(auto) log10(T&& x) {
    return foreach(std::forward<T>(x), [](auto&& x) { return std::log10(x); });
}

template <typename T>
constexpr decltype(auto) sqrt(T&& x) {
    return foreach(std::forward<T>(x), [](auto&& x) { return std::sqrt(x); });
}

template <typename T>
constexpr decltype(auto) sin(T&& x) {
    return foreach(std::forward<T>(x), [](auto&& x) { return std::sin(x); });
}

template <typename T>
constexpr decltype(auto) cos(T&& x) {
    return foreach(std::forward<T>(x), [](auto&& x) { return std::cos(x); });
}

template <typename T>
constexpr decltype(auto) tan(T&& x) {
    return foreach(std::forward<T>(x), [](auto&& x) { return std::tan(x); });
}


} // namespace tmath

#endif

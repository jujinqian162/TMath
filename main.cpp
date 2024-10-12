#include "tmath.hpp"
#include <catch2/catch_test_macros.hpp>
#include <ranges>

TEST_CASE("ranges::views") {
    std::vector<int> vec = {1, 2, 3, 4, 5};
    auto transformed = vec | std::views::transform([](int x) { return x * 2; });
    std::vector<int> expected = {2, 4, 6, 8, 10};
    CHECK(std::ranges::equal(transformed, expected));
    CHECK(transformed[0] == 2);
    // Drop the first 2 elements from the transformed view
    auto droped = transformed | std::views::drop(2);
    CHECK(droped[0] == 6);

    auto take_while =
        droped | std::views::take_while([](int x) { return x >= 3; });
    CHECK(std::ranges::equal(take_while, std::vector<int>{6, 8, 10}));
}

TEST_CASE("static_assert") {
    static_assert(concepts::math_arr<std::vector<int>>);
    static_assert(concepts::math_arr<std::array<int, 3>>);
    static_assert(concepts::math_arr<std::views::all_t<std::vector<int>>>);
    static_assert(concepts::math_arr<std::views::all_t<std::array<int, 3>>>);
}

TEST_CASE("operator *") {
    using namespace operators;
    SECTION("Vector multiplication") {
        std::vector<int> vec = {1, 2, 3, 4, 5};
        auto transformed = 3 * vec;
        CHECK(
            std::ranges::equal(transformed, std::vector<int>{3, 6, 9, 12, 15}));
    }

    SECTION("Array multiplication") {
        std::array<int, 3> arr = {1, 2, 3};
        auto transformed_arr = arr * 4;
        CHECK(
            std::ranges::equal(transformed_arr, std::array<int, 3>{4, 8, 12}));
    }

    SECTION("Nested vector multiplication") {
        std::vector<std::vector<int>> vec2{{1, 2}, {3, 4}, {5, 6}};
        auto transformed_vec2 = vec2 * 2;
        CHECK(transformed_vec2[0][0] == 2);
        CHECK(transformed_vec2[0][1] == 4);
        CHECK(transformed_vec2[1][0] == 6);
        CHECK(transformed_vec2[1][1] == 8);
        CHECK(transformed_vec2[2][0] == 10);
        CHECK(transformed_vec2[2][1] == 12);
    }

    SECTION("View multiplication") {
        auto vec3 =
            std::ranges::views::iota(1, 10) |
            std::ranges::views::take_while([](int x) { return x < 10; });
        auto transformed_vec3 = 3 * vec3;
        static_assert(
            std::ranges::random_access_range<decltype(transformed_vec3)>);
        CHECK(std::ranges::equal(
            transformed_vec3,
            std::vector<int>{3, 6, 9, 12, 15, 18, 21, 24, 27}));

        auto fliter_view =
            transformed_vec3 |
            std::ranges::views::filter([](int x) { return x % 2 == 0; });
        static_assert(!std::ranges::random_access_range<decltype(fliter_view)>);
    }

    SECTION("Element-wise multiplication with different container types and "
            "sizes") {
        std::vector<int> vec = {1, 2, 3, 4, 5};
        std::array<int, 3> arr = {1, 2, 3};
        try {
            auto transformed_vec = vec * arr;
        } catch (const std::invalid_argument& e) {
            CHECK(std::string(e.what()) ==
                  "Sizes of operands must be equal for element-wise operation");
        }
        std::array<int, 5> arr2 = {1, 2, 3, 4, 5};
        auto transformed_vec = vec * arr2;
        CHECK(std::ranges::equal(transformed_vec,
                                 std::vector<int>{1, 4, 9, 16, 25}));
    }
}

TEST_CASE("operator /") {
    using namespace operators;
    std::vector<double> vec = {1, 2, 3, 4, 5};
    auto transformed = vec / 2;
    CHECK(std::ranges::equal(transformed,
                             std::vector<double>{0.5, 1.0, 1.5, 2.0, 2.5}));

    auto transformed2 = 2 / vec;
    CHECK(std::ranges::equal(
        transformed2, std::vector<double>{2.0, 1.0, 2.0 / 3.0, 0.5, 0.4}));

    auto transformed3 = vec / std::vector<double>{2, 3, 4, 5, 6};
    CHECK(std::ranges::equal(
        transformed3,
        std::vector<double>{0.5, 2.0 / 3.0, 0.75, 0.8, 5.0 / 6.0}));
}

TEST_CASE("operator + & -") {
    using namespace operators;
    std::vector<double> vec = {1, 2, 3, 4, 5};
    auto transformed = vec + 2;
    CHECK(std::ranges::equal(transformed, std::vector<double>{3, 4, 5, 6, 7}));

    auto transformed2 = 2 + vec;
    CHECK(std::ranges::equal(transformed2, std::vector<double>{3, 4, 5, 6, 7}));

    auto transformed3 = vec - 2;
    CHECK(
        std::ranges::equal(transformed3, std::vector<double>{-1, 0, 1, 2, 3}));

    auto transformed4 = 2 - vec;
    CHECK(std::ranges::equal(transformed4,
                             std::vector<double>{1, 0, -1, -2, -3}));

    auto transformed5 = -vec;
    CHECK(std::ranges::equal(transformed5,
                             std::vector<double>{-1, -2, -3, -4, -5}));

    auto transformed6 = vec - 2.5;
    CHECK(std::ranges::equal(transformed6,
                             std::vector<double>{-1.5, -0.5, 0.5, 1.5, 2.5}));

    auto transformed7 = vec - std::vector<double>{1.5, 2.5, 3.5, 4.5, 5.5};
    CHECK(std::ranges::equal(
        transformed7, std::vector<double>{-0.5, -0.5, -0.5, -0.5, -0.5}));

    auto transformed8 = std::vector<double>{1.5, 2.5, 3.5, 4.5, 5.5} - 2 * vec;
    CHECK(std::ranges::equal(
        transformed8, std::vector<double>{-0.5, -1.5, -2.5, -3.5, -4.5}));
}

TEST_CASE("operator %") {
    using namespace operators;
    std::vector<int> vec = {1, 2, 3, 4, 5};
    auto transformed = vec % 2;
    CHECK(std::ranges::equal(transformed, std::vector<int>{1, 0, 1, 0, 1}));
}

TEST_CASE("math") {
    SECTION("pow") {
        CHECK(tmath::pow(2, 3) == 8);
        CHECK(tmath::pow(2.5, 3) == 15.625);

        std::vector<int> vec = {1, 2, 3, 4, 5};

        SECTION("integer power") {
            auto transformed = tmath::pow(vec, 2);
            CHECK(std::ranges::equal(transformed,
                                     std::vector<int>{1, 4, 9, 16, 25}));
        }
        SECTION("fractional power") {
            auto transformed2 = tmath::pow(vec, 3.5);
            CHECK(std::abs(transformed2[0] - 1.0) < 1e-6);
            CHECK(std::abs(transformed2[1] - 11.3137) < 1e-4);
            CHECK(std::abs(transformed2[2] - 46.7654) < 1e-4);
            CHECK(std::abs(transformed2[3] - 128.0) < 1e-6);
            CHECK(std::abs(transformed2[4] - 279.5085) < 1e-4);
        }
    }

    SECTION("exp") {
        CHECK(tmath::exp(1) == std::exp(1));

        std::vector<int> vec = {1, 2, 3, 4, 5};
        auto transformed = tmath::exp(vec);
        CHECK(std::ranges::equal(transformed,
                                 std::vector<double>{std::exp(1), std::exp(2),
                                                     std::exp(3), std::exp(4),
                                                     std::exp(5)}));
    }

    SECTION("log") {
        CHECK(tmath::log(1) == std::log(1));

        std::vector<double> vec = {1, 2, 3, 4, 5};
        auto transformed = tmath::log(vec);
        CHECK(std::abs(transformed[0] - std::log(1)) < 1e-6);
        CHECK(std::abs(transformed[1] - std::log(2)) < 1e-6);
        CHECK(std::abs(transformed[2] - std::log(3)) < 1e-6);
        CHECK(std::abs(transformed[3] - std::log(4)) < 1e-6);
        CHECK(std::abs(transformed[4] - std::log(5)) < 1e-6);
    }

    SECTION("log10") {
        CHECK(tmath::log10(10) == std::log10(10));

        std::vector<double> vec = {1, 10, 100, 1000, 10000};
        auto transformed = tmath::log10(vec);
        CHECK(std::abs(transformed[0] - 0.0) < 1e-6);
        CHECK(std::abs(transformed[1] - 1.0) < 1e-6);
        CHECK(std::abs(transformed[2] - 2.0) < 1e-6);
        CHECK(std::abs(transformed[3] - 3.0) < 1e-6);
        CHECK(std::abs(transformed[4] - 4.0) < 1e-6);
    }

    SECTION("sqrt") {
        CHECK(tmath::sqrt(4) == std::sqrt(4));

        std::vector<double> vec = {1, 4, 9, 16, 25};
        auto transformed = tmath::sqrt(vec);
        CHECK(std::abs(transformed[0] - 1.0) < 1e-6);
        CHECK(std::abs(transformed[1] - 2.0) < 1e-6);
        CHECK(std::abs(transformed[2] - 3.0) < 1e-6);
        CHECK(std::abs(transformed[3] - 4.0) < 1e-6);
        CHECK(std::abs(transformed[4] - 5.0) < 1e-6);
    }

    SECTION("sin") {
        CHECK(tmath::sin(M_PI / 2) == 1.0);

        std::vector<double> vec = {0, M_PI / 6, M_PI / 4, M_PI / 3, M_PI / 2};
        auto transformed = tmath::sin(vec);
        CHECK(std::abs(transformed[0] - 0.0) < 1e-6);
        CHECK(std::abs(transformed[1] - 0.5) < 1e-6);
        CHECK(std::abs(transformed[2] - std::sqrt(2) / 2) < 1e-6);
        CHECK(std::abs(transformed[3] - std::sqrt(3) / 2) < 1e-6);
        CHECK(std::abs(transformed[4] - 1.0) < 1e-6);
    }

    SECTION("cos") {
        CHECK(tmath::cos(M_PI) == -1.0);

        std::vector<double> vec = {0, M_PI / 3, M_PI / 2, 2 * M_PI / 3, M_PI};
        auto transformed = tmath::cos(vec);
        CHECK(std::abs(transformed[0] - 1.0) < 1e-6);
        CHECK(std::abs(transformed[1] - 0.5) < 1e-6);
        CHECK(std::abs(transformed[2] - 0.0) < 1e-6);
        CHECK(std::abs(transformed[3] + 0.5) < 1e-6);
        CHECK(std::abs(transformed[4] + 1.0) < 1e-6);
    }

    SECTION("tan") {
        CHECK(tmath::tan(10) == std::tan(10));

        std::vector<double> vec = {0, M_PI / 6, M_PI / 4, M_PI / 3};
        auto transformed = tmath::tan(vec);
        CHECK(std::abs(transformed[0] - 0.0) < 1e-6);
        CHECK(std::abs(transformed[1] - std::sqrt(3) / 3) < 1e-6);
        CHECK(std::abs(transformed[2] - 1.0) < 1e-6);
        CHECK(std::abs(transformed[3] - std::sqrt(3)) < 1e-6);
    }
}

namespace fn {
auto square(auto&& x) { return x * x; }

auto reciprocal(auto&& x) { return 1. / x; }

auto sigmoid(auto&& x) { return 1. / (1. + tmath::exp(-x)); }
template <typename T>
auto sigmoid2(T&& x) {
    if constexpr (std::is_rvalue_reference_v<T&&>) {
        return 1. / (1. + tmath::exp(-x)) |
               std::ranges::to<std::vector<double>>();
    } else {
        return 1. / (1. + tmath::exp(-x));
    }
}
} // namespace fn
#define TEST_CUSTOM_FUNCTION
#ifdef TEST_CUSTOM_FUNCTION
TEST_CASE("custom function") {
    using namespace fn;
    std::vector vec = {1, 2, 3, 4, 5};
    SECTION("square") {
        auto transformed = square(vec);
        CHECK(
            std::ranges::equal(transformed, std::vector<int>{1, 4, 9, 16, 25}));
    }
    SECTION("sigmoid") {
        auto v = std::vector<double>{1, 2, 3, 4, 5};
        auto transformed = sigmoid(v);
        CHECK(std::abs(transformed[0] - 0.73105858) < 1e-6);
        CHECK(std::abs(transformed[1] - 0.88079708) < 1e-6);
        CHECK(std::abs(transformed[2] - 0.95257413) < 1e-6);
        CHECK(std::abs(transformed[3] - 0.98201379) < 1e-6);
        CHECK(std::abs(transformed[4] - 0.99330715) < 1e-6);
    }

    SECTION("reciprocal") {
        auto v = std::vector<double>{1, 2, 3, 4, 5};
        auto transformed = reciprocal(v);
        CHECK(std::abs(transformed[0] - 1.0) < 1e-6);
        CHECK(std::abs(transformed[1] - 0.5) < 1e-6);
        CHECK(std::abs(transformed[2] - 0.33333333) < 1e-6);
        CHECK(std::abs(transformed[3] - 0.25) < 1e-6);
        CHECK(std::abs(transformed[4] - 0.2) < 1e-6);
    }

    SECTION("sigmoid2") {
        auto transformed = sigmoid2(std::vector<double>{1, 2, 3, 4, 5});
        CHECK(std::abs(transformed[0] - 0.73105858) < 1e-6);
        CHECK(std::abs(transformed[1] - 0.88079708) < 1e-6);
        CHECK(std::abs(transformed[2] - 0.95257413) < 1e-6);
        CHECK(std::abs(transformed[3] - 0.98201379) < 1e-6);
        CHECK(std::abs(transformed[4] - 0.99330715) < 1e-6);
    }
}
#endif

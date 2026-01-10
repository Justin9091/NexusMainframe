//
// Created by jusra on 10-1-2026.
//

#include <catch2/catch_test_macros.hpp>

TEST_CASE("Placeholder test", "[placeholder]") {
    REQUIRE(true);

    SECTION("Basic arithmetic works") {
        REQUIRE(1 + 1 == 2);
        REQUIRE(2 * 2 == 4);
    }

    SECTION("String comparison works") {
        std::string hello = "Hello";
        REQUIRE(hello == "Hello");
        REQUIRE(hello.length() == 5);
    }
}

TEST_CASE("Another placeholder", "[placeholder]") {
    CHECK(42 == 42);
    CHECK_FALSE(1 == 2);
}
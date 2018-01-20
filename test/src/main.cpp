#define CATCH_CONFIG_MAIN
#include <catch.hpp>

TEST_CASE("dummy","[dummy]"){
    REQUIRE(1 == 1);
    REQUIRE(0 == 0);
}
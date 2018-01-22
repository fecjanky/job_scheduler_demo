#include <catch.hpp>

#include <simplified_dot_parser.h>

#include <sstream>
#include <string>

using namespace std::string_literals;
using namespace job_sheduler::utils;

TEST_CASE("dot parser on a invalid graphs", "[dot parser]")
{
    SECTION("missing start")
    {
        constexpr auto simple_dot = R"#(    "a" -> "b";
    "b" -> "c";
    })#";
        std::istringstream iss(simple_dot);
        REQUIRE_THROWS(parse_simplified_dot(iss));
    }
    SECTION("missing end")
    {
        constexpr auto simple_dot = R"#(digraph G {
    "a" -> "b";
    "b" -> "c";
    )#";
        std::istringstream iss(simple_dot);
        REQUIRE_THROWS(parse_simplified_dot(iss));
    }
    SECTION("invalid edges")
    {
        constexpr auto simple_dot = R"#(digraph G {
    "a" -> "b";
    "b" ;
    })#";
        std::istringstream iss(simple_dot);
        REQUIRE_THROWS(parse_simplified_dot(iss));
    }
}

TEST_CASE("dot parser on a edge cases", "[dot parser]")
{
    SECTION("empty graph")
    {
        constexpr auto simple_dot = R"#(digraph G {
    })#";
        std::istringstream iss(simple_dot);
        auto res = parse_simplified_dot(iss);
        REQUIRE(res.size() == 0);
    }

    SECTION("empty file")
    {
        std::istringstream iss{};
        auto res = parse_simplified_dot(iss);
        REQUIRE(res.size() == 0);
    }
}

TEST_CASE("dot parser on a simple graph", "[dot parser]")
{
    constexpr auto simple_dot = R"#(digraph G {
    "a" -> "b";
    "b" -> "c";
    })#";
    std::istringstream iss(simple_dot);
    auto res = parse_simplified_dot(iss);
    REQUIRE(res.size() == 2);
    auto & [ from_0, to_0 ] = res[0];
    auto & [ from_1, to_1 ] = res[1];
    REQUIRE(from_0 == "a");
    REQUIRE(to_0 == "b");
    REQUIRE(from_1 == "b");
    REQUIRE(to_1 == "c");
}

TEST_CASE("dot parser on the reference graph", "[dot parser]")
{
    constexpr auto simple_dot = R"#(

digraph G {
    "a" -> "g";

    "b" -> "c";
    "b" -> "d";


    "g" -> "h";
    "g" -> "i";
    "c" -> "e";
    "d" -> "e";
    "h" -> "j";
    "i" -> "j";
    "e" -> "f";
    "j" -> "f";

    }



)#";
    std::istringstream iss(simple_dot);
    auto res = parse_simplified_dot(iss);
    REQUIRE(res.size() == 11);
    auto & [ from_0, to_0 ] = res[0];
    auto & [ from_1, to_1 ] = res[1];
    auto & [ from_2, to_2 ] = res[2];
    auto & [ from_3, to_3 ] = res[3];
    auto & [ from_4, to_4 ] = res[4];
    auto & [ from_5, to_5 ] = res[5];
    auto & [ from_6, to_6 ] = res[6];
    auto & [ from_7, to_7 ] = res[7];
    auto & [ from_8, to_8 ] = res[8];
    auto & [ from_9, to_9 ] = res[9];
    auto & [ from_10, to_10 ] = res[10];
    REQUIRE((from_0 == "a" && to_0 == "g"));
    REQUIRE((from_1 == "b" && to_1 == "c"));
    REQUIRE((from_2 == "b" && to_2 == "d"));
    REQUIRE((from_3 == "g" && to_3 == "h"));
    REQUIRE((from_4 == "g" && to_4 == "i"));
    REQUIRE((from_5 == "c" && to_5 == "e"));
    REQUIRE((from_6 == "d" && to_6 == "e"));
    REQUIRE((from_7 == "h" && to_7 == "j"));
    REQUIRE((from_8 == "i" && to_8 == "j"));
    REQUIRE((from_9 == "e" && to_9 == "f"));
    REQUIRE((from_10 == "j" && to_10 == "f"));
}

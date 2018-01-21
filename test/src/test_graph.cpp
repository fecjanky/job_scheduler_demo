#include <catch.hpp>

#include <utility>

#include <job_graph.h>

using namespace job_sheduler;

auto create_test_graph()
{
    return make_graph([](auto&& e) { return e; },
        { std::make_pair(std::string("a"), std::string("b")),
            std::make_pair(std::string("b"), std::string("c")),
            std::make_pair(std::string("b"), std::string("d")) });
}

TEST_CASE("test graph construction", "[graph]")
{

    auto graph = create_test_graph();
    REQUIRE(graph.num_vertices() == 4);
    REQUIRE(graph.num_edges() == 3);
}

TEST_CASE("test find node in graph", "[graph]")
{
    auto graph = create_test_graph();
    REQUIRE(graph.find("d") != graph.end());
    REQUIRE(graph.find("s") == graph.end());
}

TEST_CASE(
    "job graph with no entry point (i.e. in degree is 0) can't be created",
    "[graph]")
{
    REQUIRE_THROWS(make_graph([](auto&& e) { return e; },
        { std::make_pair(std::string("a"), std::string("b")),
            std::make_pair(std::string("b"), std::string("a")) }));
}

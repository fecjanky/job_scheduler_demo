#include <catch.hpp>

#include <string>
#include <utility>

#include <job_graph.h>

using namespace std::literals;

using namespace job_sheduler;

auto create_test_graph()
{
    return make_graph({ std::make_pair("a"s, "b"s), std::make_pair("b"s, "c"s),
        std::make_pair("b"s, "d"s) });
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

TEST_CASE("job graph that has vertices is not done", "[graph]")
{
    auto graph = make_graph({ std::make_pair("a"s, "b"s) });
    REQUIRE(!graph.is_done());
}

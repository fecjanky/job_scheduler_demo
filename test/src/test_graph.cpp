#include <catch.hpp>

#include <utility>

#include <job_graph.h>

TEST_CASE("test graph construction", "[graph]")
{

    auto graph = job_sheduler::make_graph([](auto&& e) { return e; },
        { std::make_pair(std::string("a"), std::string("b")),
            std::make_pair(std::string("b"), std::string("c")),
            std::make_pair(std::string("b"), std::string("d")) });
    REQUIRE(graph.num_vertices() == 4);
    REQUIRE(graph.num_edges() == 3);
}
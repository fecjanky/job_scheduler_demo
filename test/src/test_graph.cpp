#include <catch.hpp>

#include <utility>

#include <job_graph.h>

TEST_CASE("test graph construction", "[graph]")
{

    auto graph = job_sheduler::make_graph([](auto&& e) {return e; }, { std::make_pair("a","b"),std::make_pair("b", "c") });
    int a = 0;
}
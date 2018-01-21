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

TEST_CASE(" shedule on a trivial graph a->b is a,b", "[graph]")
{
    auto graph = make_graph({ std::make_pair("a"s, "b"s) });
    auto sched_1 = graph.next_schedule();
    auto sched_2 = graph.next_schedule();
    REQUIRE(graph.is_done());
    REQUIRE(sched_1.size() == 1);
    REQUIRE(sched_2.size() == 1);
    REQUIRE(sched_1[0] == "a"s);
    REQUIRE(sched_2[0] == "b"s);
}

TEST_CASE("shedule on a diamond graph is a,b-c,d", "[graph]")
{
    auto graph
        = make_graph({ std::make_pair("a"s, "b"s), std::make_pair("a"s, "c"s),
            std::make_pair("b"s, "d"s), std::make_pair("c"s, "d"s) });
    auto sched_1 = graph.next_schedule();
    auto sched_2 = graph.next_schedule();
    auto sched_3 = graph.next_schedule();
    REQUIRE(graph.is_done());
    REQUIRE(sched_1.size() == 1);
    REQUIRE(sched_2.size() == 2);
    REQUIRE(sched_3.size() == 1);
    REQUIRE(sched_1[0] == "a"s);
    REQUIRE(std::find(sched_2.begin(), sched_2.end(), "b"s) != sched_2.end());
    REQUIRE(std::find(sched_2.begin(), sched_2.end(), "c"s) != sched_2.end());
    REQUIRE(sched_3[0] == "d"s);
}

TEST_CASE("shedule on the reference graph", "[graph]")
{
    auto graph = make_graph({ std::make_pair("a"s, "g"s),
        std::make_pair("b"s, "c"s), std::make_pair("b"s, "d"s),
        std::make_pair("g"s, "h"s), std::make_pair("g"s, "i"s),
        std::make_pair("c"s, "e"s), std::make_pair("d"s, "e"s),
        std::make_pair("h"s, "j"s), std::make_pair("i"s, "j"s),
        std::make_pair("e"s, "f"s), std::make_pair("j"s, "f"s) });
    auto sched = graph.get_full_schedule();
    REQUIRE(graph.is_done());
    REQUIRE(sched.size() == 5);
    REQUIRE(sched[0].size() == 2);
    REQUIRE(sched[1].size() == 3);
    REQUIRE(sched[2].size() == 3);
    REQUIRE(sched[3].size() == 1);
    REQUIRE(sched[4].size() == 1);
    REQUIRE(
        std::find(sched[0].begin(), sched[0].end(), "a"s) != sched[0].end());
    REQUIRE(
        std::find(sched[0].begin(), sched[0].end(), "b"s) != sched[0].end());
    REQUIRE(
        std::find(sched[1].begin(), sched[1].end(), "g"s) != sched[1].end());
    REQUIRE(
        std::find(sched[1].begin(), sched[1].end(), "c"s) != sched[1].end());
    REQUIRE(
        std::find(sched[1].begin(), sched[1].end(), "d"s) != sched[1].end());
    REQUIRE(
        std::find(sched[2].begin(), sched[2].end(), "e"s) != sched[2].end());
    REQUIRE(
        std::find(sched[2].begin(), sched[2].end(), "h"s) != sched[2].end());
    REQUIRE(
        std::find(sched[2].begin(), sched[2].end(), "i"s) != sched[2].end());
    REQUIRE(sched[3][0] == "j"s);
    REQUIRE(sched[4][0] == "f"s);
}

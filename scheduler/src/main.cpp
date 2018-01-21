#include <job_graph.h>
#include <string>
#include <utility>

using namespace std::literals;

int main()
{
    job_sheduler::make_graph({ std::make_pair("a"s, "b"s),
        std::make_pair("b"s, "c"s), std::make_pair("b"s, "d"s) });
    return 0;
}
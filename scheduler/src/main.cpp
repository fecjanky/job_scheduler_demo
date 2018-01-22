#include <fstream>
#include <iomanip>
#include <iostream>
#include <istream>
#include <memory>
#include <string>
#include <utility>

#include <job_graph.h>
#include <simplified_dot_parser.h>

using namespace std::literals;

std::pair<std::istream*, std::unique_ptr<std::ifstream>> get_input_stream(
    int argc, char* argv[])
{
    std::istream* is{};
    std::unique_ptr<std::ifstream> ifs;
    if (argc < 2) {
        is = &std::cin;
    }
    else if (argc == 2) {
        ifs = std::make_unique<std::ifstream>(argv[1]);
        is = ifs.get();
    }
    else {
        throw std::runtime_error("ivalid arguments...");
    }
    return std::make_pair(is, std::move(ifs));
}

void print_usage(std::ostream& os, int argc, char* argv[])
{
    os << "Usage: " << argv[0] << " [<filename>]" << '\n'
       << R"#(
 filename (optional) - if given reads from file, 
                       else from standard input)#"
       << '\n';
}

const auto print_schedule = [](std::ostream& os, const auto& schedule) {
    constexpr auto field_size = 25;
    os << std::left << std::setw(field_size) << "Depth" << ' '
       << std::setw(field_size) << "Independent vertices" << '\n'
       << std::setfill('=') << std::setw(2 * field_size) << '='
       << std::setfill(' ') << '\n';
    for (auto it = schedule.cbegin(); it != schedule.cend(); ++it) {
        os << std::setw(field_size) << std::distance(schedule.cbegin(), it) + 1;
        std::copy(it->begin(), it->end(),
            std::ostream_iterator<std::string>(os, ","));
        os << '\n';
    }

};

int main(int argc, char* argv[]) try {
    auto[is, ifs] = get_input_stream(argc, argv);
    auto edges = job_sheduler::utils::parse_simplified_dot(*is);
    auto graph = job_sheduler::make_graph(edges.cbegin(), edges.cend());
    auto schedule = graph.get_full_schedule();
    print_schedule(std::cout, schedule);
    return 0;
}
catch (const std::exception& e) {
    std::cerr << "Exception occured:" << e.what() << '\n';
    print_usage(std::cout, argc, argv);
    return -1;
}
catch (...) {
    std::cerr << "Unknown exception occured...\n";
    print_usage(std::cout, argc, argv);
    return -1;
}
#pragma once

#include <istream>
#include <regex>
#include <string>
#include <utility>

namespace job_sheduler {
namespace utils {

/// \brief parsing simplified dot file with format
/// \verbatim
///  digraph G{
///    "a" -> "b";
///    "b" -> "c";
///  }
/// \endverbatim
/// simplifications on file format: digraph opening statment followed
/// by edge listings one per line (use singele quote iside string literal
/// , terminated by closing bracket on a new line
inline auto parse_simplified_dot(std::istream& dot_text)
{

    static const std::regex digraph_start(R"#(\s*digraph\s+[^{]\s*\{\s*)#");
    static const std::regex digraph_edge(
        R"#(\s*"([^"]+)"\s*->\s*"([^"]+)"\s*;\s*)#");
    static const std::regex digraph_end(R"#(\s*\}\s*)#");
    static const std::regex digraph_empty(R"#(\s*)#");

    enum class parse_state {
        START,
        EDGES,
        END,
    } state
        = parse_state::START;

    std::vector<std::pair<std::string, std::string>> res;
    size_t i = 0;
    std::string line;
    while (dot_text && std::getline(dot_text, line)) {
        ++i;
        const auto throw_error = [i, &line]() {
            throw std::runtime_error(
                "dotfile error on line " + std::to_string(i) + ':' + line);
        };

        if (std::regex_match(line, digraph_empty))
            continue;
        switch (state) {
        case parse_state::START: {
            if (std::regex_match(line, digraph_start))
                state = parse_state::EDGES;
            else
                throw_error();
        } break;
        case parse_state::EDGES: {
            if (std::smatch match; std::regex_search(line, match, digraph_edge))
                res.emplace_back(match[1], match[2]);
            else if (std::regex_match(line, digraph_end))
                state = parse_state::END;
            else
                throw_error();
        } break;
        case parse_state::END:
            break;
        default:
            throw std::logic_error(
                "error in simplified dot parser state machine");
        };
    }
    if (i != 0 && state != parse_state::END) {
        throw std::runtime_error("error in simplified dot format");
    }
    return res;
} // namespace utils

} // namespace utils
} // namespace job_sheduler

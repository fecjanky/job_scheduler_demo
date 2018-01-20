#pragma once

#include <initializer_list>
#include <type_traits>
#include <utility>
#include <vector>

namespace job_sheduler {

namespace detail {
template <typename EdgeT, typename NodeF>
struct node_type {
public:
    using from_type = decltype(from_type_func());
    using to_type = decltype(to_type_func());
    using type = std::common_type_t<from_type, to_type>;

private:
    auto from_type_func()
    {
        auto && [ from, to ] = std::declval<NodeF>()(std::declval<EdgeT>());
        return from;
    }
    auto to_type_func()
    {
        auto && [ from, to ] = std::declval<NodeF>()(std::declval<EdgeT>());
        return to;
    }
};
} // namespace detail

template <typename EdgeT, typename NodeF>
class graph : private NodeF {
public:
    graph(NodeF node_func, std::initializer_list<EdgeT> edges)
        : NodeF(node_func)
    {
    }

private:
    const NodeF& node_function() const { return *this };
    NodeF& node_function() { return *this; };
};

template <typename EdgeT, typename NodeF>
auto make_graph(NodeF f, std::initializer_list<EdgeT> edges)
{
    return graph<EdgeT, NodeF>(std::move(f), std::move(edges));
}
} // namespace job_sheduler
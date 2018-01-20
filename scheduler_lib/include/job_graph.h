#pragma once

#include <initializer_list>
#include <vector>

namespace job_sheduler {

template <typename EdgeT, typename NodeF>
class graph : private NodeF {
public:
    graph(NodeF node_func, std::initializer_list<EdgeT> edges)
        : NodeF(node_func)
    {
        auto&&[from, to] = node_function()(*edges.begin());
    }

private:
    const NodeF& node_function() const { return *this };
    NodeF& node_function(){ return *this };
};
}
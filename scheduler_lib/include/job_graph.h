#pragma once

#include <algorithm>
#include <initializer_list>
#include <iterator>
#include <memory>
#include <numeric>
#include <stdexcept>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

namespace job_sheduler {

namespace detail {

template <typename T>
void unused_variable(T&&)
{
}

template <typename EdgeT, typename VertexF>
struct vertex_type {
private:
    using vertices = std::remove_reference_t<std::remove_cv_t<decltype(
        std::declval<VertexF>()(std::declval<EdgeT>()))>>;

    template <typename V>
    static auto from_type_func(V&& v)
    {
        auto[from, to] = v;
        unused_variable(to);
        return from;
    }
    template <typename V>
    static auto to_type_func(V&& v)
    {
        auto[from, to] = v;
        unused_variable(from);
        return to;
    }
    using from_type = decltype(from_type_func(std::declval<vertices>()));
    using to_type = decltype(to_type_func(std::declval<vertices>()));

public:
    using type = std::common_type_t<from_type, to_type>;
};

template <typename EdgeT, typename VertexF>
using vertex_type_t = typename vertex_type<EdgeT, VertexF>::type;
} // namespace detail

template <typename EdgeT, typename VertexF>
class graph {
public:
    using vertex_type = detail::vertex_type_t<EdgeT, VertexF>;
    using vertices_t = std::vector<vertex_type>;
    using vertex_const_iterator = typename vertices_t::const_iterator;
    struct vertex_ref;
    using vertex_ref_pointer = vertex_ref*;
    using vertex_ref_const_pointer = const vertex_ref*;
    using edges_t = std::vector<vertex_ref_pointer>;
    struct vertex_ref {
        explicit vertex_ref(vertex_const_iterator it)
            : vertex(it)
        {
        }
        vertex_const_iterator vertex;
        edges_t out;
        edges_t in;
    };
    using graph_t = std::vector<vertex_ref>;
    using graph_iterator = typename graph_t::iterator;
    using graph_const_iterator = typename graph_t::const_iterator;
    using view_t = std::vector<graph_iterator>;
    using view_iterator = typename view_t::iterator;
    using view_const_iterator = typename view_t::const_iterator;

    graph(VertexF vertex_func, const std::initializer_list<EdgeT>& edges);

    template <typename Iterator>
    graph(VertexF vertex_func, Iterator begin, Iterator end);

    graph(graph&&) = default;
    graph& operator=(graph&&) = default;

    graph(const graph&);
    graph& operator=(const graph&) = delete;
    ~graph() = default;

    view_const_iterator find(const vertex_type& key) const;

    view_const_iterator end() const;

    size_t num_vertices() const noexcept;
    size_t num_edges() const noexcept;

    std::pair<std::vector<vertex_type>, graph> next_schedule() const;

private:
    class impl;

    void init_from_impl();
    void restore_heap();

    void check_entry();
    static auto graph_compare()
    {
        return [](const auto& lhs, const auto& rhs) {
            return lhs->in.size() > rhs->in.size();
        };
    }

    /*************************************/
    std::shared_ptr<const impl> m_impl;
    graph_t m_graph;
    view_t m_view;
    /*************************************/
    /*Resource sharing of the Node object*/
    struct impl : private VertexF {
        using vertices_t = std::vector<vertex_type>;
        using vertex_const_iterator_t = typename vertices_t::const_iterator;
        using edges_to = std::vector<std::vector<vertex_const_iterator_t>>;

        template <typename Iterator>
        impl(VertexF vertex_func, Iterator begin, Iterator end);

        size_t num_vertices() const noexcept;
        const vertices_t& vertices() const noexcept;
        const edges_to& edges() const noexcept;

        template <typename Iterator>
        void init_vertices(Iterator begin, Iterator end);

        template <typename Iterator>
        void init_edges(Iterator begin, Iterator end);

        vertex_const_iterator_t find_vertex(const vertex_type& vertex);

        const VertexF& vertex_function() const;
        VertexF& vertex_function();

        /*********************/
        vertices_t m_vertices;
        edges_to m_edges;
    };

}; // namespace job_sheduler

template <typename EdgeT, typename VertexF>
auto make_graph(VertexF f, const std::initializer_list<EdgeT>& edges)
{
    return graph<EdgeT, VertexF>(std::move(f), edges);
}

template <typename EdgeT, typename VertexF>
template <typename Iterator>
inline graph<EdgeT, VertexF>::graph(
    VertexF vertex_func, Iterator begin, Iterator end)
    : m_impl(std::make_shared<const impl>(std::move(vertex_func), begin, end))
{
    init_from_impl();
}

template <typename EdgeT, typename VertexF>
inline graph<EdgeT, VertexF>::graph(
    VertexF vertex_func, const std::initializer_list<EdgeT>& edges)
    : graph(std::move(vertex_func), edges.begin(), edges.end())
{
}

template <typename EdgeT, typename VertexF>
inline graph<EdgeT, VertexF>::graph(const graph& other)
    : m_impl(other.m_impl)
{
    m_view.reserve(other.m_view.size());
    m_graph.reserve(other.m_view.size());
    // Filter nodes from view
    std::transform(other.m_view.begin(), other.m_view.end(),
        std::back_inserter(m_graph),
        [](const auto& v) { return vertex_ref(v->vertex); });
    // Filter edges based on filtered nodes
    for (auto& g : m_graph) {
        const auto& edges
            = m_impl->edges()[std::distance(m_impl->vertices(), g.vertex)];
    }
    // build representation
}

template <typename EdgeT, typename VertexF>
inline auto graph<EdgeT, VertexF>::find(const vertex_type& key) const
    -> view_const_iterator
{
    return std::find_if(m_view.begin(), m_view.end(),
        [&key](const auto& elem) { return *(elem->vertex) == key; });
}

template <typename EdgeT, typename VertexF>
inline auto graph<EdgeT, VertexF>::end() const -> view_const_iterator
{
    return m_view.end();
}

template <typename EdgeT, typename VertexF>
inline size_t graph<EdgeT, VertexF>::num_vertices() const noexcept
{
    return m_impl->num_vertices();
}

template <typename EdgeT, typename VertexF>
inline size_t graph<EdgeT, VertexF>::num_edges() const noexcept
{
    return std::accumulate(m_graph.begin(), m_graph.end(), size_t(0),
        [](auto accu, const auto& v) { return accu + v.out.size(); });
}

template <typename EdgeT, typename VertexF>
inline auto graph<EdgeT, VertexF>::next_schedule() const
    -> std::pair<std::vector<vertex_type>, graph>
{

    return std::pair<std::vector<vertex_type>, graph>();
}

template <typename EdgeT, typename VertexF>
inline void graph<EdgeT, VertexF>::init_from_impl()
{
    m_graph.clear();
    m_view.clear();
    const auto& impl_vertices = m_impl->vertices();
    const auto& impl_edges = m_impl->edges();
    m_graph.reserve(impl_vertices.size());
    /*init vertices references*/
    for (auto v_it = impl_vertices.begin(); v_it != impl_vertices.end();
         ++v_it) {
        m_graph.emplace_back(v_it);
    }
    /*init edges in graph*/
    for (auto edges = impl_edges.begin(); edges != impl_edges.end(); ++edges) {
        auto& from_vertex = m_graph[std::distance(impl_edges.begin(), edges)];
        from_vertex.out.reserve(edges->size());
        for (auto edge_it = edges->begin(); edge_it != edges->end();
             ++edge_it) {
            auto& to_vertex
                = m_graph[std::distance(impl_vertices.begin(), *edge_it)];
            from_vertex.out.push_back(std::addressof(to_vertex));
            to_vertex.in.push_back(std::addressof(from_vertex));
        }
    }
    /*init view*/
    m_view.reserve(m_graph.size());
    for (auto view_it = m_graph.begin(); view_it != m_graph.end(); ++view_it) {
        m_view.push_back(view_it);
    }

    restore_heap();
    check_entry();
}

template <typename EdgeT, typename VertexF>
inline void graph<EdgeT, VertexF>::restore_heap()
{
    std::make_heap(m_view.begin(), m_view.end(), graph_compare());
}

template <typename EdgeT, typename VertexF>
inline void graph<EdgeT, VertexF>::check_entry()
{
    if (!m_view.empty() && !m_view.front()->in.empty()) {
        throw std::runtime_error("no entry point in graph");
    }
}

template <typename EdgeT, typename VertexF>
template <typename Iterator>
inline graph<EdgeT, VertexF>::impl::impl(
    VertexF vertex_func, Iterator begin, Iterator end)
    : VertexF(std::move(vertex_func))
{
    init_vertices(begin, end);
    init_edges(begin, end);
}

template <typename EdgeT, typename VertexF>
template <typename Iterator>
inline void graph<EdgeT, VertexF>::impl::init_vertices(
    Iterator begin, Iterator end)
{
    std::for_each(begin, end, [this](const auto& edge) {
        const auto & [ from, to ] = vertex_function()(edge);
        if (std::find(m_vertices.begin(), m_vertices.end(), from)
            == m_vertices.end()) {
            m_vertices.push_back(from);
        }
        if (std::find(m_vertices.begin(), m_vertices.end(), to)
            == m_vertices.end()) {
            m_vertices.push_back(to);
        }
    });
    std::sort(m_vertices.begin(), m_vertices.end());
    m_edges.resize(m_vertices.size());
}

template <typename EdgeT, typename VertexF>
template <typename Iterator>
inline void graph<EdgeT, VertexF>::impl::init_edges(
    Iterator begin, Iterator end)
{
    std::for_each(begin, end, [this](const auto& edge) {
        const auto & [ from, to ] = vertex_function()(edge);
        const auto from_it = find_vertex(from);
        const auto to_it = find_vertex(to);
        m_edges[std::distance(m_vertices.cbegin(), from_it)].push_back(to_it);
    });
}

template <typename EdgeT, typename VertexF>
inline size_t graph<EdgeT, VertexF>::impl::num_vertices() const noexcept
{
    return m_vertices.size();
}

template <typename EdgeT, typename VertexF>
inline auto graph<EdgeT, VertexF>::impl::vertices() const noexcept
    -> const vertices_t&
{
    return m_vertices;
}

template <typename EdgeT, typename VertexF>
inline auto graph<EdgeT, VertexF>::impl::edges() const noexcept
    -> const edges_to&
{
    return m_edges;
}

template <typename EdgeT, typename VertexF>
inline auto graph<EdgeT, VertexF>::impl::find_vertex(const vertex_type& vertex)
    -> vertex_const_iterator_t
{
    auto it = std::lower_bound(m_vertices.cbegin(), m_vertices.cend(), vertex);
    if (it == m_vertices.cend() || *it < vertex) {
        throw std::logic_error("error during vertex initialization");
    }
    return it;
}

template <typename EdgeT, typename VertexF>
inline const VertexF& graph<EdgeT, VertexF>::impl::vertex_function() const
{
    return *this;
}
template <typename EdgeT, typename VertexF>
inline VertexF& graph<EdgeT, VertexF>::impl::vertex_function()
{
    return *this;
};

} // namespace job_sheduler
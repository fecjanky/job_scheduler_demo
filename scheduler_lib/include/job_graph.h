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

template <typename T> void unused_variable(T&&) {}

template <typename EdgeT, typename VertexF> struct vertex_type {
private:
    using vertices = std::remove_reference_t<std::remove_cv_t<decltype(
        std::declval<VertexF>()(std::declval<EdgeT>()))>>;

    template <typename V> static auto from_type_func(V&& v)
    {
        auto[from, to] = v;
        unused_variable(to);
        return from;
    }
    template <typename V> static auto to_type_func(V&& v)
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

template <typename EdgeT, typename VertexF> class graph {
public:
    using vertex_type = detail::vertex_type_t<EdgeT, VertexF>;

    graph(VertexF vertex_func, const std::initializer_list<EdgeT>& edges);

    template <typename Iterator>
    graph(VertexF vertex_func, Iterator begin, Iterator end);

    size_t num_vertices() const noexcept;
    size_t num_edges() const noexcept;

private:
    class impl;
    using vertices_t = std::vector<vertex_type>;
    using vertex_const_iterator_t = typename vertices_t::const_iterator;
    using edges_t = std::vector<vertex_const_iterator_t>;
    static constexpr size_t VERTEX = 0;
    static constexpr size_t EDGES_OUT = 1;
    static constexpr size_t EDGES_IN = 2;
    static constexpr size_t IMPL_EDGES_OUT = 0;
    static constexpr size_t IMPL_EDGES_IN = 1;

    std::vector<std::tuple<vertex_const_iterator_t, edges_t, edges_t>> m_graph;
    std::shared_ptr<const impl> m_impl;

    /*************************************/
    struct impl : private VertexF {
        using vertices_t = std::vector<vertex_type>;
        using vertex_const_iterator_t = typename vertices_t::const_iterator;
        using edges_t = std::vector<vertex_const_iterator_t>;
        using io_edges_t = std::vector<std::tuple<edges_t, edges_t>>;

        template <typename Iterator>
        impl(VertexF vertex_func, Iterator begin, Iterator end);

        size_t num_vertices() const noexcept;
        const vertices_t& vertices() const noexcept;
        const io_edges_t& edges() const noexcept;

        template <typename Iterator>
        void init_vertices(Iterator begin, Iterator end);

        template <typename Iterator>
        void init_edges(Iterator begin, Iterator end);

        vertex_const_iterator_t find_vertex(const vertex_type& vertex);

        const VertexF& vertex_function() const;
        VertexF& vertex_function();

        /*********************/
        vertices_t m_vertices;
        io_edges_t m_edges;
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
    m_graph.reserve(m_impl->num_vertices());
    for (auto it = m_impl->vertices().begin(); it != m_impl->vertices().end();
         ++it) {
        m_graph.emplace_back(it,
            std::get<IMPL_EDGES_OUT>(
                m_impl->edges()[std::distance(m_impl->vertices().begin(), it)]),
            std::get<IMPL_EDGES_IN>(m_impl->edges()[std::distance(
                m_impl->vertices().begin(), it)]));
    }
    std::make_heap(
        m_graph.begin(), m_graph.end(), [](const auto& lhs, const auto& rhs) {
            return !(std::get<EDGES_IN>(lhs).size()
                < std::get<EDGES_IN>(rhs).size());
        });
}

template <typename EdgeT, typename VertexF>
inline graph<EdgeT, VertexF>::graph(
    VertexF vertex_func, const std::initializer_list<EdgeT>& edges)
    : graph(std::move(vertex_func), edges.begin(), edges.end())
{
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
        [](auto accu, const auto& v) {
            return accu + std::get<EDGES_OUT>(v).size();
        });
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
        std::get<IMPL_EDGES_OUT>(
            m_edges[std::distance(m_vertices.cbegin(), from_it)])
            .push_back(to_it);
        std::get<IMPL_EDGES_IN>(
            m_edges[std::distance(m_vertices.cbegin(), to_it)])
            .push_back(from_it);
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
    -> const io_edges_t&
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
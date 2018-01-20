#pragma once

#include <algorithm>
#include <initializer_list>
#include <iterator>
#include <memory>
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
private:
    class impl;

public:
    using vertex_type = detail::vertex_type_t<EdgeT, VertexF>;
    class view;

    graph(VertexF vertex_func, const std::initializer_list<EdgeT>& edges)
        : graph(std::move(vertex_func), edges.begin(), edges.end())
    {
    }

    template <typename Iterator>
    graph(VertexF vertex_func, Iterator begin, Iterator end)
        : m_impl(std::make_shared<impl>(std::move(vertex_func), begin, end))
    {
    }

    size_t num_vertices() const noexcept { return m_impl->num_vertices(); }

    view as_view() const {}

private:
    std::shared_ptr<impl> m_impl;
    class impl : private VertexF {
    public:
        using vertices_t = std::vector<vertex_type>;
        using vertex_iterator_t = typename vertices_t::iterator;
        using vertex_const_iterator_t = typename vertices_t::const_iterator;
        using edges_t = std::vector<vertex_iterator_t>;
        static constexpr size_t EDGES_OUT = 0;
        static constexpr size_t EDGES_IN = 1;

        template <typename Iterator>
        impl(VertexF vertex_func, Iterator begin, Iterator end)
            : VertexF(std::move(vertex_func))
        {
            init_vertices(begin, end);
            init_edges(begin, end);
        }
        size_t num_vertices() const noexcept { return m_vertices.size(); }

    private:
        template <typename Iterator>
        void init_vertices(Iterator begin, Iterator end)
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

        template <typename Iterator>
        void init_edges(Iterator begin, Iterator end)
        {
            std::for_each(begin, end, [this](const auto& edge) {
                const auto & [ from, to ] = vertex_function()(edge);
                const auto from_it = find_vertex(from);
                const auto to_it = find_vertex(to);
                std::get<EDGES_OUT>(
                    m_edges[std::distance(m_vertices.begin(), from_it)])
                    .push_back(to_it);
                std::get<EDGES_IN>(
                    m_edges[std::distance(m_vertices.begin(), to_it)])
                    .push_back(from_it);
            });
        }

        vertex_iterator_t find_vertex(const vertex_type& vertex)
        {
            auto it = std::lower_bound(
                m_vertices.begin(), m_vertices.end(), vertex);
            if (it == m_vertices.end() || *it < vertex) {
                throw std::logic_error("error during vertex initialization");
            }
            return it;
        }

        const VertexF& vertex_function() const { return *this; };
        VertexF& vertex_function() { return *this; };

        /*********************/
        vertices_t m_vertices;
        std::vector<std::tuple<edges_t, edges_t>> m_edges;
    };

}; // namespace job_sheduler

template <typename EdgeT, typename VertexF>
auto make_graph(VertexF f, const std::initializer_list<EdgeT>& edges)
{
    return graph<EdgeT, VertexF>(std::move(f), edges);
}
} // namespace job_sheduler
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

template <typename VertexT>
class graph {
public:
    using vertex_type = VertexT;
    struct vertex;
    using vertex_ref_pointer = vertex*;
    using vertex_ref_const_pointer = const vertex*;
    using edges_t = std::vector<vertex_ref_pointer>;
    struct vertex {
        explicit vertex(vertex_type e)
            : elem(e)
        {
        }
        vertex_type elem;
        edges_t out;
        edges_t in;
    };
    using graph_t = std::vector<vertex>;
    using graph_iterator = typename graph_t::iterator;
    using graph_const_iterator = typename graph_t::const_iterator;
    using view_t = std::vector<graph_iterator>;
    using view_iterator = typename view_t::iterator;
    using view_const_iterator = typename view_t::const_iterator;

    template <typename VertexF, typename EdgeT>
    graph(VertexF vertex_func, const std::initializer_list<EdgeT>& edges);

    template <typename VertexF, typename Iterator>
    graph(VertexF vertex_func, Iterator begin, Iterator end);

    graph(graph&&) = default;
    graph& operator=(graph&&) = default;
    // TODO implement copy
    graph(const graph&) = delete;
    graph& operator=(const graph&) = delete;
    ~graph() = default;

    view_const_iterator find(const vertex_type& key) const;
    view_const_iterator end() const;

    size_t num_vertices() const noexcept;
    size_t num_edges() const noexcept;

    bool is_done() const noexcept;

private:
    void restore_heap();

    void add_vertex_unique(const vertex_type& v);
    template <typename VertexF, typename Iterator>
    void add_vertices(VertexF func, Iterator begin, Iterator end);
    template <typename VertexF, typename Iterator>
    void add_edges(VertexF func, Iterator begin, Iterator end);

    void add_view();

    vertex& find_vertex(const vertex_type& key);

    void check_entry();
    static auto graph_compare()
    {
        return [](const auto& lhs, const auto& rhs) {
            return lhs->in.size() > rhs->in.size();
        };
    }

    /*************************************/
    graph_t m_graph;
    view_t m_view;

}; // namespace job_sheduler

template <typename VertexF, typename Iterator>
auto make_graph(VertexF f, Iterator begin, Iterator end)
{
    using traits = std::iterator_traits<Iterator>;
    using EdgeT = typename traits::value_type;
    return graph<detail::vertex_type_t<EdgeT, VertexF>>(
        std::move(f), begin, end);
}

template <typename VertexF, typename EdgeT>
auto make_graph(VertexF f, const std::initializer_list<EdgeT>& edges)
{

    return graph<detail::vertex_type_t<EdgeT, VertexF>>(std::move(f), edges);
}

// default is identity function
template <typename EdgeT>
auto make_graph(const std::initializer_list<EdgeT>& edges)
{
    return make_graph([](const auto& e) { return e; }, edges);
}

template <typename Iterator>
auto make_graph(Iterator begin, Iterator end)
{
    return make_graph([](const auto& e) { return e; }, begin, end);
}

template <typename VertexT>
template <typename VertexF, typename Iterator>
inline graph<VertexT>::graph(VertexF vertex_func, Iterator begin, Iterator end)
{
    using traits = std::iterator_traits<Iterator>;
    using EdgeT = typename traits::value_type;
    using v_type = detail::vertex_type_t<EdgeT, VertexF>;
    static_assert(std::is_convertible_v<v_type, vertex_type>);
    add_vertices(vertex_func, begin, end);
    add_edges(vertex_func, begin, end);
    add_view();
    restore_heap();
    check_entry();
}

template <typename VertexT>
template <typename VertexF, typename Iterator>
inline void graph<VertexT>::add_vertices(
    VertexF vertex_function, Iterator begin, Iterator end)
{
    m_graph.reserve(std::distance(begin, end));
    for (; begin != end; ++begin) {
        const auto & [ from, to ] = vertex_function(*begin);
        add_vertex_unique(from);
        add_vertex_unique(to);
    };
    std::sort(m_graph.begin(), m_graph.end(),
        [](const auto& lhs, const auto& rhs) { return lhs.elem < rhs.elem; });
}

template <typename VertexT>
template <typename VertexF, typename Iterator>
inline void graph<VertexT>::add_edges(
    VertexF vertex_function, Iterator begin, Iterator end)
{
    for (; begin != end; ++begin) {
        const auto & [ from, to ] = vertex_function(*begin);
        auto& v_from = find_vertex(from);
        auto& v_to = find_vertex(to);
        v_from.out.push_back(std::addressof(v_to));
        v_to.in.push_back(std::addressof(v_from));
    }
}

template <typename VertexT>
template <typename VertexF, typename EdgeT>
inline graph<VertexT>::graph(
    VertexF vertex_func, const std::initializer_list<EdgeT>& edges)
    : graph(std::move(vertex_func), edges.begin(), edges.end())
{
}

template <typename VertexT>
inline auto graph<VertexT>::find(const vertex_type& key) const
    -> view_const_iterator
{
    return std::find_if(m_view.begin(), m_view.end(),
        [&key](const auto& elem) { return elem->elem == key; });
}

template <typename VertexT>
inline auto graph<VertexT>::end() const -> view_const_iterator
{
    return m_view.end();
}

template <typename VertexT>
inline size_t graph<VertexT>::num_vertices() const noexcept
{
    return m_view.size();
}

template <typename VertexT>
inline size_t graph<VertexT>::num_edges() const noexcept
{
    return std::accumulate(m_graph.begin(), m_graph.end(), size_t(0),
        [](auto accu, const auto& v) { return accu + v.out.size(); });
}

template <typename VertexT>
inline bool graph<VertexT>::is_done() const noexcept
{
    return m_view.empty();
}

template <typename VertexT>
inline void graph<VertexT>::restore_heap()
{
    std::make_heap(m_view.begin(), m_view.end(), graph_compare());
}

template <typename VertexT>
inline void graph<VertexT>::add_vertex_unique(const vertex_type& v)
{
    if (std::find_if(m_graph.begin(), m_graph.end(),
            [&v](const auto& g) { return v == g.elem; })
        == m_graph.end()) {
        m_graph.emplace_back(v);
    }
}

template <typename VertexT>
inline void graph<VertexT>::add_view()
{
    m_view.reserve(m_graph.size());
    for (auto it = m_graph.begin(); it != m_graph.end(); ++it) {
        m_view.push_back(it);
    }
}

template <typename VertexT>
inline auto graph<VertexT>::find_vertex(const vertex_type& key) -> vertex&
{
    auto it = std::lower_bound(m_graph.begin(), m_graph.end(), key,
        [](const auto& v, const auto& k) { return v.elem < k; });
    if (it == m_graph.end() || it->elem < key) {
        throw std::logic_error("error during vertex initialization");
    }
    return *it;
}

template <typename VertexT>
inline void graph<VertexT>::check_entry()
{
    if (!m_view.empty() && !m_view.front()->in.empty()) {
        throw std::runtime_error("no entry point in graph");
    }
}

} // namespace job_sheduler
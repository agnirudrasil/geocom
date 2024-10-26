//
// Created by Agnirudra Sil on 10/18/24.
//

#ifndef GRAPH_H
#define GRAPH_H
#include <unordered_map>
#include <unordered_set>

enum class graph_type { DIRECTED, UNDIRECTED };

struct pairhash {
    template<typename T, typename U>
    std::size_t operator()(const std::pair<T, U> &x) const {
        return std::hash<T>()(x.first) ^ std::hash<U>()(x.second);
    }
};

template<typename VERTEX_T, typename EDGE_T, graph_type GRAPH_TYPE, typename VERTEX_ID_T = int>
class graph {
    using vertex_id_t = VERTEX_ID_T;
    using vertex_t = VERTEX_T;
    using vertices_t = std::unordered_map<vertex_id_t, vertex_t>;
    using vertices_set_t = std::unordered_set<vertex_id_t>;

    using edge_id_t = std::pair<vertex_id_t, vertex_id_t>;
    using edge_t = EDGE_T;
    using edges_t = std::unordered_map<edge_id_t, edge_t, pairhash>;

public:
    graph() = default;

    [[nodiscard]] static constexpr bool is_directed() {
        return GRAPH_TYPE == graph_type::DIRECTED;
    }

    [[nodiscard]] static constexpr bool is_undirected() {
        return GRAPH_TYPE == graph_type::UNDIRECTED;
    }

    [[nodiscard]] size_t vertex_count() const noexcept;

    [[nodiscard]] size_t edge_count() const noexcept;

    vertices_t &get_vertices() noexcept {
        return vertices;
    }

    edges_t &get_edges() const noexcept {
        return edges;
    }

    [[nodiscard]] bool contains(vertex_id_t &vertex_id) const noexcept;

    [[nodiscard]] bool is_adjacent(const vertex_id_t &lhs, const vertex_id_t &rhs) const noexcept;

    vertex_t &get_vertex(const vertex_id_t &vertex_id) {
        return vertices.at(vertex_id);
    }

    edge_t &get_edge(const vertex_id_t &lhs, const vertex_id_t &rhs);

    edge_t &get_edge(const edge_id_t &edge_id);

    vertices_set_t get_neighbours(vertex_id_t &vertex_id);

    void add_vertex(vertex_id_t &vertex_id, auto &&vertex);

    void remove_vertex(vertex_id_t &vertex_id);

    void add_edge(vertex_id_t &vertex_id_lhs, vertex_id_t &vertex_id_rhs, auto &&edge);

    void remove_edge(vertex_id_t &lhs, vertex_id_t &rhs);

private:
    vertices_t vertices{};
    edges_t edges{};
    std::unordered_map<vertex_id_t, vertices_set_t> adj_list{};

    std::pair<vertex_id_t, vertex_id_t> get_sorted_id(vertex_id_t lhs, vertex_id_t rhs) const {
        if (lhs < rhs) {
            return std::make_pair(lhs, rhs);
        }
        return std::make_pair(rhs, lhs);
    }

    std::pair<vertex_id_t, vertex_id_t> get_edge_id(vertex_id_t lhs, vertex_id_t rhs) const {
        if constexpr (GRAPH_TYPE == graph_type::UNDIRECTED) {
            return get_sorted_id(lhs, rhs);
        } else if constexpr (GRAPH_TYPE == graph_type::DIRECTED) {
            return std::make_pair(lhs, rhs);
        }

        std::abort();
    }
};


#include "graph.tpp"
#endif //GRAPH_H

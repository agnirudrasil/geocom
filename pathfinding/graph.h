//
// Created by Agnirudra Sil on 10/18/24.
//

#ifndef GRAPH_H
#define GRAPH_H
#include <unordered_map>
#include <unordered_set>

/**
 * @enum graph_type
 * @brief Enum to represent the type of the graph (DIRECTED or UNDIRECTED).
 */
enum class graph_type {
    DIRECTED, ///< Directed graph type.
    UNDIRECTED ///< Undirected graph type.
};

/**
 * @struct pairhash
 * @brief A custom hash function for hashing std::pair objects.
 */
struct pairhash {
    /**
     * @brief Operator for hashing a pair.
     *
     * @tparam T Type of the first element in the pair.
     * @tparam U Type of the second element in the pair.
     * @param x The pair to hash.
     * @return The hash value of the pair.
     */
    template<typename T, typename U>
    std::size_t operator()(const std::pair<T, U> &x) const {
        return std::hash<T>()(x.first) ^ std::hash<U>()(x.second);
    }
};

/**
 * @class graph
 * @brief A template class for creating and managing a graph structure.
 *
 * @tparam VERTEX_T The type of the vertices in the graph.
 * @tparam EDGE_T The type of the edges in the graph.
 * @tparam GRAPH_TYPE The type of the graph (DIRECTED or UNDIRECTED).
 * @tparam VERTEX_ID_T The type used to represent vertex IDs (defaults to int).
 */
template<typename VERTEX_T, typename EDGE_T, graph_type GRAPH_TYPE, typename VERTEX_ID_T = int>
class graph {
    using vertex_id_t = VERTEX_ID_T; ///< Type alias for vertex ID.
    using vertex_t = VERTEX_T; ///< Type alias for a vertex.
    using vertices_t = std::unordered_map<vertex_id_t, vertex_t>; ///< Type alias for the vertices container.
    using vertices_set_t = std::unordered_set<vertex_id_t>; ///< Type alias for a set of vertex IDs.
    using edge_id_t = std::pair<vertex_id_t, vertex_id_t>; ///< Type alias for an edge ID.
    using edge_t = EDGE_T; ///< Type alias for an edge.
    using edges_t = std::unordered_map<edge_id_t, edge_t, pairhash>; ///< Type alias for the edges container.

public:
    /**
     * @brief Default constructor.
     */
    graph() = default;

    /**
     * @brief Checks if the graph is directed.
     * @return True if the graph is directed, false otherwise.
     */
    [[nodiscard]] static constexpr bool is_directed() {
        return GRAPH_TYPE == graph_type::DIRECTED;
    }

    /**
     * @brief Checks if the graph is undirected.
     * @return True if the graph is undirected, false otherwise.
     */
    [[nodiscard]] static constexpr bool is_undirected() {
        return GRAPH_TYPE == graph_type::UNDIRECTED;
    }

    /**
     * @brief Gets the number of vertices in the graph.
     * @return The number of vertices.
     */
    [[nodiscard]] size_t vertex_count() const noexcept;

    /**
     * @brief Gets the number of edges in the graph.
     * @return The number of edges.
     */
    [[nodiscard]] size_t edge_count() const noexcept;

    /**
     * @brief Gets a reference to the container of vertices.
     * @return A reference to the vertices container.
     */
    vertices_t &get_vertices() noexcept;

    /**
     * @brief Gets a reference to the container of edges.
     * @return A reference to the edges container.
     */
    edges_t &get_edges() const noexcept;

    /**
     * @brief Checks if a vertex exists in the graph.
     * @param vertex_id The ID of the vertex to check.
     * @return True if the vertex exists, false otherwise.
     */
    [[nodiscard]] bool contains(vertex_id_t &vertex_id) const noexcept;

    /**
     * @brief Checks if two vertices are adjacent.
     * @param lhs The ID of the first vertex.
     * @param rhs The ID of the second vertex.
     * @return True if the vertices are adjacent, false otherwise.
     */
    [[nodiscard]] bool is_adjacent(const vertex_id_t &lhs, const vertex_id_t &rhs) const noexcept;

    /**
     * @brief Gets a vertex by its ID.
     * @param vertex_id The ID of the vertex.
     * @return A reference to the vertex.
     * @throws std::out_of_range if the vertex does not exist.
     */
    vertex_t &get_vertex(const vertex_id_t &vertex_id);

    /**
     * @brief Gets an edge by its vertices' IDs.
     * @param lhs The ID of the starting vertex.
     * @param rhs The ID of the ending vertex.
     * @return A reference to the edge.
     * @throws std::out_of_range if the edge does not exist.
     */
    edge_t &get_edge(const vertex_id_t &lhs, const vertex_id_t &rhs);

    /**
     * @brief Gets an edge by its ID.
     * @param edge_id The ID of the edge.
     * @return A reference to the edge.
     * @throws std::out_of_range if the edge does not exist.
     */
    edge_t &get_edge(const edge_id_t &edge_id);

    /**
     * @brief Gets the neighboring vertices of a vertex.
     * @param vertex_id The ID of the vertex.
     * @return A set of IDs of neighboring vertices.
     */
    vertices_set_t get_neighbours(vertex_id_t &vertex_id);

    /**
     * @brief Adds a vertex to the graph.
     * @param vertex_id The ID of the vertex.
     * @param vertex The vertex object.
     */
    void add_vertex(vertex_id_t &vertex_id, auto &&vertex);

    /**
     * @brief Removes a vertex from the graph.
     * @param vertex_id The ID of the vertex to remove.
     */
    void remove_vertex(vertex_id_t &vertex_id);

    /**
     * @brief Adds an edge between two vertices.
     * @param vertex_id_lhs The ID of the starting vertex.
     * @param vertex_id_rhs The ID of the ending vertex.
     * @param edge The edge object.
     */
    void add_edge(vertex_id_t &vertex_id_lhs, vertex_id_t &vertex_id_rhs, auto &&edge);

    /**
     * @brief Removes an edge between two vertices.
     * @param lhs The ID of the starting vertex.
     * @param rhs The ID of the ending vertex.
     */
    void remove_edge(vertex_id_t &lhs, vertex_id_t &rhs);

private:
    vertices_t vertices{}; ///< Container for the vertices.
    edges_t edges{}; ///< Container for the edges.
    std::unordered_map<vertex_id_t, vertices_set_t> adj_list{}; ///< Adjacency list for the graph.

    /**
     * @brief Helper function to get a sorted edge ID for undirected graphs.
     * @param lhs The first vertex ID.
     * @param rhs The second vertex ID.
     * @return A pair representing the sorted edge ID.
     */
    std::pair<vertex_id_t, vertex_id_t> get_sorted_id(vertex_id_t lhs, vertex_id_t rhs) const;

    /**
     * @brief Helper function to get an edge ID based on graph type.
     * @param lhs The starting vertex ID.
     * @param rhs The ending vertex ID.
     * @return A pair representing the edge ID.
     */
    std::pair<vertex_id_t, vertex_id_t> get_edge_id(vertex_id_t lhs, vertex_id_t rhs) const;
};

#include "graph.tpp"
#endif // GRAPH_H

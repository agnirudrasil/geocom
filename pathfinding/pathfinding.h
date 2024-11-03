//
// Created by Agnirudra Sil on 10/18/24.
//

#ifndef PATHFINDING_HPP
#define PATHFINDING_HPP
#include <utility>
#include <vector>
#include <unordered_map>
#include <string>

#include "edge.h"
#include "graph.h"
#include "node.h"
#include "node_visitor.h"

/**
 * @brief A class template for implementing pathfinding algorithms on a graph.
 *
 * @tparam G_COST The type of the function or functor used to calculate the actual cost (g-cost).
 * @tparam H_COST The type of the function or functor used to estimate the heuristic cost (h-cost).
 */
template<typename G_COST, typename H_COST>
class pathfinding {
    using graph_t = graph<node, edge, graph_type::UNDIRECTED, std::string>; /**< The type of graph used in pathfinding. */
    using path_t = std::vector<std::vector<double>>; /**< The type representing a path as a collection of coordinates. */

public:
    /**
     * @brief Constructs a pathfinding object using a reference to a graph.
     *
     * @param g The graph on which pathfinding will be performed.
     */
    explicit pathfinding(graph_t &g) : g{&g} {
    }

    /**
     * @brief Finds a path between a start and an end node.
     *
     * @param start The starting node of the path.
     * @param end The destination node of the path.
     * @return A string representing the path found.
     */
    std::string find_path(node &start, const node &end);

private:
    graph_t *g; /**< A pointer to the graph on which pathfinding is performed. */
    std::unordered_map<std::string, node_visitor> costs{}; /**< A map storing node visit information and costs. */
    G_COST g_cost; /**< The actual cost function or functor. */
    H_COST h_cost; /**< The heuristic cost function or functor. */

    /**
     * @brief Retraces the path from the end node back to the start node.
     *
     * @param start The starting node.
     * @param end The ending node.
     * @return A path represented as a collection of coordinates.
     */
    [[nodiscard]] path_t retrace_path(const node &start, const node &end) const;
};

#include "pathfinding.tpp"
#endif // PATHFINDING_HPP

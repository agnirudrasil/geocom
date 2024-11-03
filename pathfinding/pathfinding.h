//
// Created by Agnirudra Sil on 10/18/24.
//

#ifndef PATHFINDING_HPP
#define PATHFINDING_HPP
#include <utility>

#include "edge.h"
#include "graph.h"
#include "node.h"
#include <vector>

#include "node_visitor.h"

template<typename G_COST, typename H_COST>
class pathfinding {
    using graph_t = graph<node, edge, graph_type::UNDIRECTED, std::string>;
    using path_t = std::vector<std::vector<double> >;

public:
    explicit pathfinding(graph_t &g) : g{&g} {
    }

    std::string find_path(node &start, const node &end);

private:
    graph_t *g;
    std::unordered_map<std::string, node_visitor> costs{};
    G_COST g_cost{};
    H_COST h_cost{};

    [[nodiscard]] path_t retrace_path(const node &start, const node &end) const;
};

#include "pathfinding.tpp"
#endif // PATHFINDING_HPP

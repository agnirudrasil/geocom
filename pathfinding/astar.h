//
// Created by Agnirudra Sil on 10/18/24.
//

#ifndef ASTAR_H
#define ASTAR_H
#include <utility>

#include "edge.h"
#include "graph.h"
#include "node.h"
#include <vector>

class astar {
    using graph_t = graph<node, edge, graph_type::UNDIRECTED, std::string>;
    using path_t = std::vector<std::vector<double> >;

public:
    explicit astar(graph_t &g) : g{&g} {
    }

    std::string find_path(node &start, node &end) const;

private:
    graph_t *g;

    [[nodiscard]] static path_t retrace_path(node &start, node &end);
};

#endif // ASTAR_H

//
// Created by Agnirudra Sil on 10/18/24.
//

#ifndef ASTAR_H
#define ASTAR_H
#include <utility>

#include "edge.h"
#include "graph.h"
#include "node.h"


class astar {
    using graph = graph<node, edge, graph_type::UNDIRECTED, std::string>;

public:
    astar(std::string &start_id, std::string &end_id, graph &g): g{&g} {
        this->start = &g.get_vertex(start_id);
        this->end = &g.get_vertex(end_id);
    }

    std::vector<std::pair<double, double> > begin();

private:
    graph *g;
    node *start;
    node *end;

    [[nodiscard]] std::vector<std::pair<double, double> > retrace_path() const;
};


#endif //ASTAR_H

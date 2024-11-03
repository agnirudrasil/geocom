//
// Created by Agnirudra Sil on 11/3/24.
//

#ifndef NODE_VISITOR_H
#define NODE_VISITOR_H
#include <utility>

#include "node.h"


class node_visitor {
public:
    double g_cost = 0.0;
    double h_cost = 0.0;
    node *parent = nullptr;
    node *n = nullptr;

    explicit node_visitor(node *n) : n(n) {
    }

    [[nodiscard]] double f_cost() const { return g_cost + h_cost; }
};


#endif //NODE_VISITOR_H

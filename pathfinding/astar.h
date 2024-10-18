//
// Created by Agnirudra Sil on 10/18/24.
//

#ifndef ASTAR_H
#define ASTAR_H
#include "node.h"


class astar {
    node start;
    node end;

    std::vector<node> retrace_path();

public:
    astar(node start, node end): start{std::move(start)}, end{std::move(end)} {
    }

    std::vector<node> begin();
};


#endif //ASTAR_H

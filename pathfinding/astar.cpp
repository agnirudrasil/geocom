//
// Created by Agnirudra Sil on 10/18/24.
//

#include "astar.h"

#include <unordered_set>

#include "heap.h"

std::vector<node> astar::retrace_path() {
    auto path = {this->start, this->end};

    return path;
}

std::vector<node> astar::begin() {
    heap<node> heap;
    auto path = {this->start, this->end};

    return path;
}

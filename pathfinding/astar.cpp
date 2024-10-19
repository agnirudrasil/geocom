//
// Created by Agnirudra Sil on 10/18/24.
//

#include "astar.h"

#include <unordered_set>
#include <numbers>
#include "heap.h"

#define R 6371

long double deg2rad(const double deg) {
    return deg * (std::numbers::pi_v<double> / 180);
}

long double distance(const node *node1, const node *node2) {
    return sqrt(pow(node1->lat - node2->lat, 2) + pow(node1->lon - node2->lon, 2));
}

std::vector<std::pair<double, double> > astar::retrace_path() const {
    std::vector<std::pair<double, double> > path;
    auto current = end;

    while (current != start) {
        path.emplace_back(current->lat, current->lon);
        current = current->parent;
    }

    path.emplace_back(start->lat, start->lon);

    return path;
}

std::vector<std::pair<double, double> > astar::begin() {
    heap<node *, CompareNode> heap;
    std::unordered_set<std::string> open;
    std::unordered_set<std::string> closed;

    open.insert(start->id);
    heap.push(start);

    while (!heap.empty()) {
        const auto current = heap.pop();
        closed.insert(current->id);

        if (current->id == end->id) {
            return retrace_path();
        }

        for (auto &neighbour: g->get_neighbours(current->id)) {
            if (closed.contains(neighbour)) continue;

            auto neighbour_node = &g->get_vertex(neighbour);

            if (const long double movement_cost = current->g_cost + distance(current, neighbour_node);
                movement_cost < neighbour_node->g_cost or !open.contains(neighbour_node->id)) {
                neighbour_node->g_cost = movement_cost;
                neighbour_node->h_cost = distance(neighbour_node, end);
                neighbour_node->parent = &g->get_vertex(current->id);

                if (!open.contains(neighbour_node->id)) {
                    open.insert(neighbour_node->id);
                    heap.push(neighbour_node);
                } else {
                    heap.heapify();
                }
            }
        }
    }
}

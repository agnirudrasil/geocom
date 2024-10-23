//
// Created by Agnirudra Sil on 10/18/24.
//

#include "astar.h"

#include <unordered_set>
#include <numbers>
#include "heap.h"

#define R 6371

double deg2rad(const double deg) {
    return deg * (std::numbers::pi_v<double> / 180);
}

double distance(const node *node1, const node *node2) {
    const auto d_lat = deg2rad(node2->lat - node1->lat);
    const auto d_lon = deg2rad(node2->lon - node1->lon);

    const auto a = sin(d_lat / 2) * sin(d_lon / 2) +
                   cos(deg2rad(node1->lat)) * cos(deg2rad(node2->lat)) * sin(d_lon / 2) * sin(d_lon / 2);

    const auto c = 2 * atan2(sqrt(a), sqrt(1 - a));

    return R * c;
}

struct CompareNode {
    bool operator()(const node *l, const node *r) const {
        return l->f_cost() < r->f_cost();
    }
};

std::optional<astar::path_t> astar::find_path(node &start, node &end) const {
    heap<node *, CompareNode> heap;
    std::unordered_set<std::string> open;
    std::unordered_set<std::string> closed;

    open.insert(start.id);
    heap.push(&start);

    while (!heap.empty()) {
        const auto current = heap.pop();
        open.erase(current->id);
        closed.insert(current->id);

        if (current->id == end.id) return retrace_path(start, end);

        for (auto &neighbour: g->get_neighbours(current->id)) {
            if (closed.contains(neighbour)) continue;

            auto neighbour_node = &g->get_vertex(neighbour);

            if (const double movement_cost = current->g_cost + distance(current, neighbour_node);
                movement_cost < neighbour_node->g_cost or !open.contains(neighbour_node->id)) {
                neighbour_node->g_cost = movement_cost;
                neighbour_node->h_cost = distance(neighbour_node, &end);
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

    return {};
}

astar::path_t astar::retrace_path(node &start, node &end) {
    std::vector<std::pair<double, double> > path;
    auto current = &end;

    while (current->id != start.id) {
        path.emplace_back(current->lat, current->lon);
        current = current->parent;
    }

    std::ranges::reverse(path);
    return path;
}

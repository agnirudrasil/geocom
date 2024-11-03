#pragma once

#include <unordered_set>
#include <nlohmann/json.hpp>
#include "pathfinding.h"
#include "heap.h"
#include "utils.h"

using json = nlohmann::json;

struct CompareNode {
    bool operator()(const node_visitor &l, const node_visitor &r) const {
        return l.f_cost() < r.f_cost();
    }
};

template<typename G_COST, typename H_COST>
std::string pathfinding<G_COST, H_COST>::find_path(node &start, const node &end) {
    heap<node_visitor, CompareNode> heap;
    std::unordered_set<std::string> open;
    std::unordered_set<std::string> closed;

    node_visitor start_visitor(&g->get_vertex(start.id));
    costs.insert({start.id, start_visitor});
    open.insert(start.id);
    heap.push(start_visitor);


    json animation;
    animation["step_count"] = 0;
    animation["steps"] = json::array();

    while (!heap.empty()) {
        const auto current = heap.pop();
        open.erase(current.n->id);
        closed.insert(current.n->id);

        json step;
        animation["step_count"] = animation["step_count"].get<int>() + 1;
        step["parent"] = get_lat_lon(current.n);
        step["children"] = json::array();

        if (current.n->id == end.id) {
            auto path = retrace_path(g->get_vertex(start.id), g->get_vertex(end.id));
            costs.clear();

            animation["path"] = path;
            return animation.dump();
        }

        for (const auto &neighbour: g->get_neighbours(current.n->id)) {
            if (closed.contains(neighbour))
                continue;

            auto neighbour_node = &g->get_vertex(neighbour);
            if (!costs.contains(neighbour)) {
                node_visitor neighbour_cost(neighbour_node);
                costs.insert({neighbour, neighbour_cost});
            }
            const auto neighbour_cost = &costs.at(neighbour);

            if (const double movement_cost = current.g_cost + g_cost(current.n, neighbour_node);
                movement_cost < neighbour_cost->g_cost or !open.contains(neighbour_node->id)) {
                neighbour_cost->g_cost = movement_cost;
                neighbour_cost->h_cost = h_cost(neighbour_node, &end);
                neighbour_cost->parent = &g->get_vertex(current.n->id);

                if (!open.contains(neighbour_node->id)) {
                    open.insert(neighbour_node->id);
                    heap.push(costs.at(neighbour));

                    step["children"].push_back(get_lat_lon(neighbour_node));
                }
            }
        }
        heap.heapify();

        animation["steps"].push_back(step);
    }

    costs.clear();
    return animation.dump();
}

template<typename G_COST, typename H_COST>
typename pathfinding<G_COST, H_COST>::path_t pathfinding<G_COST, H_COST>::retrace_path(
    const node &start, const node &end) const {
    std::vector<std::vector<double> > path;
    auto current = costs.at(end.id);
    path.push_back(get_lat_lon(current.n));

    while (current.n->id != start.id) {
        current = costs.at(current.parent->id);
        path.push_back(get_lat_lon(current.n));
    }

    return path;
}

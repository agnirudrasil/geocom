//
// Created by Agnirudra Sil on 10/18/24.
//

#include "astar.h"

#include <unordered_set>
#include "heap.h"
#include "utils.h"
#include <nlohmann/json.hpp>

using json = nlohmann::json;

struct CompareNode {
    bool operator()(const node *l, const node *r) const {
        return l->f_cost() < r->f_cost();
    }
};

std::string astar::find_path(node &start, node &end) const {
    heap<node *, CompareNode> heap;
    std::unordered_set<std::string> open;
    std::unordered_set<std::string> closed;

    open.insert(start.id);
    heap.push(&start);
    json animation;
    animation["step_count"] = 0;
    animation["steps"] = json::array();

    while (!heap.empty()) {
        const auto current = heap.pop();
        open.erase(current->id);
        closed.insert(current->id);
        json step;

        animation["step_count"] = animation["step_count"].get<int>() + 1;
        step["parent"] = get_lat_lon(current);
        step["children"] = json::array();

        if (current->id == end.id) {
            auto path = retrace_path(g->get_vertex(start.id), g->get_vertex(end.id));
            animation["path"] = path;
            return animation.dump();
        }

        for (auto &neighbour: g->get_neighbours(current->id)) {
            if (closed.contains(neighbour))
                continue;

            auto neighbour_node = &g->get_vertex(neighbour);

            if (const double movement_cost = current->g_cost + distance(current, neighbour_node);
                movement_cost < neighbour_node->g_cost or !open.contains(neighbour_node->id)) {
                neighbour_node->g_cost = movement_cost;
                // neighbour_node->h_cost = distance(neighbour_node, &end);
                neighbour_node->h_cost = 0;
                neighbour_node->parent = &g->get_vertex(current->id);

                if (!open.contains(neighbour_node->id)) {
                    step["children"].push_back(get_lat_lon(neighbour_node));
                    open.insert(neighbour_node->id);
                    heap.push(neighbour_node);
                }
            }
        }
        heap.heapify();
        animation["steps"].push_back(step);
    }

    return {};
}

astar::path_t astar::retrace_path(node &start, node &end) {
    std::vector<std::vector<double> > path;
    auto current = &end;
    path.push_back(get_lat_lon(current));

    while (current->id != start.id) {
        current = current->parent;
        path.push_back(get_lat_lon(current));
    }

    // std::ranges::views::reverse(path);
    return path;
}

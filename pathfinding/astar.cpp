//
// Created by Agnirudra Sil on 10/18/24.
//

#include "astar.h"

#include <unordered_set>
#include <numbers>
#include "heap.h"
#include "utils.h"

struct CompareNode
{
    bool operator()(const node *l, const node *r) const
    {
        return l->f_cost() < r->f_cost();
    }
};

std::optional<astar::path_t> astar::find_path(node &start, node &end) const
{
    heap<node *, CompareNode> heap;
    std::unordered_set<std::string> open;
    std::unordered_set<std::string> closed;

    open.insert(start.id);
    heap.push(&start);

    while (!heap.empty())
    {
        const auto current = heap.pop();
        open.erase(current->id);
        closed.insert(current->id);

        if (current->id == end.id)
            return retrace_path(g->get_vertex(start.id), g->get_vertex(end.id));

        for (auto &neighbour : g->get_neighbours(current->id))
        {
            if (closed.contains(neighbour))
                continue;

            auto neighbour_node = &g->get_vertex(neighbour);

            if (const double movement_cost = current->g_cost + distance(current, neighbour_node);
                movement_cost < neighbour_node->g_cost or !open.contains(neighbour_node->id))
            {
                neighbour_node->g_cost = movement_cost;
                neighbour_node->h_cost = distance(neighbour_node, &end);
                neighbour_node->parent = &g->get_vertex(current->id);

                if (!open.contains(neighbour_node->id))
                {
                    open.insert(neighbour_node->id);
                    heap.push(neighbour_node);
                }
                else
                {
                    heap.heapify();
                }
            }
        }
    }

    return {};
}

astar::path_t astar::retrace_path(node &start, node &end)
{
    std::vector<std::pair<double, double>> path;
    auto current = &end;
    path.emplace_back(current->lat, current->lon);

    while (current->id != start.id)
    {
        current = current->parent;
        path.emplace_back(current->lat, current->lon);
    }

    // std::ranges::views::reverse(path);
    return path;
}

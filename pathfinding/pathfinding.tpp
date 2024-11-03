#pragma once

#include <unordered_set>
#include <nlohmann/json.hpp>
#include "pathfinding.h"
#include "heap.h"
#include "utils.h"

using json = nlohmann::json;

/**
 * @brief Functor used to compare nodes based on their f-costs.
 */
struct CompareNode {
    /**
     * @brief Compares two node_visitor objects.
     *
     * @param l The left-hand node_visitor.
     * @param r The right-hand node_visitor.
     * @return True if the f-cost of `l` is less than that of `r`.
     */
    bool operator()(const node_visitor &l, const node_visitor &r) const {
        return l.f_cost() < r.f_cost();
    }
};

/**
 * @brief Finds a path from the start node to the end node using pathfinding algorithms.
 *
 * @tparam G_COST The type of the g-cost function or functor.
 * @tparam H_COST The type of the h-cost (heuristic) function or functor.
 * @param start The starting node for the pathfinding.
 * @param end The ending node for the pathfinding.
 * @return A JSON string representing the animation of steps and path found.
 */
template<typename G_COST, typename H_COST>
std::string pathfinding<G_COST, H_COST>::find_path(node &start, const node &end) {
    heap<node_visitor, CompareNode> heap; /**< A min-heap to store nodes to be visited, prioritized by f-cost. */
    std::unordered_set<std::string> open; /**< A set of node IDs that are in the open set. */
    std::unordered_set<std::string> closed; /**< A set of node IDs that have been visited (closed set). */

    node_visitor start_visitor(&g->get_vertex(start.id));
    costs.insert({start.id, start_visitor});
    open.insert(start.id);
    heap.push(start_visitor);

    json animation; /**< JSON object to store step-by-step animation of the pathfinding process. */
    animation["step_count"] = 0;
    animation["steps"] = json::array();

    while (!heap.empty()) {
        const auto current = heap.pop(); /**< Pop the node with the lowest f-cost. */
        open.erase(current.n->id);
        closed.insert(current.n->id);

        json step; /**< JSON object representing a single step in the pathfinding process. */
        animation["step_count"] = animation["step_count"].get<int>() + 1;
        step["parent"] = get_lat_lon(current.n);
        step["children"] = json::array();

        if (current.n->id == end.id) {
            auto path = retrace_path(g->get_vertex(start.id), g->get_vertex(end.id));
            costs.clear(); /**< Clear the costs map after finding the path. */

            animation["path"] = path; /**< Append the found path to the animation JSON. */
            return animation.dump(); /**< Return the animation as a JSON string. */
        }

        for (const auto &neighbour : g->get_neighbours(current.n->id)) {
            if (closed.contains(neighbour))
                continue; /**< Skip the neighbour if it is already in the closed set. */

            auto neighbour_node = &g->get_vertex(neighbour);
            if (!costs.contains(neighbour)) {
                node_visitor neighbour_cost(neighbour_node);
                costs.insert({neighbour, neighbour_cost});
            }
            const auto neighbour_cost = &costs.at(neighbour);

            // Condition to update node costs:
            // If the new movement cost is lower than the current g-cost or the neighbour is not in the open set.
            if (const double movement_cost = current.g_cost + g_cost(current.n, neighbour_node);
                movement_cost < neighbour_cost->g_cost || !open.contains(neighbour_node->id)) {

                // Update g-cost and h-cost of the neighbour.
                neighbour_cost->g_cost = movement_cost;
                neighbour_cost->h_cost = h_cost(neighbour_node, &end);
                neighbour_cost->parent = &g->get_vertex(current.n->id);

                // Add the neighbour to the open set if it is not already present.
                if (!open.contains(neighbour_node->id)) {
                    open.insert(neighbour_node->id);
                    heap.push(costs.at(neighbour));

                    step["children"].push_back(get_lat_lon(neighbour_node));
                }
            }
        }
        heap.heapify(); /**< Reorder the heap after modifying it. */

        animation["steps"].push_back(step); /**< Append the step to the animation JSON. */
    }

    costs.clear(); /**< Clear the costs map if no path is found. */
    return animation.dump(); /**< Return the animation as a JSON string indicating no path was found. */
}

/**
 * @brief Retraces the path from the end node back to the start node.
 *
 * @tparam G_COST The type of the g-cost function or functor.
 * @tparam H_COST The type of the h-cost (heuristic) function or functor.
 * @param start The starting node.
 * @param end The ending node.
 * @return A path represented as a collection of coordinates (latitude and longitude).
 */
template<typename G_COST, typename H_COST>
typename pathfinding<G_COST, H_COST>::path_t pathfinding<G_COST, H_COST>::retrace_path(
    const node &start, const node &end) const {
    std::vector<std::vector<double>> path; /**< Container to store the retraced path. */
    auto current = costs.at(end.id);
    path.push_back(get_lat_lon(current.n));

    while (current.n->id != start.id) {
        current = costs.at(current.parent->id);
        path.push_back(get_lat_lon(current.n));
    }

    return path; /**< Return the retraced path. */
}

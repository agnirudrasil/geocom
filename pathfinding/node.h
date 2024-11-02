//
// Created by Agnirudra Sil on 10/18/24.
//

#ifndef NODE_H
#define NODE_H
#include <string>
#include <format>
#include <iostream>

/**
 * @class node
 * @brief A class representing a node in a graph or grid, typically used for pathfinding.
 */
class node {
public:
    double lat{}; ///< The latitude of the node.
    double lon{}; ///< The longitude of the node.
    double g_cost = 0; ///< The cost from the start node to this node.
    double h_cost = 0; ///< The heuristic cost estimate from this node to the goal.
    node *parent{}; ///< Pointer to the parent node in a path.
    std::string id; ///< A unique identifier for the node.

    /**
     * @brief Default constructor.
     */
    node() = default;

    /**
     * @brief Constructs a node with given latitude and longitude.
     *
     * @param lat The latitude of the node.
     * @param lon The longitude of the node.
     */
    node(const double lat, const double lon);

    /**
     * @brief Calculates the total estimated cost (f_cost) of this node.
     *
     * @return The sum of g_cost and h_cost.
     */
    [[nodiscard]] double f_cost() const noexcept;

    /**
     * @brief Equality operator to compare two nodes based on their IDs.
     *
     * @param l The left-hand side node.
     * @param r The right-hand side node.
     * @return True if the IDs are equal, false otherwise.
     */
    friend bool operator==(const node &l, const node &r);

    /**
     * @brief Inequality operator to compare two nodes based on their IDs.
     *
     * @param l The left-hand side node.
     * @param r The right-hand side node.
     * @return True if the IDs are not equal, false otherwise.
     */
    friend bool operator!=(const node &l, const node &r);

    /**
     * @brief Greater-than operator to compare nodes based on their f_cost.
     *
     * @param l The left-hand side node.
     * @param r The right-hand side node.
     * @return True if the f_cost of l is greater than that of r.
     */
    friend bool operator>(const node &l, const node &r);

    /**
     * @brief Greater-than-or-equal-to operator to compare nodes based on their f_cost.
     *
     * @param l The left-hand side node.
     * @param r The right-hand side node.
     * @return True if the f_cost of l is greater than or equal to that of r.
     */
    friend bool operator>=(const node &l, const node &r);

    /**
     * @brief Less-than operator to compare nodes based on their f_cost.
     *
     * @param l The left-hand side node.
     * @param r The right-hand side node.
     * @return True if the f_cost of l is less than that of r.
     */
    friend bool operator<(const node &l, const node &r);

    /**
     * @brief Less-than-or-equal-to operator to compare nodes based on their f_cost.
     *
     * @param l The left-hand side node.
     * @param r The right-hand side node.
     * @return True if the f_cost of l is less than or equal to that of r.
     */
    friend bool operator<=(const node &l, const node &r);
};

/**
 * @struct std::hash<node>
 * @brief A custom hash specialization for the node class.
 */
template<>
struct std::hash<node> {
    /**
     * @brief Hash function for the node class.
     *
     * @param s The node to hash.
     * @return The hash value of the node's ID.
     */
    std::size_t operator()(node const &s) const noexcept;
};

#endif //NODE_H

//
// Created by Agnirudra Sil on 10/18/24.
//

#ifndef NODE_H
#define NODE_H
#include <string>
#include <iostream>

/**
 * @class node
 * @brief A class representing a node in a graph or grid, typically used for pathfinding.
 */
class node {
public:
    double lat{}; ///< The latitude of the node.
    double lon{}; ///< The longitude of the node.
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
    node(const double lat, const double lon): lat{lat}, lon{lon} {
        this->id = std::format("{}_{}", lat, lon);
    }
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

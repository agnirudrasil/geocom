//
// Created by Agnirudra Sil on 10/26/24.
//

#ifndef UTILS_H
#define UTILS_H

#include "node.h"

/**
 * @brief Converts an angle in degrees to radians.
 *
 * @param deg The angle in degrees.
 * @return The angle in radians.
 */
double deg2rad(double deg);

/**
 * @brief Calculates the Haversine distance between two nodes based on their latitude and longitude.
 *
 * @param node1 Pointer to the first node.
 * @param node2 Pointer to the second node.
 * @return The Haversine distance between the two nodes.
 */
double distance(const node *node1, const node *node2);

#endif //UTILS_H

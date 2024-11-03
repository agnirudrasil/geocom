//
// Created by Agnirudra Sil on 10/26/24.
//

#ifndef UTILS_H
#define UTILS_H

#include <nlohmann/json.hpp>
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


std::tuple<double, double> get_lat_lon(const nlohmann::basic_json<> &coords);

std::vector<double> get_lat_lon(const std::pair<double, double> &coords);

std::vector<double> get_lat_lon(double lat, double lon);

std::vector<double> get_lat_lon(const node &node);

std::vector<double> get_lat_lon(const node *node);

#endif //UTILS_H

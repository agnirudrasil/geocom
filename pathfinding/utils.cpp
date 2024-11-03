//
// Created by Agnirudra Sil on 10/26/24.
//
#include <cmath>
#include <numbers>
#include "utils.h"

#include <nlohmann/json.hpp>

#define R 6371

double deg2rad(double deg) {
    return deg * (std::numbers::pi_v<double> / 180);
}

double distance(const node *node1, const node *node2) {
    const auto d_lat = deg2rad(node2->lat - node1->lat);
    const auto d_lon = deg2rad(node2->lon - node1->lon);

    const auto a = sin(d_lat / 2) * sin(d_lat / 2) +
                   cos(deg2rad(node1->lat)) * cos(deg2rad(node2->lat)) * sin(d_lon / 2) * sin(d_lon / 2);

    const auto c = 2 * atan2(sqrt(a), sqrt(1 - a));

    return R * c;
}

std::tuple<double, double> get_lat_lon(const nlohmann::basic_json<> &coords) {
    double lon = coords[0].get<double>();
    double lat = coords[1].get<double>();

    return std::make_tuple(lat, lon);
}

std::vector<double> get_lat_lon(const std::pair<double, double> &coords) {
    const double lat = coords.first;
    const double lon = coords.second;

    std::vector vector_coords = {lat, lon};

    return vector_coords;
}

std::vector<double> get_lat_lon(double lat, double lon) {
    std::vector vector_coords = {lat, lon};
    return vector_coords;
}

std::vector<double> get_lat_lon(const node &node) {
    std::vector vector_coords = {node.lat, node.lon};

    return vector_coords;
}

std::vector<double> get_lat_lon(const node *node) {
    std::vector vector_coords = {node->lat, node->lon};

    return vector_coords;
}

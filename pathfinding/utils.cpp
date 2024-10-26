//
// Created by Agnirudra Sil on 10/26/24.
//
#include <cmath>
#include <numbers>
#include "utils.h"

double deg2rad(double deg) {
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

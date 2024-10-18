#include <iostream>
#include <vector>
#include <format>
#include <fstream>
#include <nlohmann/json.hpp>

#include "graph.h"
#include "node.h"

using json = nlohmann::json;


std::tuple<double, double> get_lat_lon(const nlohmann::basic_json<> &coords) {
    double lon = coords[0].get<double>();
    double lat = coords[1].get<double>();

    return std::make_tuple(lat, lon);
}


int main() {
    std::fstream f("/Users/agnirudrasil/Projects/geocom/pathfinding/example.json");
    graph<node> g;

    for (json d = json::parse(f); auto &feature: d["features"]) {
        if (feature["geometry"]["type"] == "LineString") {
            auto coordinates = feature["geometry"]["coordinates"];
            double lat, lon;
            std::tie(lat, lon) = get_lat_lon(coordinates[0]);
            node prev(lat, lon);
            g.add_vertex(prev);

            for (int i = 1; i < coordinates.size(); ++i) {
                std::tie(lat, lon) = get_lat_lon(coordinates[i]);
                node node(lat, lon);
                g.add_vertex(node);
                g.add_edge(&prev, &node);
                prev = node;
            }
        }
    }

    std::cout << g.size();
}

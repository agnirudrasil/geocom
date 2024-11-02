//
// Created by Agnirudra Sil on 11/1/24.
//

#include "bind.h"
#include <emscripten/bind.h>
#include <nlohmann/json.hpp>

#include "astar.h"

using namespace emscripten;

using json = nlohmann::json;
using graph_t = graph<node, edge, graph_type::UNDIRECTED, std::string>;


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

class API {
public:
    API() = default;

    static std::optional<std::vector<std::vector<double> > > find_path_local(graph_t &g, node &start, node &goal) {
        const astar a(g);
        auto path = a.find_path(start, goal);

        if (!path) {
            return std::nullopt;
        }

        std::vector<std::vector<double> > result;

        for (const auto &p: *path) {
            result.push_back(get_lat_lon(p));
        }

        return result;
    }

    static graph_t initialize_graph(const std::string &data) {
        graph_t g;
        for (json d = json::parse(data); auto &feature: d["features"]) {
            if (feature["geometry"]["type"] == "LineString") {
                auto coordinates = feature["geometry"]["coordinates"];
                double lat, lon;
                std::tie(lat, lon) = get_lat_lon(coordinates[0]);
                node prev(lat, lon);
                if (!g.contains(prev.id)) {
                    g.add_vertex(prev.id, prev);
                }
                for (int i = 1; i < coordinates.size(); ++i) {
                    std::tie(lat, lon) = get_lat_lon(coordinates[i]);
                    node node(lat, lon);
                    if (!g.contains(node.id)) {
                        g.add_vertex(node.id, node);
                    }
                    g.add_edge(prev.id, node.id, edge(0));
                    prev = std::move(node);
                }
            }
        }

        return g;
    }
};

EMSCRIPTEN_BINDINGS (pathfinding) {
    register_optional<std::vector<std::vector<double> > >();
    register_vector<std::vector<double> >("line");
    register_vector<double>("point");
    class_<API>("API")
            .constructor<>()
            .class_function("find_path", &API::find_path_local)
            .class_function("initialize_graph", &API::initialize_graph);
    class_<graph_t>("graph")
            .constructor<>();
    class_<node>("node")
            .constructor<double, double>();
}

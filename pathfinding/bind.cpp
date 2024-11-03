//
// Created by Agnirudra Sil on 11/1/24.
//

#include <emscripten/bind.h>
#include <nlohmann/json.hpp>

#include "bind.h"
#include "pathfinding.h"
#include "utils.h"

using namespace emscripten;

using json = nlohmann::json;
using graph_t = graph<node, edge, graph_type::UNDIRECTED, std::string>;

struct GCost {
    double operator()(const node *n1, const node *n2) const {
        return distance(n1, n2);
    }
};

struct DijkstraHCost {
    double operator()(const node *, const node *) const {
        return 0.0;
    }
};

class API {
public:
    API() = default;

    static std::string astar_find_path(graph_t &g, node &start, const node &goal) {
        using astar = pathfinding<GCost, GCost>;
        astar a(g);
        return a.find_path(start, goal);
    }

    static std::string dijkstra_find_path(graph_t &g, node &start, const node &goal) {
        using dijkstra = pathfinding<GCost, DijkstraHCost>;
        dijkstra d(g);
        return d.find_path(start, goal);
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


EMSCRIPTEN_BINDINGS(pathfinding) {
    class_<API>("API")
            .constructor<>()
            .class_function("astar_find_path", &API::astar_find_path)
            .class_function("dijkstra_find_path", &API::dijkstra_find_path)
            .class_function("initialize_graph", &API::initialize_graph);
    class_<graph_t>("graph")
            .constructor<>();
    class_<node>("node")
            .constructor<double, double>();
}

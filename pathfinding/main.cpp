#include <fstream>
#include <sstream>

#include "astar.h"
#include "bind.h"
#include "edge.h"

using json = nlohmann::json;
using graph_t = graph<node, edge, graph_type::UNDIRECTED, std::string>;

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

int main() {
    std::ifstream t("/Users/agnirudrasil/Projects/geocom/pathfinding/example.json");
    std::stringstream buffer;
    buffer << t.rdbuf();

    auto g = initialize_graph(buffer.str());

    astar ga(g);
    node start(13.0102262, 74.7925884);
    node end(13.0095455, 74.7950478);

    std::cout << ga.find_path(start, end) << std::endl;


    return 0;
}

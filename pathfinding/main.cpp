#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>

#include "edge.h"
#include "graph.h"
#include "node.h"
#include "utils.h"

using json = nlohmann::json;


std::tuple<double, double> get_lat_lon(const nlohmann::basic_json<> &coords) {
    double lon = coords[0].get<double>();
    double lat = coords[1].get<double>();

    return std::make_tuple(lat, lon);
}

int main(int argc, char *argv[]) {
    using graph_t = graph<node, edge, graph_type::UNDIRECTED, std::string>;
    if (argc < 3) {
        return 1;
    }
    std::fstream f(argv[1]);
    graph_t g;

    for (json d = json::parse(f); auto &feature: d["features"]) {
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

    json j;

    std::cout << "Graph with " << g.vertex_count() << " vertices and " << g.edge_count() << " edges." << "\n";
    double min = INT_MAX;
    node minnode(0, 0);

    std::fstream f1(argv[2]);

    json jarr = json::array();
    for (json d = json::parse(f1); auto &fea: d) {
        json jdata;
        double lat = fea["geometry"]["coordinates"][1];
        double lon = fea["geometry"]["coordinates"][0];


        for (const auto &n: g.get_vertices() | std::views::values) {
            node node(lat, lon);
            if (double dis = distance(&n, &node); dis < min) {
                min = dis;
                minnode = n;
            }
        }

        jdata["name"] = fea["properties"]["tags"]["name"];
        std::vector coords{lat, lon};
        jdata["coords"] = coords;
        std::vector closest{minnode.lat, minnode.lon};
        jdata["closest_node"] = closest;
        jdata["id"] = std::stol(fea["id"].get<std::string>().substr(5));
        jdata["rrule"] = "8:30am to 6:30pm";
        jdata["description"] = "";
        json poc;
        poc["name"] = "John Doe";
        poc["phone"] = "";
        poc["email"] = "";
        std::vector pocs{poc};
        jdata["pocs"] = pocs;
        jarr.push_back(jdata);
    }

    printf("INSERT INTO pois (id, name, description, rrule, coords, closest_coords, pocs) VALUES ");
    for (const auto &jdata: jarr) {
        auto name = jdata["name"].get<std::string>();
        printf("(%ld, '%s', '%s', '%s', '{%.17g, %.17g}', '{%.17g, %.17g}', ARRAY[", jdata["id"].get<long>(),
               jdata["name"].get<std::string>().c_str(),
               jdata["description"].get<std::string>().c_str(),
               jdata["rrule"].get<std::string>().c_str(),
               jdata["coords"][0].get<double>(),
               jdata["coords"][1].get<double>(),
               jdata["closest_node"][0].get<double>(),
               jdata["closest_node"][1].get<double>()
        );
        for (int i = 0; i < jdata["pocs"].size(); i++) {
            auto poc = jdata["pocs"][i];
            if (i != 0) printf(", ");
            printf("('%s', '%s', '%s')::poc",
                   poc["name"].get<std::string>().c_str(),
                   poc["phone"].get<std::string>().c_str(),
                   poc["email"].get<std::string>().c_str()
            );
        }
        printf("]), ");
    }
    printf(";\n");
}

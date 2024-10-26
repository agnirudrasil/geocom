#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>

#include "edge.h"
#include "graph.h"
#include "node.h"
#include "utils.h"

using json = nlohmann::json;

std::tuple<double, double> get_lat_lon(const nlohmann::basic_json<> &coords)
{
    double lon = coords[0].get<double>();
    double lat = coords[1].get<double>();

    return std::make_tuple(lat, lon);
}

int main()
{
    using graph_t = graph<node, edge, graph_type::UNDIRECTED, std::string>;
    std::fstream f("/home/yash/geocom/pathfinding/example.json");
    graph_t g;

    for (json d = json::parse(f); auto &feature : d["features"])
    {
        if (feature["geometry"]["type"] == "LineString")
        {
            auto coordinates = feature["geometry"]["coordinates"];
            double lat, lon;
            std::tie(lat, lon) = get_lat_lon(coordinates[0]);
            node prev(lat, lon);
            if (!g.contains(prev.id))
            {
                g.add_vertex(prev.id, prev);
            }
            for (int i = 1; i < coordinates.size(); ++i)
            {
                std::tie(lat, lon) = get_lat_lon(coordinates[i]);
                node node(lat, lon);
                if (!g.contains(node.id))
                {
                    g.add_vertex(node.id, node);
                }
                g.add_edge(prev.id, node.id, edge(0));
                prev = node;
            }
        }
    }

    json j;

    std::cout << "Graph with " << g.vertex_count() << " vertices and " << g.edge_count() << " edges." << "\n";
    double min = INT_MAX;
    node minnode(0, 0);

    std::fstream f1("/home/yash/geocom/app/filtered.json");
     
    json jarr=json::array();
    for (json d = json::parse(f1); auto &fea : d){
        int i=0;
        json jdata;
        double lat=fea["geometry"]["coordinates"][1];
        double lon=fea["geometry"]["coordinates"][0];
       
        
       

    
    for (const auto &n : g.get_vertices() | std::views::values)
    {
        node node(lat, lon);
        double dis = distance(&n, &node);
        if (dis < min)
        {
            min = dis;
            minnode = std::forward<decltype(n)>(n);
        }
    }
        jdata["name"]=fea["properties"]["tags"]["name"];
        jdata["closest_node"]=minnode.id;
        jdata["id"]=fea["id"];
        jdata["timings"]="8:30am to 6:30pm";
        jdata["description"]="";
        jdata["pocs"]={
            {"name","John Doe"},
            {"phone",""},
            {"email",""}
        };
   // std::cout << minnode.id << std::endl; 
    jarr.push_back(jdata);
    }
    std::ofstream fl("finaldata.json");
    if(fl.is_open()){
        fl<<jarr.dump(1);
        fl.close();
        std::cout<<"succesfully dumped"<<std::endl;
    }
    else{
        std::cerr<<"failed"<<std::endl;
    }
    // node start(13.0124083, 74.7917972);
    // node end(13.0072809, 74.7970999);
    // astar astar(g);
    // auto path = astar.find_path(start, end);
    // j["path"] = path.value();

    // std::cout << j << '\n';
}

//
// Created by Agnirudra Sil on 10/18/24.
//

#ifndef GRAPH_H
#define GRAPH_H
#include <unordered_map>
#include <unordered_set>


template<typename T>
class graph {
    std::unordered_map<T, std::unordered_set<T *> > adj_list;

public:
    graph() = default;

    void add_vertex(T vertex) {
        if (!adj_list.contains(vertex)) {
            std::unordered_set<T *> neighbours;
            adj_list.insert({vertex, neighbours});
        }
    }

    void add_edge(T u, T v) {
        if (!adj_list.contains(*u))
            add_vertex(u);
        else if (!adj_list.contains(*v))
            add_vertex(v);


        adj_list.at(u).insert(v);
        adj_list.at(v).insert(u);
    }

    size_t size() {
        return adj_list.size();
    }

    std::unordered_set<T> neighbours(T vertex) {
        return adj_list[vertex];
    }
};


#endif //GRAPH_H

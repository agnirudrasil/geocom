#pragma once


template<typename VERTEX_T, typename EDGE_T, graph_type GRAPH_TYPE, typename VERTEX_ID_T>
size_t graph<VERTEX_T, EDGE_T, GRAPH_TYPE, VERTEX_ID_T>::vertex_count() const noexcept {
    return vertices.size();
}

template<typename VERTEX_T, typename EDGE_T, graph_type GRAPH_TYPE, typename VERTEX_ID_T>
size_t graph<VERTEX_T, EDGE_T, GRAPH_TYPE, VERTEX_ID_T>::edge_count() const noexcept {
    return edges.size();
}

template<typename VERTEX_T, typename EDGE_T, graph_type GRAPH_TYPE, typename VERTEX_ID_T>
bool graph<VERTEX_T, EDGE_T, GRAPH_TYPE, VERTEX_ID_T>::contains(vertex_id_t &vertex_id) const noexcept {
    return vertices.contains(vertex_id);
}

template<typename VERTEX_T, typename EDGE_T, graph_type GRAPH_TYPE, typename VERTEX_ID_T>
bool graph<VERTEX_T, EDGE_T, GRAPH_TYPE, VERTEX_ID_T>::is_adjacent(const vertex_id_t &lhs,
                                                                   const vertex_id_t &rhs) const noexcept {
    auto edge_id = get_edge_id(lhs, rhs);
    return edges.contains(edge_id);
}


template<typename VERTEX_T, typename EDGE_T, graph_type GRAPH_TYPE, typename VERTEX_ID_T>
typename graph<VERTEX_T, EDGE_T, GRAPH_TYPE, VERTEX_ID_T>::edge_t &graph<VERTEX_T, EDGE_T, GRAPH_TYPE, VERTEX_ID_T>::
get_edge(const vertex_id_t &lhs, const vertex_id_t &rhs) {
    if (!is_adjacent(lhs, rhs)) {
        throw std::invalid_argument{"No edge found"};
    }
    auto edge_id = get_edge_id(lhs, rhs);
    return edges.at(edge_id);
}

template<typename VERTEX_T, typename EDGE_T, graph_type GRAPH_TYPE, typename VERTEX_ID_T>
typename graph<VERTEX_T, EDGE_T, GRAPH_TYPE, VERTEX_ID_T>::edge_t &graph<VERTEX_T, EDGE_T, GRAPH_TYPE, VERTEX_ID_T>::
get_edge(const edge_id_t &edge_id) {
    if (!edges.contains(edge_id)) {
        throw std::invalid_argument{"No edge found"};
    }

    return edges.at(edge_id);
}

template<typename VERTEX_T, typename EDGE_T, graph_type GRAPH_TYPE, typename VERTEX_ID_T>
typename graph<VERTEX_T, EDGE_T, GRAPH_TYPE, VERTEX_ID_T>::vertices_set_t graph<VERTEX_T, EDGE_T, GRAPH_TYPE,
    VERTEX_ID_T>::get_neighbours(vertex_id_t &vertex_id) {
    if (!adj_list.contains(vertex_id)) {
        return {};
    }
    return adj_list.at(vertex_id);
}

template<typename VERTEX_T, typename EDGE_T, graph_type GRAPH_TYPE, typename VERTEX_ID_T>
void graph<VERTEX_T, EDGE_T, GRAPH_TYPE, VERTEX_ID_T>::add_vertex(vertex_id_t &vertex_id, auto &&vertex) {
    if (contains(vertex_id)) {
        throw std::invalid_argument{"Vertex already exists"};
    }
    vertices.emplace(vertex_id, std::forward<decltype(vertex)>(vertex));
}

template<typename VERTEX_T, typename EDGE_T, graph_type GRAPH_TYPE, typename VERTEX_ID_T>
void graph<VERTEX_T, EDGE_T, GRAPH_TYPE, VERTEX_ID_T>::remove_vertex(vertex_id_t &vertex_id) {
    if (adj_list.contains(vertex_id)) {
        for (auto &neighbour_id: adj_list.at(vertex_id)) {
            edges.erase({vertex_id, neighbour_id});
        }
    }
    adj_list.erase(vertex_id);
    vertices.erase(vertex_id);

    for (auto &[parent_vertex_id, neighbours]: adj_list) {
        neighbours.erase(vertex_id);
        edges.erase({parent_vertex_id, vertex_id});
    }
}

template<typename VERTEX_T, typename EDGE_T, graph_type GRAPH_TYPE, typename VERTEX_ID_T>
void graph<VERTEX_T, EDGE_T, GRAPH_TYPE, VERTEX_ID_T>::add_edge(vertex_id_t &vertex_id_lhs, vertex_id_t &vertex_id_rhs,
                                                                auto &&edge) {
    if (!contains(vertex_id_lhs) || !contains(vertex_id_rhs)) {
        throw std::invalid_argument("Vertices not found");
    }

    auto edge_id = get_edge_id(vertex_id_lhs, vertex_id_rhs);
    edges.emplace(edge_id, std::forward<decltype(edge)>(edge));
    adj_list[vertex_id_lhs].insert(vertex_id_rhs);
    if constexpr (is_undirected()) {
        adj_list[vertex_id_rhs].insert(vertex_id_lhs);
    }
}

template<typename VERTEX_T, typename EDGE_T, graph_type GRAPH_TYPE, typename VERTEX_ID_T>
void graph<VERTEX_T, EDGE_T, GRAPH_TYPE, VERTEX_ID_T>::remove_edge(vertex_id_t &lhs, vertex_id_t &rhs) {
    auto edge_id = get_edge_id(lhs, rhs);
    edges.erase(edge_id);
    adj_list[lhs].erase(rhs);
    if constexpr (is_undirected()) {
        adj_list[rhs].insert(lhs);
    }
}

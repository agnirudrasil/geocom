//
// Created by Agnirudra Sil on 10/19/24.
//

#ifndef EDGE_H
#define EDGE_H


class edge {
    int weight = 0;

public:
    edge() = default;

    explicit edge(const int weight) : weight{weight} {
    }
};


#endif //EDGE_H

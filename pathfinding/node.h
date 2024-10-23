//
// Created by Agnirudra Sil on 10/18/24.
//

#ifndef NODE_H
#define NODE_H
#include <string>
#include <format>
#include <iostream>


class node {
public:
    double lat;
    double lon;
    double g_cost = 0;
    double h_cost = 0;
    node *parent{};
    std::string id;

    node(const double lat, const double lon): lat{lat}, lon{lon} {
        this->id = std::format("{}_{}", lat, lon);
    }

    [[nodiscard]] double f_cost() const noexcept {
        return g_cost + h_cost;
    }


    friend bool operator==(const node &l, const node &r) {
        return l.id == r.id;
    };

    friend bool operator!=(const node &l, const node &r) {
        return l.id != r.id;
    };

    friend bool operator>(const node &l, const node &r) {
        return l.f_cost() > r.f_cost();
    };

    friend bool operator>=(const node &l, const node &r) {
        return !(l.f_cost() < r.f_cost());
    };

    friend bool operator<(const node &l, const node &r) {
        return l.f_cost() < r.f_cost();
    };

    friend bool operator<=(const node &l, const node &r) {
        return !(l.f_cost() > r.f_cost());
    };
};

template<>
struct std::hash<node> {
    std::size_t operator()(node const &s) const noexcept {
        return std::hash<std::string>{}(s.id);
    }
};

#endif //NODE_H

//
// Created by Agnirudra Sil on 10/18/24.
//

#include "node.h"


bool operator==(const node &l, const node &r) {
    return l.id == r.id;
}

bool operator!=(const node &l, const node &r) {
    return !(l == r);
}

bool operator>(const node &l, const node &r) {
    return l.f_cost() > r.f_cost();
}

bool operator>=(const node &l, const node &r) {
    return !(l < r);
}

bool operator<(const node &l, const node &r) {
    return l.f_cost() < r.f_cost();
}

bool operator<=(const node &l, const node &r) {
    return !(l > r);
}

double node::f_cost() const {
    return this->g_cost + this->g_cost;
}

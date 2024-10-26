//
// Created by Agnirudra Sil on 10/26/24.
//

#ifndef UTILS_H
#define UTILS_H

#define R 6371
#include <numbers>

#include "node.h"

double deg2rad(double deg);

double distance(const node *node1, const node *node2);
#endif //UTILS_H

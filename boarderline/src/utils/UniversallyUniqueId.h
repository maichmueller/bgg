//
// Created by michael on 04.09.19.
//

#pragma once

#include <numeric>
#include "array"

template <int N = 10000>
struct UUID {
    static std::array<unsigned int, N> pool = std::iota(pool.begin(), pool.end(), 0);


};


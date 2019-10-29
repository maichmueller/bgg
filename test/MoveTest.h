//
// Created by michael on 20.10.19.
//

#pragma once

#include "board/Move.h"

namespace {
    template <size_t N>
    using int_move = Move<Position<int, N>>;
    template <size_t N>
    using double_move = Move<Position<double, N>>;
};


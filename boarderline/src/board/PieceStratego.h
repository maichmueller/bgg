//
// Created by Michael on 20/02/2019.
//

#pragma once

#include "array"
#include "Position.h"
#include "Piece.h"


class PieceStratego : public Piece<Position<int, 2>, 2> {
public:
    using base_type = Piece<Position<int, 2>, 2>;
    using position_type = base_type::position_type;
    using kin_type = base_type::kin_type;
};

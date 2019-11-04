#pragma once

#include "array"
#include "Position.h"
#include "Piece.h"


class PieceStratego : public Piece<Position<int, 2>, 2> {
public:
using base_type = Piece<Position<int, 2>, 2>;

// inheriting the base constructors with the next command!
using base_type::base_type;
};
//
// Created by michael on 21.10.19.
//


#pragma once

#include "azpp/board/Board.h"

namespace BoardTest{
    using position_type = Position<int, 2>;
    using piece_type = Piece<position_type, 2>;
    using kin_type = typename piece_type::kin_type;
    using planar_board = Board<piece_type>;
};



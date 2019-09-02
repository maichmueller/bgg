//
// Created by Michael on 20/02/2019.
//

#pragma once

#include "map"
#include "array"
#include "vector"
#include "memory"

#include "PieceStratego.h"
#include "Board.h"


class BoardStratego : public Board<PieceStratego, typename PieceStratego::position_type> {
};



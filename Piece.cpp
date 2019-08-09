//
// Created by Michael on 26/02/2019.
//

#include "Piece.h"


Piece::Piece(int team, int type, pos_t pos, int version=1,
             bool hidden=true, bool has_moved=false, bool can_move=true)
        : null_piece(false), team(team), type(type), pos(pos), version(version),
          hidden(hidden), has_moved(has_moved),
          can_move(can_move)
{
    if(type == 0 || type == 11) {
        this->can_move = false;
    }
}

Piece::Piece(int team, int type, pos_t pos, int version=1)
        : null_piece(false), team(team), type(type),
          pos(pos), version(version),
          hidden(true), has_moved(false),
          can_move(true)
{
    if(type == 0 || type == 11) {
        this->can_move = false;
    }
}

// a Null Piece Constructor
Piece::Piece(const pos_t& pos)
        : null_piece(true), pos(pos), team(-1), type(-1), version(-1),
          hidden(false), has_moved(false),
          can_move(false)
{}

Piece::Piece()
        : null_piece(true), pos({-1, -1}), team(-1), type(-1), version(-1),
          hidden(false), has_moved(false),
          can_move(false)
{}

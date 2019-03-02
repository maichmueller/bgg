//
// Created by Michael on 26/02/2019.
//

#include "Piece.h"


Piece::Piece(int team, int type, pos_type& pos, int version=1,
             bool hidden=true, bool alive=true,
             bool has_moved=false, bool can_move=true)
        : null_piece(false), team(team), type(type), pos(pos), version(version),
          hidden(hidden), alive(alive), has_moved(has_moved),
          can_move(can_move)
{
    if(type == 0 || type == 11) {
        this->can_move = false;
    }
    null_piece = false;
}

Piece::Piece(int team, int type, pos_type& pos, int version=1)
        : null_piece(false), team(team), type(type), pos(pos), version(version),
          hidden(true), alive(true), has_moved(false),
          can_move(true)
{
    if(type == 0 || type == 11) {
        this->can_move = false;
    }
    null_piece = false;
}

// a Null Piece Constructor
Piece::Piece(pos_type& pos)
        : null_piece(true), pos(pos), team(-1), type(-1), version(-1),
          hidden(false), alive(false), has_moved(false),
          can_move(false)
{}

Piece::Piece()
        : null_piece(true), pos({-1, -1}), team(-1), type(-1), version(-1),
          hidden(false), alive(false), has_moved(false),
          can_move(false)
{}

void Piece::change_piece(int team, int type, bool hidden, bool alive) {
    this->team = team;
    this->type = type;
    this->hidden = hidden;
    this->alive = alive;
    if(type != 0 && type != 11) {
        this->can_move = true;
    }
}
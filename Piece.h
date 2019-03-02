//
// Created by Michael on 20/02/2019.
//

#ifndef STRATEGO_CPP_PIECE_H
#define STRATEGO_CPP_PIECE_H

#include "array"
#include "GameDeclarations.h"


using namespace std;

using namespace GameTypes;


class Piece {

private:
    bool null_piece = false;
    pos_type pos;
    int team;
    int type;
    int version;
    bool hidden;
    bool alive;
    bool has_moved;
    bool can_move;

public:
    Piece(int team, int type, pos_type& pos, int version,
          bool hidden, bool alive, bool has_moved, bool can_move);
    Piece(int team, int type, pos_type& pos, int version);
    explicit Piece(pos_type& pos);
    Piece();

    void set_version(int v) {version = v;}
    void set_flag_has_moved() {has_moved = true;}
    void set_flag_unhidden() {hidden = false;}
    void set_position(pos_type& p) {pos = p;}

    bool is_null() {return null_piece;}
    pos_type get_position() {return pos;}
    int get_team() {return team;}
    int get_type() {return type;}
    int get_version() {return version;}
    bool get_flag_hidden() {return hidden;}
    bool get_flag_alive() {return alive;}
    bool get_flag_has_moved() {return has_moved;}
    bool get_flag_can_move() {return can_move;}

    void change_piece(int team, int type, bool hidden=true, bool alive=true);


};


#endif //STRATEGO_CPP_PIECE_H

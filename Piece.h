//
// Created by Michael on 20/02/2019.
//

#ifndef STRATEGO_CPP_PIECE_H
#define STRATEGO_CPP_PIECE_H

#include "array"
#include "GameDeclarations.h"


using namespace GameDeclarations;


class Piece {

private:
    bool null_piece = false;
    pos_type pos;
    int team;
    int type;
    int version;
    bool hidden;
    bool has_moved;
    bool can_move;

public:
    Piece(int team, int type, pos_type& pos, int version,
          bool hidden, bool has_moved, bool can_move);
    Piece(int team, int type, const pos_type& pos, int version);
    explicit Piece(const pos_type& pos);
    Piece();

    void set_version(int v) {version = v;}
    void set_flag_has_moved() {has_moved = true;}
    void set_flag_unhidden() {hidden = false;}
    void set_position(pos_type& p) {pos = p;}

    bool is_null() {return null_piece;}
    pos_type get_position(bool flip_position=false, int dim=0) {
        if(flip_position) {
            return {dim - pos[0], dim - pos[1]};
        }
        else
            return pos;
    }
    int get_team(bool flip_team=false) {return (flip_team) ? 1 - team : team;}
    int get_type() {return type;}
    int get_version() {return version;}
    bool get_flag_hidden() {return hidden;}
    bool get_flag_has_moved() {return has_moved;}
    bool get_flag_can_move() {return can_move;}

    void change_piece(int team, int type, bool hidden=true);


};


#endif //STRATEGO_CPP_PIECE_H

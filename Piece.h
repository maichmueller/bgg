//
// Created by Michael on 20/02/2019.
//

#ifndef STRATEGO_CPP_PIECE_H
#define STRATEGO_CPP_PIECE_H

#include "array"
#include "GameDeclarations.h"


using namespace GameDeclarations;


class Piece {
    /**
     * A typical Piece class holding the most relevant data to describe a piece.
     * Each piece is assigned a team (0 or 1), a position (int, int), and given a type (int 0-11).
     * Since there can be more than one piece of a type each piece also receives a version
     * int member starting at 0.
     * Meta-attributes are 'hidden', 'has_moved', 'can_move'.
     *
     * Null-Pieces are set by the flag 'null_piece', which is necessary
     * since every position on a board needs a piece at any time (for simpler iteration code).
     *
     **/
private:
    bool null_piece = false;
    pos_t pos;
    int team;
    int type;
    int version;
    bool hidden;
    bool has_moved;
    bool can_move;

public:
    Piece(int team, int type, pos_t& pos, int version,
          bool hidden, bool has_moved, bool can_move);
    Piece(int team, int type, const pos_t& pos, int version);
    explicit Piece(const pos_t& pos);
    Piece();

    // getter and setter methods here only

    void set_version(int v) {version = v;}
    void set_flag_has_moved() {has_moved = true;}
    void set_flag_unhidden() {hidden = false;}
    void set_position(pos_t& p) { pos = p;}

    bool is_null() {return null_piece;}
    pos_t get_position(bool flip_position=false, int dim=0) {
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


};


#endif //STRATEGO_CPP_PIECE_H

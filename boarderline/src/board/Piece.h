//
// Created by Michael on 20/02/2019.
//

#ifndef STRATEGO_CPP_PIECE_H
#define STRATEGO_CPP_PIECE_H

#include "array"
#include "../game/GameDeclarations.h"


using namespace GameDeclarations;

template <typename Position, typename Species>
class PieceBase {
    /**
     * A typical Piece class holding the most relevant data to describe a piece.
     * Each piece is assigned a team (0 or 1), a PositionType position, and given a type (int 0-11).
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
    Position pos;
    int team;
    Species type;
    int version;
    bool hidden;
    bool has_moved;
    bool can_move;

public:
    using position_type = Position;
    using spieces_type = Species;

    PieceBase(Position pos, int team, Species type, int version=0)
            : team(team), type(type),
              pos(pos), version(version),
              hidden(true), has_moved(false),
              can_move(true)
    {
        if(type == 0 || type == 11) {
            this->can_move = false;
        }
    }

// a Null Piece Constructor
    PieceBase(const Position& pos)
            : null_piece(true), pos(pos), team(-1), type(-1), version(-1),
              hidden(false), has_moved(false),
              can_move(false)
    {}

    PieceBase()
            : null_piece(true), pos({-1, -1}), team(-1), type(-1), version(-1),
              hidden(false), has_moved(false),
              can_move(false)
    {}


    // getter and setter methods here only

    void set_version(int v) { version = v; }

    void set_flag_has_moved(bool has_moved=true) { this->has_moved = has_moved; }

    void set_flag_hidden(bool h=false) { hidden = h; }

    void set_position(Position &p) { pos = p; }

    [[nodiscard]] bool is_null() const { return null_piece; }

    [[nodiscard]] Position get_position(bool flip_position = false, int dim = 5) const {
        if (flip_position) {
            return {dim - pos[0] - 1, dim - pos[1] - 1};
        } else
            return pos;
    }

    [[nodiscard]] int get_team(bool flip_team = false) const { return (flip_team) ? 1 - team : team; }

    [[nodiscard]] Species get_type() const { return type; }

    [[nodiscard]] int get_version() const { return version; }

    [[nodiscard]] bool get_flag_hidden() const { return hidden; }

    [[nodiscard]] bool get_flag_has_moved() const { return has_moved; }

    [[nodiscard]] bool get_flag_can_move() const { return can_move; }

};


#endif //STRATEGO_CPP_PIECE_H

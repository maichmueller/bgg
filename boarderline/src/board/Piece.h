//
// Created by Michael on 20/02/2019.
//

#pragma once

#include <array>
#include "../utils/UniversallyUniqueId.h"




template <typename Position, int NrTypeIds>
class Piece {
    /**
     * A typical Piece class holding the most relevant data to describe a piece.
     * Each piece is assigned a team (0 or 1), a PositionType position, and given a Type (int as the basic idea 0-11).
     * Since there can be more than one piece of a type each piece also receives a version (part of the type member).
     * Meta-attributes are 'hidden', 'has_moved', 'can_move'.
     *
     * Null-Pieces are set by the flag 'null_piece', which is necessary
     * since every position on a board needs a piece at any time.
     *
     **/

    struct CharacterType{
        using hash = tuple::hash<CharacterType>;
        using eq_comp = eqcomp_tuple::eqcomp<CharacterType>;

        std::array<int, NrTypeIds> specifiers;
        static const int nr_identifiers = NrTypeIds;

        explicit CharacterType(std::array<int, NrTypeIds> sp) : specifiers(sp) {}

        int operator[](size_t index) const {return specifiers[index];}
        int operator[](size_t index) {return specifiers[index];}
        auto begin() {return specifiers.begin();}
        auto end() { return specifiers.end();}
        auto begin() const {return specifiers.begin();}
        auto end() const { return specifiers.end();}
    };

public:
    using position_type = Position;
    using character_type = CharacterType;

protected:
    position_type m_pos;
    character_type m_type;
    unsigned int m_uuid = UUID::get_unique_id();
    int m_team;
    bool m_null_piece = false;
    bool m_hidden;
    bool m_has_moved;
    bool m_can_move;

public:
    Piece(position_type pos, character_type type, int team,
          bool hidden, bool has_moved, bool can_move)
            : m_team(team), m_type(type),
              m_pos(pos),
              m_hidden(hidden), m_has_moved(has_moved),
              m_can_move(can_move)
    {}

    Piece(position_type pos, int team, character_type type)
            : m_team(team), m_type(type),
              m_pos(pos),
              m_hidden(true), m_has_moved(false),
              m_can_move(true)
    {}

// a Null Piece Constructor
    explicit Piece(const position_type & pos)
            : m_null_piece(true), m_pos(pos), m_team(-1), m_type(),
              m_hidden(false), m_has_moved(false),
              m_can_move(false)
    {}

    ~Piece() {
        UUID::free_id(m_uuid);
    }

    // getter and setter methods here only

    void set_flag_has_moved(bool has_moved=true) { this->m_has_moved = has_moved; }

    void set_flag_hidden(bool h=false) { m_hidden = h; }

    void set_position(position_type p) { m_pos = std::move(p); }

    [[nodiscard]] bool is_null() const { return m_null_piece; }

    [[nodiscard]] position_type get_position() const {return m_pos;}

    [[nodiscard]] int get_team(bool flip_team = false) const { return (flip_team) ? 1 - m_team : m_team; }

    [[nodiscard]] typename character_type::const_iterator get_type() const { return m_type.begin(); }

    [[nodiscard]] bool get_flag_hidden() const { return m_hidden; }

    [[nodiscard]] bool get_flag_has_moved() const { return m_has_moved; }

    [[nodiscard]] bool get_flag_can_move() const { return m_can_move; }

};

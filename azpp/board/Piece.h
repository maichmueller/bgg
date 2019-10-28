//
// Created by Michael on 20/02/2019.
//

#pragma once

#include <array>
#include "../utils/utils.h"
//#include "../utils/UniversallyUniqueId.h"


template <size_t NrIds>
struct Kin{
    using container_type = std::array<int, NrIds>;
    using hash = tuple::hash<Kin>;
    using eq_comp = eqcomp_tuple::eqcomp<Kin>;

    std::array<int, NrIds> specifiers;
    static const size_t nr_identifiers = NrIds;

    explicit Kin(std::array<int, NrIds> sp) : specifiers(std::move(sp)) {}
    explicit Kin() : specifiers() {std::fill(specifiers.begin(), specifiers.end(), 404);};

    template <typename ... Types, typename std::enable_if<sizeof...(Types) == NrIds, int>::type = 0>
    Kin(Types&& ... vals)
    : Kin(std::index_sequence_for<Types...>{}, std::forward<Types>(vals)...) {}

private:
    template <std::size_t...Indices, typename ... Types>
    Kin(std::index_sequence<Indices...>, Types&& ... vals)
    {
        (static_cast<void>(specifiers[Indices] = vals), ...);
    }

public:
    int operator[](size_t index) const {return specifiers[index];}
    int operator[](size_t index) {return specifiers[index];}
    auto begin() {return specifiers.begin();}
    auto end() { return specifiers.end();}
    auto begin() const {return specifiers.begin();}
    auto end() const { return specifiers.end();}
    bool operator==(const Kin & other) {
        for(size_t i = 0; i < NrIds; ++i) {
            if(specifiers[i] != other[i])
                return false;
        }
        return true;
    }
    bool operator!=(const Kin & other) {return !(this == other);}
    std::string to_string() {
        std::ostringstream ss;
        ss << "{";
        for (auto spec_it = specifiers.begin(); spec_it != specifiers.end(); ++spec_it) {
            ss << *spec_it << ", ";
        }
        ss << specifiers.back() << "}";
        return ss.str();
    }
};


template <typename Position, size_t NrIdentifiers>
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

public:
    using position_type = Position;
    using kin_type = Kin<NrIdentifiers>;

protected:
    position_type m_position;
    kin_type m_type;
//    unsigned int m_uuid = UUID::get_unique_id();
    int m_team;
    bool m_null_piece = false;
    bool m_hidden;
    bool m_has_moved;

public:
    Piece(position_type pos, kin_type type, int team,
          bool hidden, bool has_moved)
            : m_position(pos),
              m_type(type),
              m_team(team),
              m_hidden(hidden),
              m_has_moved(has_moved)
    {}

    Piece(position_type position, kin_type type, int team)
            : Piece(position, type, team, true, false)
    {}

// a Null Piece Constructor
    explicit Piece(const position_type & position)
            : m_position(position),
              m_type(),
              m_team(-1),
              m_null_piece(true),
              m_hidden(false),
              m_has_moved(false)
    {}
//
//    ~Piece() {
//        UUID::free_id(m_uuid);
//    }

    // getter and setter methods here only

    void set_flag_has_moved(bool has_moved=true) { this->m_has_moved = has_moved; }

    void set_flag_unhidden(bool h=false) { m_hidden = h; }

    void set_position(position_type p) { m_position = std::move(p); }

    [[nodiscard]] bool is_null() const { return m_null_piece; }

    [[nodiscard]] position_type get_position() const {return m_position;}

    [[nodiscard]] int get_team(bool flip_team = false) const { return (flip_team) ? 1 - m_team : m_team; }

    [[nodiscard]] kin_type get_kin() const { return m_type; }

    [[nodiscard]] bool get_flag_hidden() const { return m_hidden; }

    [[nodiscard]] bool get_flag_has_moved() const { return m_has_moved; }

    bool operator==(const Piece & other) {
        return
        other.get_position() == m_position &&
        m_type == other.get_kin() &&
        m_team == other.get_team() &&
        m_hidden == other.get_flag_hidden() &&
        m_has_moved == other.get_flag_has_moved();
    }
    bool operator!=(const Piece & other) {return !(this == other);}

};

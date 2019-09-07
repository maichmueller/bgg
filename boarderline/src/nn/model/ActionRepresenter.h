//
// Created by michael on 08.08.19.
//

#pragma once

#include <numeric>
#include "array"
#include "vector"
#include "map"
#include "unordered_map"
#include "memory"
#include "../../board/Position.h"
#include "../../board/Move.h"


template <typename Vector, typename PieceIdType>
struct Action {

    using vector_type = Vector;
    using piece_ident_type = PieceIdType;

    vector_type m_effect_pl_0;
    vector_type m_effect_pl_1;
    piece_ident_type m_piece_id;

    Action(vector_type action, piece_ident_type piece_identifier)
    : m_effect_pl_0(action), m_effect_pl_1(action.invert()), m_piece_id(piece_identifier)
    {}

    [[nodiscard]] vector_type get_action(int player) const {
        if(player) return m_effect_pl_1;
        else return m_effect_pl_0;
    }
    [[nodiscard]] piece_ident_type get_piece_id() const {return m_piece_id;}

    template <typename LengthType, size_t dim>
    Move<Position<LengthType, dim>> to_move(const Position<LengthType, dim> & pos, int player) {
        if(player==0)
            return pos + m_effect_pl_0;
        else
            return pos + m_effect_pl_1;
    }
};


template <typename Action, typename Piece>
class ActionRepBase {

public:
    using action_type = Action;
    using piece_id_type = typename Action::piece_id_type;
    using piece_type = Piece;

    using key = typename piece_type::character_type;
    using hasher = typename piece_type::character_type::hash;
    using eq_comp = typename piece_type::character_type::eq_comp;
    using piece_ident_map = std::unordered_map<key, std::shared_ptr<piece_type>, hasher, eq_comp>;

protected:
    static const std::vector<action_type > action_rep_vector;
    std::array<piece_ident_map, 2> actors{};

public:
    virtual void assign_actors() = 0;
    static auto const & get_act_rep() { return action_rep_vector; }

    template <typename Position>
    Move<Position> action_to_move(const Position & pos, int action, int player) {
        return pos + action_rep_vector[action].get_action(player);
    }
};


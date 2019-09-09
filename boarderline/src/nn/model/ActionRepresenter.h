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


template <typename Vector, typename KinType>
struct Action {

    using vector_type = Vector;
    using kin_type = KinType;

    vector_type m_effect_pl_0;
    vector_type m_effect_pl_1;
    kin_type m_piece_id;

    Action(vector_type action, kin_type piece_identifier)
    : m_effect_pl_0(action), m_effect_pl_1(action.invert()), m_piece_id(piece_identifier)
    {}

    [[nodiscard]] vector_type get_action(int player) const {
        if(player) return m_effect_pl_1;
        else return m_effect_pl_0;
    }
    [[nodiscard]] kin_type get_piece_id() const {return m_piece_id;}

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
    using piece_type = Piece;

    using kin_type = typename Action::kin_type;
    using hasher = typename kin_type::hash;
    using eq_comp = typename kin_type::eq_comp;
    using piece_ident_map = std::unordered_map<kin_type, std::shared_ptr<piece_type>, hasher, eq_comp>;

protected:
    static const std::vector<action_type > action_rep_vector;
    std::array<piece_ident_map, 2> actors{};

public:
    template <typename GameState>
    virtual void assign_actors(GameState & gs) = 0;

    static auto const & get_act_rep() { return action_rep_vector; }

    template <typename Position>
    Move<Position> action_to_move(const Position & pos, int action, int player) {
        return pos + action_rep_vector[action].get_action(player);
    }

    virtual torch::Tensor state_representation(int player) = 0;
};


template <typename Action, typename Piece>
class ActionRepBase {

public:
    using action_type = Action;
    using piece_type = Piece;

    using kin_type = typename Action::kin_type;
    using hasher = typename kin_type::hash;
    using eq_comp = typename kin_type::eq_comp;
    using piece_ident_map = std::unordered_map<kin_type, std::shared_ptr<piece_type>, hasher, eq_comp>;

protected:
    static const std::vector<action_type > action_rep_vector;
    std::array<piece_ident_map, 2> actors{};

public:
    template <typename GameState>
    virtual void assign_actors(GameState & gs) = 0;
    static auto const & get_act_rep() { return action_rep_vector; }

    template <typename Position>
    Move<Position> action_to_move(const Position & pos, int action, int player) {
        return pos + action_rep_vector[action].get_action(player);
    }

    virtual torch::Tensor state_representation(int player) = 0;
};


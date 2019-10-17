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

#include "torch/torch.h"

#include "../../board/Position.h"
#include "../../board/Move.h"
#include "../../game/GameState.h"

#include <experimental/type_traits>


template<typename Action, typename State, typename Derived>
class ActionRepBase {

public:
    using action_type = Action;
    using state_type = State;
    using board_type = typename state_type::Board;
    using position_type = typename board_type::position_type;
    using move_type = typename board_type::move_type;
    using kin_type = typename state_type::kin_type;


    void enable_representation(const state_type& gs) { static_cast<Derived*>(this)->enable_representation(gs); }
    void update_actors(int team, kin_type kin) { static_cast<Derived*>(this)->update_actors(team, kin); }
    const std::vector<Action> * get_act_rep() { return static_cast<Derived*>(this)->get_action_rep_vector(); }
    torch::Tensor state_representation(int player) { return static_cast<Derived*>(this)->state_representation(player); }

    template <typename Position>
    Move<Position> action_to_move(const Position & pos, int action, int player) {
        return pos + get_act_rep()[action].get_effect(player);
    }

protected:
    std::vector<kin_type> live_actors;

};

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
    using board_type = typename state_type::board_type;
    using position_type = typename board_type::position_type;
    using move_type = typename board_type::move_type;
    using kin_type = typename state_type::kin_type;


    void enable_representation(const state_type & gs) { static_cast<Derived*>(this)->enable_representation(gs); }
    const std::vector<Action> * get_act_rep() { return static_cast<Derived*>(this)->get_action_rep_vector(); }

    // depending on the game/representation strategy of the subclass, a positional variable amount of parameters
    // can be passed to allow differing implementations without knowing each use case beforehand.
    template <typename... Params>
    torch::Tensor state_representation(const state_type & state, int player, Params... params) {
        return static_cast<Derived*>(this)->state_representation(player, params...);
    }

    template <typename Position>
    Move<Position> action_to_move(const Position & pos, int action, int player) {
        return pos + get_act_rep()[action].get_effect(player);
    }


};

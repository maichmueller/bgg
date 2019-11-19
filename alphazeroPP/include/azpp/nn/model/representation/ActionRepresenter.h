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

#include "azpp/board/Position.h"
#include "azpp/board/Move.h"
#include "azpp/game/State.h"

#include <type_traits>


template<typename Action, typename StateType, typename Derived>
class ActionRepBase {

public:
    using action_type = Action;
    using state_type = StateType;
    using board_type = typename state_type::board_type;
    using position_type = typename board_type::position_type;
    using move_type = typename board_type::move_type;
    using kin_type = typename state_type::kin_type;


    const std::vector<Action> * get_actions_vec() { return static_cast<Derived *>(this)->get_action_rep_vector(); }

    // depending on the game/representation strategy of the subclass, a positional variable amount of parameters
    // can be passed to allow differing implementations without knowing each use case beforehand.
    template<typename... Params>
    torch::Tensor state_representation(const state_type &state, const Params &... params) {
        return static_cast<Derived *>(this)->state_representation(state, params...);
    }

    template<typename Position>
    Move<Position> action_to_move(const Position &pos, const action_type & action, int player) {
        return Move<Position>{pos, pos + action.get_effect(player)};
    }

    template<typename BType,
            typename std::enable_if<
                    std::is_base_of<
                            Board<typename BType::piece_type>,
                            BType>
                    ::value,
                    int>
            ::type = 0>
    typename BType::move_type action_to_move(const BType &board, const action_type & action, int player) {
        typename BType::position_type pos = board.get_position_of_kin(player, action.get_piece_id());
        return {pos, pos + action.get_effect(player)};
    }

    template<typename SType,
            typename std::enable_if<
                    std::is_base_of<
                            State<typename SType::board_type>,
                            SType>
                    ::value,
                    int>
            ::type = 0>
    typename SType::move_type action_to_move(const SType &state, const action_type & action, int player) {
        return action_to_move(*state.get_board(), action, player);
    }

    template<typename Position>
    Move<Position> action_to_move(const Position &pos, int action_index, int player) {
        action_to_move(pos, get_actions_vec()[action_index], player);
    }

    template<typename BOSType>
    typename BOSType::move_type action_to_move(const BOSType &bos, int action_index, int player) {
        action_to_move(bos, get_actions_vec()[action_index], player);
    }


};

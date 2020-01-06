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
#include "Action.h"

#include <type_traits>


template<typename StateType, typename DerivedType>
class RepresenterBase {

private:
    // Convenience method for CRTP
    //
    const DerivedType* derived() const
    {
        return static_cast<const DerivedType*>(this);
    }
    DerivedType* derived()
    {
        return static_cast<DerivedType*>(this);
    }

public:
    using state_type = StateType;
    using board_type = typename state_type::board_type;
    using position_type = typename board_type::position_type;
    using move_type = typename board_type::move_type;
    using kin_type = typename state_type::kin_type;


    const auto & get_actions() const {
        return derived()->get_actions_();
    }

    // depending on the game/representation strategy of the subclass, a positional variable amount of parameters
    // can be passed to allow differing implementations without knowing each use case beforehand.
    template<typename... Params>
    torch::Tensor state_representation(const state_type &state, const Params &... params) {
        return derived()->state_representation_(state, params...);
    }

    template<typename Board>
    std::vector<unsigned int> get_action_mask(
            const Board &board,
            int player
    ) {
        return derived()->get_action_mask_(board, player);
    }

    template<typename Board, typename ActionType>
    static std::vector<unsigned int> get_action_mask(
            const std::vector<ActionType> &actions,
            const Board &board,
            int player
    ) {
        return DerivedType::get_action_mask_(actions, board, player);
    }

    template<typename ValueType, size_t Dim, typename KinType>
    inline Move<Position<ValueType, Dim>> action_to_move(
            const Position<ValueType, Dim> &pos,
            const Action<Position<ValueType, Dim>, KinType> & action,
            int player) const {
        return Move<Position<ValueType, Dim>>{pos, pos + action.get_effect()};
    }

    template<typename PieceType>
    inline typename Board<PieceType>::move_type action_to_move(
            const Board<PieceType> &board,
            const Action<typename PieceType::position_type, typename PieceType::kin_type> & action,
            int player) const {
        typename PieceType::position_type pos = board.get_position_of_kin(player, action.get_assoc_kin())->second;
        return {pos, pos + action.get_effect()};
    }

    template <typename BoardType>
    inline typename BoardType::move_type action_to_move(
            const State<BoardType> &state,
            const Action<typename BoardType::position_type, typename BoardType::kin_type> & action,
            int player) const {
        return action_to_move(*state.get_board(), action, player);
    }

    // BSPType = Board or State or Position Type
    template<typename BSPType>
    inline typename BSPType::move_type action_to_move(const BSPType &bos, int action_index, int player) const {
        return action_to_move(bos, get_actions()[action_index], player);
    }


};

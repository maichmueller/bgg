
#pragma once

#include "../../game/GameStateStratego.h"
#include "ActionRepresenter.h"
#include "Action.h"

#include "torch/torch.h"


class ActionRepStratego : public ActionRepBase<Action<typename BoardStratego::position_type,
                                                      typename BoardStratego::piece_type::kin_type>,
                                               GameStateStratego,
                                               ActionRepStratego> {

public:
    using state_type = GameStateStratego;
    using position_type = state_type::position_type;
    using kin_type = state_type::piece_type::kin_type;
    using move_type = state_type::move_type;
    using piece_type = state_type::piece_type;
    using board_type = state_type::board_type;
    using action_type = Action<position_type, kin_type>;
    static_assert(std::is_same_v<typename move_type::position_type, typename board_type::position_type>);

private:
    static void _build_actions_vector(size_t shape);

    torch::Tensor state_representation(int action, int player);

    static std::vector<action_type> get_action_vector() { return actions; }

    template <typename condition_type=std::tuple<kin_type, int, bool>>
    static torch::Tensor state_representation(int player, std::vector<condition_type> conditions=default_conditions);

    template <typename Board, typename Move>
    static std::vector<int> get_action_mask(
            const Board& board,
            const std::vector<Move>& action_arr,
            int player);

//    template <typename Board, typename Move, typename Position>
//    static void _enable_action_if_legal(std::vector<int> & action_mask, const Board& board,
//                                        int act_range_start,
//                                        const std::vector<Move> & action_arr,
//                                        const std::vector<int> & act_range,
//                                        const Position & pos, const Position & pos_to,
//                                        bool flip_board= false);
//
//
//    template <typename Move>
//    static int _find_action_idx(std::vector<Move> &vec_to_search, Move &action_to_find);

private:
    static const std::vector<action_type> actions;
    static const std::vector<std::tuple<kin_type, int, bool>> default_conditions;
    static bool conditions_computed;

};

if(!ActionRepStratego::conditions_computed)



    template<typename Board, typename Action, typename Position>
    void LogicStratego::_enable_action_if_legal(std::vector<int> &action_mask, const Board &board, int act_range_start,
                                                const std::vector <Action> &action_arr, const std::vector<int> &act_range,
                                                const Position &pos, const Position &pos_to, const bool flip_board) {

        Move move = {pos, pos_to};

        if(is_legal_move(board, move)) {
            Action action_effect;

            if(flip_board)
                action_effect = {pos[0] - pos_to[0], pos[1] - pos_to[1]};
            else
                action_effect = {pos_to[0] - pos[0], pos_to[1] - pos[1]};
            std::vector<Move<Position> > slice(act_range.size());
            for(unsigned long idx = 0; idx < slice.size(); ++idx) {
                slice[idx] = action_arr[act_range[idx]];
            }
            int idx = LogicStratego::_find_action_idx(slice, action_effect);
            action_mask[act_range_start + idx] = 1;
        }
    }
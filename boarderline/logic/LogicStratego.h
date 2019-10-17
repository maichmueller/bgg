//
// Created by Michael on 28/02/2019.
//

#pragma once

#include "../board/Board.h"
#include <functional>

class LogicStratego {

    template <typename Board, typename Move, typename Position>
    static void _enable_action_if_legal(std::vector<int> & action_mask, const Board& board,
                                        int act_range_start,
                                        const std::vector<Move> & action_arr,
                                        const std::vector<int> & act_range,
                                        const Position & pos, const Position & pos_to,
                                        const bool flip_board= false);

    template <typename Move>
    static int _find_action_idx(std::vector<Move> &vec_to_search, Move &action_to_find);

    template <typename Position>
    static inline void _invert_pos(int &len, Position &pos);

    template <typename Move>
    static inline void _invert_move(Move &move, int &len);

    static inline int _invert_team(int team);

    template <typename Board, typename Move>
    static std::vector<Move> _get_poss_moves(const Board & board, int player);

public:

    static const std::map<std::array<int,2>, int> battle_matrix;
    static std::map<std::array<int,2>, int> initialize_battle_matrix();

    template <typename Board, typename Move>
    static bool is_legal_move(const Board & board, const Move & move);

    template <typename Board, typename Move>
    static std::vector<Move> get_poss_moves(const Board & board, int player, bool flip_board=false);

    template <typename Board>
    static bool has_poss_moves(const Board & board, int player);

    template <typename Board, typename Move>
    static std::vector<int> get_action_mask(
            const Board& board,
            const std::vector<Move>& action_arr,
            const std::map<
                    std::array<int, 2>,
                    std::tuple<int, std::vector<int>>
                            > & piece_act_map,
            int player);

    static std::map<std::array<int,2>, int> get_battle_matrix() {return battle_matrix;}
    static int fight_outcome(int attacker, int defender) {return battle_matrix.find({attacker, defender})->second;}
    static int fight_outcome(std::array<int, 2> att_def) {return battle_matrix.find(att_def)->second;}


};

const std::map<std::array<int,2>, int> LogicStratego::battle_matrix = initialize_battle_matrix();

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
        std::vector<strat_move_base_t > slice(act_range.size());
        for(unsigned long idx = 0; idx < slice.size(); ++idx) {
            slice[idx] = action_arr[act_range[idx]];
        }
        int idx = LogicStratego::_find_action_idx(slice, action_effect);
        action_mask[act_range_start + idx] = 1;
    }
}
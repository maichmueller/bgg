//
// Created by Michael on 28/02/2019.
//

#ifndef STRATEGO_CPP_STRATEGOLOGIC_H
#define STRATEGO_CPP_STRATEGOLOGIC_H

#include "Board.h"
#include <functional>

class StrategoLogic {

    static void enable_action_if_legal(std::vector<int>& action_mask, const Board& board,
                                       int act_range_start,
                                       const std::vector<move_base_t >& action_arr,
                                       const std::vector<int>& act_range,
                                       const pos_t& pos, const pos_t& pos_to);

    static int find_action_idx(std::vector<move_base_t >& vec_to_search, move_base_t & action_to_find);

    static inline pos_t pos_ident(int& len, const pos_t & pos);
    static inline pos_t pos_invert(int& len, const pos_t & pos);

    static inline move_t move_ident(int& len, const move_t & move);
    static inline move_t move_invert(int& len, const move_t & move);

    static inline int team_ident(int team);
    static inline int team_invert(int team);

public:

    static const std::map<std::array<int,2>, int> battle_matrix;
    static std::map<std::array<int,2>, int> initialize_battle_matrix();
    static bool is_legal_move(const Board& board, const move_t& move, bool flip_teams=false);
    static std::vector<move_t> get_poss_moves(const Board& board, int player, bool flip_teams=false);
    static bool has_poss_moves(const Board& board, int player);
    static std::vector<int> get_action_mask(
            const Board& board,
            const std::vector<move_base_t >& action_arr,
            const std::map<std::array<int, 2>, std::tuple<int, std::vector<int>>>& piece_act_map,
            int player);

    static const std::map<std::array<int,2>, int> get_battle_matrix() {return battle_matrix;}
    static int fight_outcome(int attacker, int defender) {return battle_matrix.find({attacker, defender})->second;}
    static int fight_outcome(std::array<int, 2> att_def) {return battle_matrix.find(att_def)->second;}

};

#endif //STRATEGO_CPP_STRATEGOLOGIC_H

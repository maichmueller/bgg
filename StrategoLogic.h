//
// Created by Michael on 28/02/2019.
//

#ifndef STRATEGO_CPP_STRATEGOLOGIC_H
#define STRATEGO_CPP_STRATEGOLOGIC_H

#include "Board.h"
#include <functional>

class StrategoLogic {

    static void enable_legal_action(std::vector<int>& action_mask, const Board& board,
                                    const std::vector<std::vector<int>>& action_arr,
                                    const std::vector<int>& act_range,
                                    const pos_type& pos, const pos_type& pos_to);

    static int find_action_idx(std::vector<std::vector<int>>& vec_to_search, std::vector<int>& action_to_find);

    static pos_type pos_ident(int& len, const pos_type & pos);
    static pos_type pos_invert(int& len, const pos_type & pos);

    static std::vector<pos_type> move_ident(int& len, const std::vector<pos_type> & move);
    static std::vector<pos_type> move_invert(int& len, const std::vector<pos_type> & move);

    static int team_ident(int team);
    static int team_invert(int team);

public:

    static const std::map<std::array<int,2>, int> battle_matrix;
    static std::map<std::array<int,2>, int> initialize_battle_matrix();
    static bool is_legal_move(const Board& board, const std::vector<pos_type>& move, bool flip_teams=false);
    static std::vector<std::vector<pos_type>> get_poss_moves(const Board& board, int player, bool flip_teams=false);
    static bool has_poss_moves(const Board& board, int player);
    static std::vector<int> get_action_mask(const Board& board, const std::vector<std::vector<int>>& action_arr,
                                       const std::map<std::vector<int>, std::vector<int>>& piece_act_map, int player);

    static const std::map<std::array<int,2>, int> get_battle_matrix() {return battle_matrix;}
    static int fight_outcome(int attacker, int defender) {return battle_matrix.find({attacker, defender})->second;}
    static int fight_outcome(std::array<int, 2> att_def) {return battle_matrix.find(att_def)->second;}

};

#endif //STRATEGO_CPP_STRATEGOLOGIC_H

//
// Created by Michael on 28/02/2019.
//

#ifndef STRATEGO_CPP_STRATEGOLOGIC_H
#define STRATEGO_CPP_STRATEGOLOGIC_H

#include "Board.h"
#include <functional>

class StrategoLogic {

    static void enable_legal_action(vector<int>& action_mask, Board& board,
                                      vector<vector<int>>& action_arr,
                                      vector<int>& act_range,
                                      pos_type& pos, pos_type& pos_to);

    static int find_action_idx(vector<vector<int>>& vec_to_search, vector<int>& action_to_find);

    static pos_type pos_ident(int& len, pos_type & pos);
    static pos_type pos_invert(int& len, pos_type & pos);

    static vector<pos_type> move_ident(int& len, vector<pos_type> & move);
    static vector<pos_type> move_invert(int& len, vector<pos_type> & move);

    static int team_ident(int team);
    static int team_invert(int team);

public:

    static const map<array<int,2>, int> battle_matrix;
    static map<array<int,2>, int> initialize_battle_matrix();
    static bool is_legal_move(Board& board, vector<pos_type>& move, bool flip_teams=false);
    static vector<vector<pos_type>> get_poss_moves(Board& board, int player, bool flip_teams=false);
    static bool has_poss_moves(Board& board, int player);
    static vector<int> get_action_mask(Board& board, vector<vector<int>>& action_arr,
            map<vector<int>, vector<int>>& piece_act_map, int player);

    static const map<array<int,2>, int> get_battle_matrix() {return battle_matrix;}
    static int fight_outcome(int attacker, int defender) {return battle_matrix.find({attacker, defender})->second;}
    static int fight_outcome(array<int, 2> att_def) {return battle_matrix.find(att_def)->second;}

};

#endif //STRATEGO_CPP_STRATEGOLOGIC_H

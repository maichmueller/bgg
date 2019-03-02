//
// Created by Michael on 28/02/2019.
//

#ifndef STRATEGO_CPP_STRATEGOLOGIC_H
#define STRATEGO_CPP_STRATEGOLOGIC_H

#include "Board.h"

class StrategoLogic {

public:

    static const map<array<int,2>, int> battle_matrix;
    static map<array<int,2>, int> initialize_battle_matrix();
    static bool is_legal_move(Board& board, vector<pos_type>& move);
    static vector<vector<pos_type>> get_poss_moves(Board& board, int player);
    static bool has_poss_moves(Board& board, int player);
    static vector<int> get_action_mask(Board& board, vector<vector<int>>& action_arr,
            map<vector<int>, vector<int>>& piece_act_map, int player);

    static const map<array<int,2>, int> get_battle_matrix() {return battle_matrix;}
    static int fight_outcome(int attacker, int defender) {return battle_matrix.find({attacker, defender})->second;}
    static int fight_outcome(array<int, 2> att_def) {return battle_matrix.find(att_def)->second;}

};

#endif //STRATEGO_CPP_STRATEGOLOGIC_H

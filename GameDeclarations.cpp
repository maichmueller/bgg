//
// Created by Michael on 27/02/2019.
//

#include "GameDeclarations.h"


const vector<int> ActionRep::available_types_s = {0, 1, 2, 2, 2, 3, 3, 10, 11, 11};
const vector<int> ActionRep::available_types_m = {0, 1, 2, 2, 2, 2, 2, 3, 3, 3, 4,
                                                     4, 4, 5, 5, 6, 10, 11, 11, 11, 11};
const vector<int> ActionRep::available_types_l = {0, 1, 2, 2, 2, 2, 2, 2, 2, 2, 3, 3,
                                                     3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5,
                                                     6, 6, 6, 6, 7, 7, 7, 8, 8, 9, 10,
                                                     11, 11, 11, 11, 11, 11};

vector<pos_type> action_ar_s_copy(64);
vector<pos_type> action_ar_m_copy(124);
vector<pos_type> action_ar_l_copy(228);
map<vector<int>, vector<int>> piece_act_map_s_copy;
map<vector<int>, vector<int>> piece_act_map_m_copy;
map<vector<int>, vector<int>> piece_act_map_l_copy;

auto dummy1 = ActionRep::fill_act_vector(action_ar_s_copy, ActionRep::available_types_s, piece_act_map_s_copy);
auto dummy2 = ActionRep::fill_act_vector(action_ar_m_copy, ActionRep::available_types_m, piece_act_map_m_copy);
auto dummy3 = ActionRep::fill_act_vector(action_ar_l_copy, ActionRep::available_types_l, piece_act_map_l_copy);

const vector<vector<int>> ActionRep::action_ar_s = action_ar_s_copy;
const map<vector<int>, vector<int>> ActionRep::piece_act_map_s = piece_act_map_s_copy;

const vector<vector<int>> ActionRep::action_ar_m = action_ar_m_copy;
const map<vector<int>, vector<int>> ActionRep::piece_act_map_m = piece_act_map_m_copy;

const vector<vector<int>> ActionRep::action_ar_l = action_ar_l_copy;
const map<vector<int>, vector<int>> ActionRep::piece_act_map_l = piece_act_map_l_copy;




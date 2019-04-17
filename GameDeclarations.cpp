//
// Created by Michael on 27/02/2019.
//

#include "GameDeclarations.h"


std::vector<pos_type> action_ar_s_copy(64);
std::vector<pos_type> action_ar_m_copy(124);
std::vector<pos_type> action_ar_l_copy(228);
std::map<std::vector<int>, std::vector<int>> piece_act_map_s_copy;
std::map<std::vector<int>, std::vector<int>> piece_act_map_m_copy;
std::map<std::vector<int>, std::vector<int>> piece_act_map_l_copy;

auto dummy1 = ActionRep::fill_act_vector(action_ar_s_copy, GameDeclarations::available_types_s, piece_act_map_s_copy);
auto dummy2 = ActionRep::fill_act_vector(action_ar_m_copy, GameDeclarations::available_types_m, piece_act_map_m_copy);
auto dummy3 = ActionRep::fill_act_vector(action_ar_l_copy, GameDeclarations::available_types_l, piece_act_map_l_copy);

const std::vector<std::vector<int>> ActionRep::action_ar_s = action_ar_s_copy;
const std::map<std::vector<int>, std::vector<int>> ActionRep::piece_act_map_s = piece_act_map_s_copy;

const std::vector<std::vector<int>> ActionRep::action_ar_m = action_ar_m_copy;
const std::map<std::vector<int>, std::vector<int>> ActionRep::piece_act_map_m = piece_act_map_m_copy;

const std::vector<std::vector<int>> ActionRep::action_ar_l = action_ar_l_copy;
const std::map<std::vector<int>, std::vector<int>> ActionRep::piece_act_map_l = piece_act_map_l_copy;




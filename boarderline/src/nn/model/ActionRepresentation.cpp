//
// Created by michael on 08.08.19.
//

#include "ActionRepresenter.h"


std::vector<Position> action_ar_s_copy(64);
std::vector<Position> action_ar_m_copy(124);
std::vector<Position> action_ar_l_copy(228);
std::map<std::array<int, 2>, std::tuple<int, std::vector<int>>> piece_act_map_s_copy;
std::map<std::array<int, 2>, std::tuple<int, std::vector<int>>> piece_act_map_m_copy;
std::map<std::array<int, 2>, std::tuple<int, std::vector<int>>> piece_act_map_l_copy;

auto dummy1 = ActionRep::fill_act_vector(action_ar_s_copy, GameDeclarations::available_types_s, piece_act_map_s_copy);
auto dummy2 = ActionRep::fill_act_vector(action_ar_m_copy, GameDeclarations::available_types_m, piece_act_map_m_copy);
auto dummy3 = ActionRep::fill_act_vector(action_ar_l_copy, GameDeclarations::available_types_l, piece_act_map_l_copy);

const std::vector<strat_move_base_t > ActionRep::action_ar_s = action_ar_s_copy;
const decltype(ActionRep::piece_act_map_s) ActionRep::piece_act_map_s = piece_act_map_s_copy;

const std::vector<strat_move_base_t > ActionRep::action_ar_m = action_ar_m_copy;
const decltype(ActionRep::piece_act_map_m) ActionRep::piece_act_map_m = piece_act_map_m_copy;

const std::vector<strat_move_base_t > ActionRep::action_ar_l = action_ar_l_copy;
const decltype(ActionRep::piece_act_map_l) ActionRep::piece_act_map_l = piece_act_map_l_copy;
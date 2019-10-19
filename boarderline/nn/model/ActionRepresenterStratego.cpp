//
// Created by michael on 07.09.19.
//

#include "ActionRepresenterStratego.h"
#include "../../game/GameUtilsStratego.h"


torch::Tensor ActionRepStratego::state_representation(int action, int player) {
    if(!conditions_set) {
        auto type_counter = utils::counter(GameUtilsStratego<kin_type, position_type>::get_available_types(m_board.get_shape()));
        conditions_torch_rep = StateRepresentation::create_conditions(type_counter, 0);
        conditions_set = true;
    }

    return StateRepresentation::b2s_cond_check(
            m_board,
            conditions_torch_rep,
            player);
}

void ActionRepStratego::assign_actors(ActionRepStratego::game_state_type & gs) {
    for(const auto& entry: gs.get_board()) {
        const auto& piece = entry.second;
        if(!piece->is_null() && piece->get_kin() != kin_type{99, 99};
                actors[piece->get_team()][piece->get_kin()] = piece;
    }
}

int ActionRepStratego::_fill_act_vector(std::vector<action_type> &action_ar, int board_len) {

    const auto & available_types = GameDeclarations::get_available_types(board_len);
    int curr_idx = 0;
    int curr_type = -1;
    int curr_version = -1;
    /*
        we want to iterate over every type of piece (as often as this type exists)
        and add the move-changes, i.e. the pos-vector that will need to be added to the
        current position of the piece. These quasi-moves for each type are added in
        sequence to the action_arr.
    */
    for (auto &type : available_types) {
        if (0 < type && type < 11) {
            if (curr_type != type) {
                curr_type = type;
                curr_version = 0;
            } else
                curr_version += 1;

            int start_idx = curr_idx;
            // if its of type 2 it can reach further -> encoded in the step_len
            int step_len = 1;
            if (type == 2)
                step_len = 4;
            // add all four directions in which the piece can walk
            for (int i = 1; i < step_len + 1; ++i) {
                action_ar[curr_idx] = {0, i};
                action_ar[curr_idx + 1] = {i, 0};
                action_ar[curr_idx + 2] = {-i, 0};
                action_ar[curr_idx + 3] = {0, -i};
                curr_idx += 4;
            }
            // here we remember the index numbers of the moves, that are assigned to this
            // specific piece type and version.
            std::vector<int> this_assign(static_cast<unsigned long> (curr_idx - start_idx));
            std::iota(this_assign.begin(), this_assign.end(), start_idx);
            piece_act_map[{curr_type, curr_version}] = std::tuple(start_idx, this_assign);
        }
    }
    return 0;
}

move_type ActionRepStratego::action_to_move(int action, int action_dim, int board_len, const std::unordered_map<
        std::tuple<int, int>,
        std::shared_ptr<piece_type>,
        tuple::hash<std::tuple<int, int> >,
        eqcomp_tuple::eqcomp<std::tuple<int, int> >
        > &actors, int player)
{

    if (action < 0 || action >= action_dim)
        throw std::invalid_argument("Action index out of range.");

    int type = -1;
    int version = -1;
    // Iterate over the piece_action_map to find the type and the version of the
    // piece belonging to this action index.
    for (auto const&[type_ver_vec, ind_vec_and_idx] : ActionRep::get_act_map(board_len)) {
        auto ind_vec = std::get<1>(ind_vec_and_idx);
        auto find_action_ind = std::find(ind_vec.begin(), ind_vec.end(), action);
        if (find_action_ind != ind_vec.end()) {
            type = type_ver_vec[0];
            version = type_ver_vec[1];
            break;
        }
    }
    // error checking. If we didn't find anything, then there is something wrong in the logic.
    if (type == -1) {
        std::string err_msg = "Couldn't find type and version for action index " + std::to_string(action) + ".";
        throw std::logic_error(err_msg);
    }
    std::shared_ptr<Piece> actor = actors.at(std::make_tuple(type, version));
    Position move_change = ActionRep::get_act_rep(board_len)[action];
    Position curr_pos = actor->get_position(player, board_len);
    Position new_pos = {curr_pos[0] + move_change[0], curr_pos[1] + move_change[1]};
    strat_move_t move{curr_pos, new_pos};
    return move;
    int board_len = m_board.get_shape();
    int action_dim = ActionRep::get_act_rep(board_len).size();
    return ActionRep::action_to_move(action, action_dim, board_len, actors.at(player), player);
}
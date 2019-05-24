//
// Created by michael on 14.04.19.
//

#include "AgentReinforce.h"


move_type AgentReinforceBase::_action_to_move(int action) {

    if(action < 0 || action >= action_dim)
        throw std::invalid_argument("Action index out of range.");

    int type = -1;
    int version = -1;
    // Iterate over the piece_action_map to find the type and the version of the
    // piece belonging to this action index.
    for( auto const& [type_ver_vec, ind_vec] : ActionRep::get_act_map(board_len)) {
        auto find_action_ind = std::find(ind_vec.begin(), ind_vec.end(), action);
        if(find_action_ind != ind_vec.end()) {
            type = type_ver_vec[0];
            version = type_ver_vec[1];
            break;
        }
    }
    // error checking. If we didn't find anything, then there is something wrong in the logic.
    if(type == -1){
        // something is logically wrong. Couldn't find the type and version belonging to the action
        std::string err_msg = "Couldn't find type and version for action index " + std::to_string(action) + ".";
        throw std::logic_error(err_msg);
    }
    std::shared_ptr<Piece> actor = actors[{type, version}];
    pos_type move_change = ActionRep::get_act_rep(board_len)[action];
    pos_type curr_pos = actor->get_position();
    pos_type new_pos = {curr_pos[0] + move_change[0], curr_pos[1] + move_change[1]};
    move_type move {curr_pos, new_pos};
    return move;
}


void AgentReinforceBase::install_board(const Board &board) {
    board_len = board.get_board_len();
    auto avail_types = GameDeclarations::get_available_types(board_len);
    type_counter = utils::counter(avail_types);

    for(auto& entry: board) {
        auto piece = entry.second;
        if(piece->get_team() == team) {
            actors[{piece->get_type(), piece->get_version()}] = piece;
        }
        // find the position of this type in the avail types.
        auto entry_it = std::find(avail_types.begin(), avail_types.end(), piece->get_type());

        if(entry_it != avail_types.end()) {
            // type has been found, erase the entry.
            avail_types.erase(entry_it);
        }
        else {
            // reaching here means something is wrong with the logic. We have too many pieces of this type
            std::string err_msg = "Too many pieces of type " + std::to_string(piece->get_type()) + " found.";
            throw std::logic_error(err_msg);
        }
    }
}

/// Forwarders to the overarching state rep
bool AlphaZeroAgent::check_condition(const std::shared_ptr<Piece> &piece, int team, int type, int version,
                                     bool hidden) {
    return torch_utils::StateRep::check_condition(piece, team, type, version, hidden);
}
std::vector<std::tuple<int, int, int, bool>> AlphaZeroAgent::create_conditions() {
    return torch_utils::StateRep::create_conditions(type_counter, team);
}
torch::Tensor AlphaZeroAgent::board_to_state_rep(const Board &board) {
    return torch_utils::StateRep::b2s_cond_check(board, state_dim, board_len, conditions);
}


void AlphaZeroAgent::install_board(const Board &board) {
    AgentReinforceBase::install_board(board);
    conditions = create_conditions();
    state_dim = conditions.size();
}


move_type AlphaZeroAgent::decide_move(const Board &board) {
    model->to(torch_utils::GLOBAL_DEVICE::get_device());
    torch::Tensor board_state = board_to_state_rep(board);
    auto [pi, v] = model->predict(board_state);

    int action = pi.argmax().item<int64_t>();

    move_type move = _action_to_move(action);

    return move;
}
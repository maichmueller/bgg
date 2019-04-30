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
    type_counter = counter(avail_types);

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
            // something is logically wrong. We have too many pieces of this type
            std::string err_msg = "Too many pieces of type " + std::to_string(piece->get_type()) + " found.";
            throw std::logic_error(err_msg);
        }
    }
}


std::map<int, unsigned int> AgentReinforceBase::counter(const std::vector<int>& vals) {
    std::map<int, unsigned int> rv;

    for(auto val = vals.begin(); val != vals.end(); ++val) {
        rv[*val]++;
    }

    return rv;
}


bool AlphaZeroAgent::check_condition(const std::shared_ptr<Piece>& piece,
                                     int team,
                                     int type,
                                     int version,
                                     bool hidden) const {

    if(team == 0) {
        if(!hidden){
            // if it is about team 0, the 'hidden' status is unimportant
            // (since the alpha zero agent alwazs plays from the perspective
            // of player 0, therefore it can see all its own pieces).
            bool eq_team = piece->get_team() == team;
            bool eq_type = piece->get_type() == type;
            bool eq_vers = piece->get_version() == version;
            return eq_team && eq_type && eq_vers;
        }
        else {
            // 'hidden' is only important for the single condition that specifically
            // checks for this property (information about own pieces visible or not).
            bool eq_team = piece->get_team() == team;
            bool hide = piece->get_flag_hidden() == hidden;
            return eq_team && hide;
        }
    }
    else if(team == 1) {
        // for team 1 we only get the info about type and version if it isn't hidden
        // otherwise it will fall into the 'hidden' layer
        if(!hidden) {
            if(piece->get_flag_hidden())
                return false;
            else {
                bool eq_team = piece->get_team() == team;
                bool eq_type = piece->get_type() == type;
                bool eq_vers = piece->get_version() == version;
                return eq_team && eq_type && eq_vers;
            }
        }
        else {
            bool eq_team = piece->get_team() == team;
            bool hide = piece->get_flag_hidden() == hidden;
            return eq_team && hide;
        }
    }
    else {
        // only the obstacle should reach here
        return piece->get_team() == team;
    }
}


std::vector<std::tuple<int, int, int, bool>> AlphaZeroAgent::create_conditions() {

    std::vector<std::tuple<int, int, int, bool>> conditions(21);

    // own team 0
    // flag, 1, 2, 3, 4, ..., 10, bombs UNHIDDEN
    for(auto entry : type_counter) {
        int type = entry.second;
        for (int version = 1; version <= entry.second; ++version) {
            conditions.emplace_back(std::make_tuple(0, type, version, false));
        }
    }
    // all own pieces HIDDEN
    // Note: type and version info are being "short-circuited" (unused)
    // in the check in this case (thus -1)
    conditions.emplace_back(std::make_tuple(0, -1, -1, false));

    // enemy team 1
    // flag, 1, 2, 3, 4, ..., 10, bombs UNHIDDEN
    for(auto entry : type_counter) {
        int type = entry.second;
        for (int version = 1; version <= entry.second; ++version) {
            conditions.emplace_back(std::make_tuple(1, type, version, false));
        }
    }
    // all enemy pieces HIDDEN
    // Note: type and version info are being "short-circuited" (unused)
    // in the check in this case (thus -1)
    conditions.emplace_back(std::make_tuple(1, -1, -1, false));

    return conditions;
}

torch::Tensor AlphaZeroAgent::board_to_state_rep(const Board &board) {
    /*
     * We are trying to build a state representation of a Stratego board.
     * To this end, i will define conditions that are evaluated for each
     * piece on the board. These conditions are checked in sequence.
     * Each condition receives its own layer with 0's everywhere, except
     * for where the specific condition was true, a 1.
     * In short: x conditions -> x layers (one for each condition)
     */

    torch::Tensor board_state_rep = torch::zeros({1, state_dim, board_len, board_len});
    auto board_state_access = board_state_rep.accessor<int, 4> ();
    for(const auto& pos_piece : board) {
        pos_type pos = pos_piece.first;
        std::shared_ptr<Piece> piece = pos_piece.second;
        if(!piece->is_null()) {
            for(auto [i, cond_it] = std::make_pair(0, conditions.begin()); cond_it != conditions.end(); ++i, ++cond_it) {
                // unpack the condition
                auto [team, type, vers, hidden] = *cond_it;
                // write the result of the condition check to the tensor
                board_state_access[0][i][pos[0]][pos[1]] = check_condition(piece, team, type, vers, hidden);
            }
        }
    }
    // send the tensor to the global device
    board_state_rep.to(torch_utils::GLOBAL_DEVICE::get_device());

    return board_state_rep;
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
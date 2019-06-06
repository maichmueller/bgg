//
// Created by michael on 14.04.19.
//

#include "AgentReinforce.h"


void AgentReinforceBase::install_board(const Board &board) {
    board_len = board.get_board_len();
    type_counter = utils::counter(GameDeclarations::get_available_types(board_len));

    for(const auto& entry: board) {
        auto piece = entry.second;
        if(piece->get_team() == team) {
            actors[{piece->get_type(), piece->get_version()}] = piece;
        }
    }
}

/// Forwarders to the overarching state rep
bool AlphaZeroAgent::check_condition(const std::shared_ptr<Piece> &piece, int team, int type, int version,
                                     bool hidden) {
    return StateRepresentation::check_condition(piece, team, type, version, hidden);
}
std::vector<std::tuple<int, int, int, bool>> AlphaZeroAgent::create_conditions() {
    return StateRepresentation::create_conditions(type_counter, team);
}
torch::Tensor AlphaZeroAgent::board_to_state_rep(const Board &board) {
    return StateRepresentation::b2s_cond_check(board, state_dim, board_len, conditions);
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

    move_type move = ActionRep::action_to_move(action, action_dim, board_len, actors);

    return move;
}
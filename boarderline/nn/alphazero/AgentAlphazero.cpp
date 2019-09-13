//
// Created by michael on 13.08.19.
//

#include "AgentAlphazero.h"

/// Forwarders to the overarching state rep

bool AlphaZeroAgent::check_condition(const std::shared_ptr<Piece> &piece, int team, int type, int version,
                                     bool hidden) {
    return StateRepresentation::check_condition(piece, team, type, version, hidden);
}

std::vector<std::tuple<int, int, int, bool>> AlphaZeroAgent::create_conditions() {
    return StateRepresentation::create_conditions(Base::m_type_counter,
                                                  Base::m_team);
}

torch::Tensor AlphaZeroAgent::board_to_state_rep(const Board &board) {
    return StateRepresentation::b2s_cond_check(board,
                                               Base::m_conditions);
}


void AlphaZeroAgent::install_board(const Board &board) {
    Base::install_board(board);
    Base::m_conditions = create_conditions();
    Base::m_state_dim = Base::m_conditions.size();
}


strat_move_t AlphaZeroAgent::decide_move(const Board &board) {
    torch::Tensor board_state = board_to_state_rep(board);
    auto [pi, v] = Base::m_model->predict(board_state);

    int action = pi.argmax().template item<int64_t>();

    strat_move_t move = ActionRep::action_to_move(action, Base::m_action_dim, Base::m_board_len, Base::m_actors, m_team);

    return move;
}
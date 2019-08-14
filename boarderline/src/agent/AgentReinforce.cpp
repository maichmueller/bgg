//
// Created by michael on 14.04.19.
//

#include "AgentReinforce.h"
#include "../alphazero/ActionRepresentation.h"

void AgentReinforceBase::install_board(const Board &board) {
    m_board_len = board.get_board_len();
    m_type_counter = utils::counter(GameDeclarations::get_available_types(m_board_len));

    for(const auto& entry: board) {
        auto piece = entry.second;
        if(piece->get_team() == m_team) {
            m_actors[{piece->get_type(), piece->get_version()}] = piece;
        }
    }
}


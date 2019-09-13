//
// Created by michael on 07.09.19.
//

#include "ActionRepresenterStratego.h"


torch::Tensor ActionRepStratego::state_representation(int action, int player) {
    if(!conditions_set) {
        auto type_counter = utils::counter(GameDeclarations::get_available_types(m_board.get_shape()));
        conditions_torch_rep = StateRepresentation::create_conditions(type_counter, 0);
        conditions_set = true;
    }

    return StateRepresentation::b2s_cond_check(
            m_board,
            conditions_torch_rep,
            player);
}
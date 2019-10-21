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

void ActionRepStratego::_build_actions_vector(size_t shape) {
    std::vector<action_type> actions;
    const auto & available_types = GameUtilsStratego<kin_type, position_type>::get_available_types(shape);
    int curr_kin = -1;
    int curr_kin_version = -1;
    /*
        we want to iterate over every type of piece (as often as this type exists)
        and add the actions corresponding to its possible moves.
    */
    for (auto &kin : available_types) {
        if (0 < kin && kin < 11) {
            if (curr_kin != kin) {
                curr_kin = kin;
                curr_kin_version = 0;
            } else
                curr_kin_version += 1;

            // if its of kin 2 it can reach further -> encoded in the max_steps
            int max_steps = 1;
            if (kin == 2)
                max_steps = 4;
            // add all four directions in which the piece can walk
            for (int i = 1; i < max_steps + 1; ++i) {
                actions.emplace_back(
                        action_type(
                                position_type(0, i),
                                kin_type(curr_kin, curr_kin_version)
                        )
                );
                actions.emplace_back(
                        action_type(
                                position_type(i, 0),
                                kin_type(curr_kin, curr_kin_version)
                        )
                );
                actions.emplace_back(
                        action_type(
                                position_type(-i, 0),
                                kin_type(curr_kin, curr_kin_version)
                        )
                );
                actions.emplace_back(
                        action_type(
                                position_type(0, -i),
                                kin_type(curr_kin, curr_kin_version)
                        )
                );
            }
        }
    }
}
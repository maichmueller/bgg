//
// Created by michael on 07.09.19.
//

#include "ActionRepresenterStratego.h"
#include "azpp/utils/torch_utils.h"

/**
* We are trying to build a state representation of a Stratego board.
* To this end, 'conditions' are evaluated for each
* piece on the board. These 'conditions' are checked in sequence.
* Each condition receives its own layer with 0's everywhere, except
* for where the specific condition was true, which holds a 1.
* |==========================================================================|
* |              In short: x conditions -> x binary layers                   |
* |                            (one for each condition)                      |
* |==========================================================================|
*
* Parameters
* ----------
* @param board, the board whose representation we want
* @param conditions, std::vector of tuples for the conditions,
*      on which to check the board
* @param player, int deciding which player's representation we're seeking
*
* Returns
* -------
* @return tensor of 0's and 1's on the positions for which the relevant condition was
*      true (1) or wrong (0)
**/
ActionRepStratego::condition_container ActionRepStratego::_build_conditions_vector(size_t shape) {
    std::vector<std::tuple<kin_type, int, bool>> conditions(0);
    int own_team = 0;
    auto counter = utils::counter(LogicStratego<board_type>::get_available_types(shape));
    // own m_team 0
    // [flag, 1, 2, 3, 4, ..., 10, bombs] UNHIDDEN
    for(const auto& entry : counter) {
        int type = entry.first;
        for (decltype(entry.second) version = 0; version < entry.second; ++version) {
            conditions.emplace_back(std::make_tuple(kin_type(type, version), own_team, false));
        }
    }
    // [all own pieces] HIDDEN
    // Note: type and version info are unused
    // in the check in this case (thus -1)
    conditions.emplace_back(std::make_tuple(kin_type(-1, -1), own_team, true));

    // enemy m_team 1
    // [flag, 1, 2, 3, 4, ..., 10, bombs] UNHIDDEN
    for(const auto& entry : counter) {
        int type = entry.first;
        for (decltype(entry.second) version = 0; version < entry.second; ++version) {
            conditions.emplace_back(std::make_tuple(kin_type(type, version), 1-own_team, false));
        }
    }
    // [all enemy pieces] HIDDEN
    // Note: type and version info are unused
    // in the check in this case (thus -1)
    conditions.emplace_back(std::make_tuple(kin_type(-1, -1), 1-own_team, true));
    return conditions;
}


std::vector<ActionRepStratego::action_type> ActionRepStratego::_build_actions_vector(size_t shape) {
    std::vector<action_type> acts;
    const auto &available_types = LogicStratego<board_type>::get_available_types(shape);
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
                acts.emplace_back(
                        action_type(
                                position_type(0, i),
                                kin_type(curr_kin, curr_kin_version)
                        )
                );
                acts.emplace_back(
                        action_type(
                                position_type(i, 0),
                                kin_type(curr_kin, curr_kin_version)
                        )
                );
                acts.emplace_back(
                        action_type(
                                position_type(-i, 0),
                                kin_type(curr_kin, curr_kin_version)
                        )
                );
                acts.emplace_back(
                        action_type(
                                position_type(0, -i),
                                kin_type(curr_kin, curr_kin_version)
                        )
                );
            }
        }
    }
    return acts;
}






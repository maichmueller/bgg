//
// Created by michael on 07.09.19.
//

#include "ActionRepresenterStratego.h"
#include "../../game/GameUtilsStratego.h"
#include "../../utils/torch_utils.h"


void ActionRepStratego::_build_actions_vector(size_t shape) {
    std::vector<action_type> actions;
    const auto &available_types = GameUtilsStratego<kin_type, position_type>::get_available_types(shape);
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

template<typename Piece>
bool
ActionRepStratego::_check_condition(
        const std::shared_ptr<Piece> &piece,
        const kin_type &kin,
        int team,
        bool hidden,
        bool flip_teams) {

    // if we flip the teams, we want pieces of m_team 1 to appear as m_team 0
    // and vice versa
    int team_piece = flip_teams ? 1 - piece->get_team() : piece->get_team();

    if (team == 0) {
        if (!hidden) {
            // if it is about m_team 0, the 'hidden' status is unimportant
            // (since the alpha zero agent always plays from the perspective
            // of player 0, therefore it can see all its own pieces)
            bool eq_team = team_piece == team;
            bool eq_kin = piece->get_kin() == kin;
            return eq_team && eq_kin;
        } else {
            // 'hidden' is only important for the single condition that specifically
            // checks for this property (information about own pieces visible or not).
            bool eq_team = team_piece == team;
            bool hide = piece->get_flag_hidden() == hidden;
            return eq_team && hide;
        }
    } else if (team == 1) {
        // for m_team 1 we only get the info about type and version if it isn't hidden
        // otherwise it will fall into the 'hidden' layer
        if (!hidden) {
            if (piece->get_flag_hidden())
                return false;
            else {
                bool eq_team = team_piece == team;
                bool eq_kin = piece->get_kin() == kin;
                return eq_team && eq_kin;
            }
        } else {
            bool eq_team = team_piece == team;
            bool hide = piece->get_flag_hidden() == hidden;
            return eq_team && hide;
        }
    } else {
        // only the obstacle should reach here
        return team_piece == team;
    }
}

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

template<typename condition_type>
torch::Tensor ActionRepStratego::state_representation(
        const state_type & state,
        int player,
        std::vector<condition_type> conditions) {

    auto board = state.get_board();
    auto shape = board->get_shape();
    auto starts = board->get_starts();
    int state_dim = conditions.size();
    bool flip_teams = static_cast<bool> (player);

    std::function<position_type (position_type&)> canonize_pos = [&](position_type& pos) {return pos;};
    std::function<int(int)> canonize_team = [](int team) {return team;};

    if(flip_teams) {
        canonize_pos = [&](position_type& pos) {return pos.invert(starts, shape);};
        canonize_team = [](int team) {return 1-team;};
    }

    auto options =
            torch::TensorOptions()
                    .dtype(torch::kFloat32)
                    .layout(torch::kStrided)
                    .device(torch_utils::GLOBAL_DEVICE::get_device())
                    .requires_grad(true);
    // the dimensions here are as follows:
    // 1 = batch_size (in this case obviously only 1)
    // state_dim = dimension of the state rep, i.e. how many layers of the conditions
    // shape[0] = first board dimension
    // shape[1] = second board dimension
    torch::Tensor board_state_rep = torch::zeros({1, state_dim, shape[0], shape[1]}, options);

    for(const auto& pos_piece : *board) {
        Position pos = pos_piece.first;
        pos = canonize_pos(pos);
        auto piece = pos_piece.second;
        if(!piece->is_null()) {
            for(auto&& [i, cond_it] = std::make_tuple(0, conditions.begin());
                cond_it != conditions.end();
                ++i, ++cond_it)
            {
                // unpack the condition
                auto [kin, team, hidden] = *cond_it;
                // write the result of the condition check to the tensor
                board_state_rep[0][i][pos[0]][pos[1]] = check_condition(piece, kin, team, hidden, flip_teams);
            }
        }
    }
    return board_state_rep;
}


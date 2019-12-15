
#pragma once

#include "azpp/nn.h"
#include "torch/torch.h"

#include "game/StateStratego.h"


class ActionRepStratego :
        public ActionRepBase<
                Action<typename BoardStratego::position_type,
                        typename BoardStratego::kin_type>,
        StateStratego,
        ActionRepStratego
        > {

public:
    using state_type = StateStratego;
    using position_type = state_type::position_type;
    using kin_type = state_type::piece_type::kin_type;
    using move_type = state_type::move_type;
    using piece_type = state_type::piece_type;
    using board_type = state_type::board_type;
    using action_type = Action<position_type, kin_type>;
    using condition_container = std::vector<std::tuple<typename BoardStratego::piece_type::kin_type, int, bool>>;
    static_assert(std::is_same_v<typename move_type::position_type, typename board_type::position_type>);

    explicit ActionRepStratego(size_t shape)
            : m_actions(_build_actions(shape)),
              m_conditions(_build_default_conditions(shape)) {}

    ActionRepStratego(size_t shape, const condition_container & conditions)
            : m_actions(_build_actions(shape)),
              m_conditions(conditions) {}


    torch::Tensor state_representation(const state_type &state,
                                       int player) {
        return state_representation(state, player, m_conditions);
    }

    template<typename condition_type=std::tuple<kin_type, int, bool>>
    torch::Tensor state_representation(const state_type &state,
                                       int player,
                                       std::vector<condition_type> conditions);

    [[nodiscard]] const std::vector<action_type> & get_actions_vec() const { return m_actions; }

    [[nodiscard]] const std::vector<action_type> & get_conditions() const { return m_actions; }

    template<typename Board>
    std::vector<int> get_action_mask(
            const Board &board,
            int player
    );

    template<typename Board>
    static std::vector<int> get_action_mask(
            const std::vector<action_type> &actions,
            const Board &board,
            int player
    );


private:
    static std::vector<action_type> _build_actions(size_t shape);

    static condition_container _build_default_conditions(size_t shape);

    template<typename Piece>
    inline bool _check_condition(const std::shared_ptr<Piece> &piece,
                                 const kin_type &kin,
                                 int team,
                                 bool hidden,
                                 bool flip_teams = false
    );

    const std::vector<action_type> m_actions;
    const condition_container m_conditions;
};


template<typename condition_type>
torch::Tensor ActionRepStratego::state_representation(
        const state_type &state,
        int player,
        std::vector<condition_type> conditions) {

    auto board = state.get_board();
    auto shape = board->get_shape();
    auto starts = board->get_starts();
    int state_dim = conditions.size();
    bool flip_teams = static_cast<bool> (player);

    std::function<position_type(position_type &)> canonize_pos = [&](position_type &pos) { return pos; };
    std::function<int(int)> canonize_team = [](int team) { return team; };

    if (flip_teams) {
        canonize_pos = [&](position_type &pos) { return pos.invert(starts, shape); };
        canonize_team = [](int team) { return 1 - team; };
    }

    auto options =
            torch::TensorOptions()
                    .dtype(torch::kFloat32)
                    .layout(torch::kStrided)
                    .device(GLOBAL_DEVICE::get_device())
                    .requires_grad(true);
    // the dimensions here are as follows:
    // state_dim = dimension of the state rep, i.e. how many layers of the conditions
    // shape[0] = first board dimension
    // shape[1] = second board dimension
    torch::Tensor board_state_rep = torch::zeros({state_dim,
                                                  static_cast<long>(shape[0]),
                                                  static_cast<long>(shape[1])},
                                                 options);

    for (const auto &pos_piece : *board) {
        Position pos = pos_piece.first;
        pos = canonize_pos(pos);
        auto piece = pos_piece.second;
        if (!piece->is_null()) {
            for (auto&&[i, cond_it] = std::make_tuple(0, conditions.begin());
                 cond_it != conditions.end();
                 ++i, ++cond_it) {
                // unpack the condition
                auto[kin, team, hidden] = *cond_it;
                // write the result of the condition check to the tensor
                board_state_rep[i][pos[0]][pos[1]] = _check_condition(piece, kin, team, hidden, flip_teams);
            }
        }
    }
    return board_state_rep;
}

template<typename Board>
std::vector<int> ActionRepStratego::get_action_mask(const Board &board, int player) {
    return get_action_mask(m_actions, board, player);
}

template<typename Board>
std::vector<int> ActionRepStratego::get_action_mask(
        const std::vector<action_type> &actions,
        const Board &board,
        int player) {
    std::vector<int> action_mask(actions.size(), 0);
    for (size_t i = 0; i < actions.size(); ++i) {
        const action_type &action = actions[i];
        position_type old_pos = board.get_position_of_kin(player, action.get_piece_id());
        position_type new_pos = old_pos + action.get_effect();
        if (LogicStratego<board_type>::is_legal_move(board, {old_pos, new_pos})) {
            action_mask[i] = 1;
        }
    }
    return action_mask;
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


#pragma once

#include "game/GameStateStratego.h"
#include "utils/torch_utils.h"
#include "ActionRepresenter.h"
#include "Action.h"
#include "torch/torch.h"

class ActionRepStratego : public ActionRepBase<Action<typename BoardStratego::position_type,
        typename BoardStratego::kin_type>,
        GameStateStratego,
        ActionRepStratego> {

public:
    using state_type = GameStateStratego;
    using position_type = state_type::position_type;
    using kin_type = state_type::piece_type::kin_type;
    using move_type = state_type::move_type;
    using piece_type = state_type::piece_type;
    using board_type = state_type::board_type;
    using action_type = Action<position_type, kin_type>;
    using condition_container = std::vector<std::tuple<typename BoardStratego::piece_type::kin_type, int, bool>>;
    static_assert(std::is_same_v<typename move_type::position_type, typename board_type::position_type>);

    explicit ActionRepStratego(size_t shape)
            : actions(_build_actions_vector(shape)),
              default_conditions(_build_conditions_vector(shape)) {}


    torch::Tensor state_representation(const state_type &state,
                                       int player) {
        return state_representation(state, player, default_conditions);
    }

    template<typename condition_type=std::tuple<kin_type, int, bool>>
    torch::Tensor state_representation(const state_type &state,
                                       int player,
                                       std::vector<condition_type> conditions);

    std::vector<action_type> get_action_vector() { return actions; }

    template<typename BoardType>
    std::vector<int> get_action_mask(
            const BoardType &board,
            int player
    );

    template<typename BoardType>
    static std::vector<int> get_action_mask(
            const std::vector<action_type> &actions,
            const BoardType &board,
            int player
    );


private:
    static std::vector<action_type> _build_actions_vector(size_t shape);

    static condition_container _build_conditions_vector(size_t shape);

    template<typename PieceType>
    inline bool _check_condition(const std::shared_ptr<PieceType> &piece,
                                 const kin_type &kin,
                                 int team,
                                 bool hidden,
                                 bool flip_teams = false
    );

    const std::vector<action_type> actions;
    const condition_container default_conditions;
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

template<typename BoardType>
std::vector<int> ActionRepStratego::get_action_mask(const BoardType &board, int player) {
    return get_action_mask(actions, board, player);
}

template<typename BoardType>
std::vector<int> ActionRepStratego::get_action_mask(
        const std::vector<action_type> &actions,
        const BoardType &board,
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

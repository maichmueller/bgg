//
// Created by michael on 13.08.19.
//

#pragma once

#include "agent/AgentReinforce.h"

template <class Board, class ActionRep>
class AlphaZeroAgent : public AgentReinforceBase<Board> {
    using base_type = AgentReinforceBase<Board>;
    using board_type = Board;
    using action_rep_type = ActionRep;
    using move_type = typename Board::move_type;
    static inline bool check_condition(const std::shared_ptr<Piece>& piece, int team, int type, int version, bool hidden);
    inline std::vector<std::tuple<int, int, int, bool>> create_conditions();

protected:
    action_rep_type m_action_repper;

public:
    template <typename...Params>
    AlphaZeroAgent(int team, bool learner, const std::shared_ptr<NetworkWrapper> & model_sptr)
            : AgentReinforceBase(team, true, model_sptr),
              m_action_repper(Params... & params)
    {}

    move_type decide_move(const Board & board) override {
        torch::Tensor board_state = board_to_state_rep(board);
        auto [pi, v] = Base::m_model->predict(board_state);

        int action = pi.argmax().template item<int64_t>();

        move_type move = ActionRep::action_to_move(action, m_team);

        return move;
    };

};

//
// Created by michael on 13.08.19.
//

#pragma once

#include "agent/AgentReinforce.h"

template <class StateType, class ActionRep>
class AlphaZeroAgent : public AgentReinforceBase<StateType> {
    using base_type = AgentReinforceBase<StateType>;
    using base_type::base_type;
    using state_type = typename base_type::state_type;
    using board_type = typename base_type::board_type;
    using action_rep_type = ActionRep;
    using move_type = typename base_type::move_type;

protected:
    action_rep_type m_action_repper;

public:
    template <typename... Params>
    AlphaZeroAgent(int team, bool learner, const std::shared_ptr<NetworkWrapper> & model_sptr, Params & ...params)
            : base_type(team, true, model_sptr),
              m_action_repper(params...)
    {}
    template <typename... Params>
    AlphaZeroAgent(int team, bool learner, const std::shared_ptr<NetworkWrapper> & model_sptr, const Params & ...params)
            : base_type(team, true, model_sptr),
              m_action_repper(params...)
    {}

    move_type decide_move(const state_type & state) override {
        torch::Tensor state_rep = m_action_repper.state_representation(state);
        auto [pi, v] = base_type::m_model->predict(state_rep);

        int action = pi.argmax().template item<int64_t>();

        move_type move = action_rep_type::action_to_move(action, base_type::m_team);

        return move;
    };

};

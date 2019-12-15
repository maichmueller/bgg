#pragma once

#include "azpp/agent/AgentReinforce.h"

template <class StateType, class ActionRep>
class AlphaZeroAgent : public AgentReinforceBase<StateType> {
    using base_type = AgentReinforceBase<StateType>;
    using base_type::base_type;
    using state_type = typename base_type::state_type;
    using board_type = typename state_type::board_type;
    using action_rep_type = ActionRep;
    using move_type = typename state_type::move_type;

protected:
    std::shared_ptr<action_rep_type> m_action_repper_sptr;

public:
    AlphaZeroAgent(
            int team, 
            const std::shared_ptr<NetworkWrapper> & model_sptr,
            const std::shared_ptr<action_rep_type>  & action_repper
            )
            : base_type(team, model_sptr),
              m_action_repper_sptr(action_repper)
    {}
    template <typename... Params>
    AlphaZeroAgent(int team, const std::shared_ptr<NetworkWrapper> & model_sptr, const Params & ...params)
            : base_type(team, model_sptr),
              m_action_repper_sptr(std::make_shared<action_rep_type >(params...))
    {}

    move_type decide_move(const state_type & state, const std::vector<move_type> & poss_moves) override {
        torch::Tensor state_rep = m_action_repper_sptr->state_representation(state, base_type::m_team);
        auto [pi, v] = base_type::m_model->predict(state_rep);

        int action_idx = pi.argmax().template item<int64_t>();
        move_type move = m_action_repper_sptr->action_to_move(state, action_idx, base_type::m_team);

        return move;
    };

};

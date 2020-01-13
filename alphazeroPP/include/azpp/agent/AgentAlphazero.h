#pragma once

#include "azpp/agent/AgentReinforce.h"
#include "azpp/nn/representation/Representer.h"
#include "azpp/utils.h"
#include "torch/torch.h"

template < class StateType, class ActionRepType >
class AlphaZeroAgent: public AgentReinforceBase< StateType > {
   using base_type = AgentReinforceBase< StateType >;
   using base_type::base_type;
   using state_type = typename base_type::state_type;
   using board_type = typename state_type::board_type;
   using action_rep_type = RepresenterBase< state_type, ActionRepType >;
   using move_type = typename state_type::move_type;

  protected:
   std::shared_ptr< action_rep_type > m_action_rep_ptr;

  public:
   AlphaZeroAgent(
      int team,
      const std::shared_ptr< NetworkWrapper > &model_sptr,
      const std::shared_ptr< action_rep_type > &action_rep_ptr)
       : base_type(team, model_sptr), m_action_rep_ptr(action_rep_ptr)
   {
   }

   template < typename... Params >
   AlphaZeroAgent(
      int team,
      const std::shared_ptr< NetworkWrapper > &model_sptr,
      const Params &... params)
       : base_type(team, model_sptr), m_action_rep_ptr(params...)
   {
   }

   move_type decide_move(
      const state_type &state,
      const std::vector< move_type > &poss_moves) override
   {
      torch::Tensor state_rep = m_action_rep_ptr->state_representation(
         state, base_type::m_team);

      auto [pi, v] = base_type::m_model->predict(state_rep);
      auto validity_mask = m_action_rep_ptr->get_action_mask(
         *state.get_board(), base_type::m_team);
      pi.squeeze_(0);
      auto pi_acc = pi.template accessor< float, 1 >();
      double max = torch::log(
                      torch::from_blob(std::vector< float >{0}.data(), {1}))
                      .template item< double >();
      size_t argmax = 0;
      for(size_t i = 0; i < validity_mask.size(); ++i) {
         if(validity_mask[i]) {
            if(pi_acc[i] > max) {
               max = pi_acc[i];
               argmax = i;
            }
         }
      }
      move_type move = m_action_rep_ptr->action_to_move(
         state, argmax, base_type::m_team);

      return move;
   };
};

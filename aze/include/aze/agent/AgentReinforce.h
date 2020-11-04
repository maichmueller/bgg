#pragma once

#include <algorithm>  // std::find
#include <utility>

#include "aze/agent/Agent.h"
#include "aze/board/Piece.h"
#include "aze/nn/model/NeuralNet.h"
#include "aze/nn/representation/Representer.h"
#include "aze/utils/torch_utils.h"
#include "torch/torch.h"

template < class StateType >
class AgentReinforceBase: public Agent< StateType > {
  public:
   using base_type = Agent< StateType >;
   using base_type::base_type;
   using piece_type = typename StateType::piece_type;

  protected:
   sptr< NetworkWrapper > m_model;

  public:
   AgentReinforceBase(int team, sptr< NetworkWrapper > model_sptr)
       : base_type(team, true), m_model(std::move(model_sptr))
   {
   }

   sptr< NetworkWrapper > get_model_sptr() { return m_model; }
};

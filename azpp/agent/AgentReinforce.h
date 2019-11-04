//
// Created by michael on 14.04.19.
//

#pragma once

#include <algorithm>    // std::find

#include "Agent.h"
#include "board/Piece.h"
#include "nn/model/NeuralNet.h"
#include "utils/torch_utils.h"
#include "torch/torch.h"
#include "nn/model/representation/ActionRepresenter.h"


template <class StateType>
class AgentReinforceBase: public Agent<StateType> {

public:
    using base_type = Agent<StateType>;
    using base_type::base_type;
    using piece_type = typename base_type::piece_type;

protected:
    std::shared_ptr<NetworkWrapper> m_model;

public:
    AgentReinforceBase(int team, const std::shared_ptr<NetworkWrapper> & model_sptr)
    : base_type(team, true),
      m_model(model_sptr)
    {}

    std::shared_ptr<NetworkWrapper> get_model_sptr() {return m_model;}
};

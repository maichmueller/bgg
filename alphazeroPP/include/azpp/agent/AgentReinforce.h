#pragma once

#include <algorithm>    // std::find
#include <utility>

#include "torch/torch.h"

#include "azpp/agent/Agent.h"
#include "azpp/board/Piece.h"
#include "azpp/nn/model/NeuralNet.h"
#include "azpp/utils/torch_utils.h"
#include "azpp/nn/model/representation/Representer.h"


template <class StateType>
class AgentReinforceBase: public Agent<StateType> {

public:
    using base_type = Agent<StateType>;
    using base_type::base_type;
    using piece_type = typename StateType::piece_type;

protected:
    std::shared_ptr<NetworkWrapper> m_model;

public:
    AgentReinforceBase(int team, std::shared_ptr<NetworkWrapper>  model_sptr)
    : base_type(team, true),
      m_model(std::move(model_sptr))
    {}

    std::shared_ptr<NetworkWrapper> get_model_sptr() {return m_model;}
};

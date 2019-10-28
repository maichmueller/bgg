//
// Created by michael on 14.04.19.
//

#pragma once

#include <algorithm>    // std::find

#include "Agent.h"
#include "../board/Piece.h"
#include "../nn/model/NeuralNet.h"
#include "../utils/torch_utils.h"
#include "torch/torch.h"
#include "../nn/training/StateRepresentation.h"

//struct VecIntHasher {
//    std::size_t operator()(std::vector<int> const& vec) const {
//        std::size_t seed = vec.size();
//        for(auto& i : vec) {
//            seed ^= i + 0x9e3779b9 + (seed << 6) + (seed >> 2);
//        }
//        return seed;
//    }
//};

template <class Board>
class AgentReinforceBase: public Agent<Board> {

public:
    using base_type = Agent<Board>;
    using base_type::base_type;
    using piece_type = typename Board::piece_type;

protected:
    int m_state_dim;
    int m_action_dim;
    int m_board_len;

    std::vector<std::tuple<int, int, int, bool>> m_conditions;
    std::map<int, unsigned int> m_type_counter;

    std::shared_ptr<NetworkWrapper> m_model;

public:
    AgentReinforceBase(int team, const std::shared_ptr<NetworkWrapper> & model_sptr)
    : base_type(team, true), m_state_dim(), m_action_dim(), m_board_len(), m_conditions(), m_type_counter(),
      m_model(model_sptr)
    {}

    std::shared_ptr<NetworkWrapper> get_model_sptr() {return m_model;}
};

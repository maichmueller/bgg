//
// Created by michael on 14.04.19.
//

#ifndef STRATEGO_CPP_AGENTREINFORCE_H
#define STRATEGO_CPP_AGENTREINFORCE_H

#include <algorithm>    // std::find

#include "Agent.h"
#include "../board/Piece.h"
#include "../nn/model/NeuralNetwork.h"
#include "../utils/torch_utils.h"
#include "torch/torch.h"
#include "../nn/training/StateRepresentation.h"

struct VecIntHasher {
    std::size_t operator()(std::vector<int> const& vec) const {
        std::size_t seed = vec.size();
        for(auto& i : vec) {
            seed ^= i + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        }
        return seed;
    }
};

class AgentReinforceBase: public Agent {
protected:
    int m_state_dim;
    int m_action_dim;
    int m_board_len;

    std::vector<std::tuple<int, int, int, bool>> m_conditions;
    std::map<int, unsigned int> m_type_counter;

    // type that stores {type, version} -> Piece-Obj information
    using si_key = std::tuple<int, int>;
    using si_hasher = hash_tuple::hash<si_key>;
    using si_eq_comp = eqcomp_tuple::eqcomp<si_key >;
    std::unordered_map<si_key, std::shared_ptr<Piece>, si_hasher, si_eq_comp> m_actors;
    // the neural network
    std::shared_ptr<NetworkWrapper> m_model;
public:
    AgentReinforceBase(int team, bool learner, const std::shared_ptr<NetworkWrapper> & model_sptr)
    : Agent(team, learner), m_state_dim(), m_action_dim(), m_board_len(), m_conditions(), m_type_counter(),
    m_model(model_sptr)
    {
    }
    virtual torch::Tensor board_to_state_rep(const Board& board) = 0;
    void install_board(const Board& board) override;
    std::shared_ptr<NetworkWrapper> get_model_sptr() {return m_model;}
};



#endif //STRATEGO_CPP_AGENTREINFORCE_H

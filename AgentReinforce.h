//
// Created by michael on 14.04.19.
//

#ifndef STRATEGO_CPP_AGENTREINFORCE_H
#define STRATEGO_CPP_AGENTREINFORCE_H

#include <algorithm>    // std::find

#include "Agent.h"
#include "Piece.h"
#include "torch_utils.h"
#include "torch/torch.h"

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
    int state_dim;
    int action_dim;
    int board_len;

    std::vector<std::tuple<int, int, int, bool>> conditions;
    std::map<int, unsigned int> type_counter;

    // member that stores {type, version} -> Piece-Obj information
    std::unordered_map<std::vector<int>, std::shared_ptr<Piece>, VecIntHasher> actors;
    // the neural network
    std::shared_ptr<torch::nn::Module> model;

    std::vector<pos_type > _action_to_move(int action);

    std::map<int, unsigned int> counter(const std::vector<int>& vals);

public:

    virtual torch::Tensor board_to_state_rep(const Board& board) = 0;

    void install_board(const Board& board) override;
};

class AlphaZeroAgent : public AgentReinforceBase {


    bool check_condition(const std::shared_ptr<Piece>& piece, int team, int type, int version, bool hidden) const;
    std::vector<std::tuple<int, int, int, bool>> create_conditions();

public:

    void install_board(const Board& board) override;
    torch::Tensor board_to_state_rep(const Board& board) override;
    std::vector<pos_type > decide_move(const Board& board) override;

};

#endif //STRATEGO_CPP_AGENTREINFORCE_H

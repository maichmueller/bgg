//
// Created by Michael on 08-Apr-19.
//

#ifndef STRATEGO_CPP_MCTS_H
#define STRATEGO_CPP_MCTS_H

#include "unordered_map"
#include "string"

#include "model/NeuralNet.h"
#include "utils/utils.h"
#include "game/GameState.h"

#include "training/StateRepresentation.h"
#include "logic/LogicStratego.h"


class MCTS {

    std::shared_ptr<NetworkWrapper> m_nnet_sptr;
    double m_cpuct;
    int m_num_mcts_sims;

    // maps for Q-values and visit count (N) per state s and action a
    std::unordered_map<std::tuple<std::string, int>, double, utils::StringIntHasher, utils::StringIntEqCompare> m_Qsa;
    std::unordered_map<std::tuple<std::string, int>, int, utils::StringIntHasher, utils::StringIntEqCompare> m_Nsa;

    // maps for visit counts (N), initial policy (P), terminality (E), valid actions (V) per state s
    std::unordered_map<std::string, int> m_Ns;
    std::unordered_map<std::string, std::vector<float>> m_Ps;
    std::unordered_map<std::string, int> m_Es;
    std::unordered_map<std::string, std::vector<int>> m_Vs;

    double _search(GameState& state, int player, bool root= false);

    std::tuple<std::vector<float>, std::vector<int>, double>  _evaluate_new_state(GameState & state, int player);

public:

    MCTS(std::shared_ptr<NetworkWrapper> nnet_sptr, int num_mcts_sims, double cpuct=4);

    std::vector<double> get_action_probs(GameState& state, int player, double expl_rate=1.);

    static strat_move_t flip_move(strat_move_t move, int board_len);

};


#endif //STRATEGO_CPP_MCTS_H

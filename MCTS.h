//
// Created by Michael on 08-Apr-19.
//

#ifndef STRATEGO_CPP_MCTS_H
#define STRATEGO_CPP_MCTS_H

#include "unordered_map"
#include "string"

#include "NeuralNetwork.h"
#include "utils.h"




struct StringIntHasher {
    std::hash<std::string> hasher;
    size_t operator()(std::tuple<std::string, int>& s) {
        return hasher(std::get<0>(s) + std::to_string(std::get<1>(s)));
    }
};

struct StringIntEqCompare {
    bool operator()(const std::tuple<std::string, int>& s1, const std::tuple<std::string, int>& s2) {
        return (std::get<0>(s1) == std::get<0>(s2)) && (std::get<1>(s1) == std::get<1>(s2));
    }
};


class MCTS {

    std::shared_ptr<NeuralNetwork> nnet_wrapper_sptr;
    double cpuct;
    int num_mcts_sims;

    // maps for Q-values and visit count (N) per state s and action a
    std::unordered_map<std::tuple<std::string, int>, int, StringIntHasher, StringIntEqCompare> Qsa;
    std::unordered_map<std::tuple<std::string, int>, int, StringIntHasher, StringIntEqCompare> Nsa;

    // for consistency reasons the tuple is being kept here
    // maps for visit counts (N), initial policy (P), terminality (E), valid actions (V) per state s
    std::unordered_map<std::tuple<std::string>, int> Ns;
    std::unordered_map<std::tuple<std::string>, int> Ps;
    std::unordered_map<std::tuple<std::string>, int> Es;
    std::unordered_map<std::tuple<std::string>, int> Vs;

public:

    template <typename StateType>
    std::vector<double> get_action_probs(StateType& state, int player, double expl_rate=1.);

    template <typename StateType>
    int search(StateType& state, int player, bool root=false);


};


#endif //STRATEGO_CPP_MCTS_H

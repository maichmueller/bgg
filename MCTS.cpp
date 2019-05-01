//
// Created by Michael on 08-Apr-19.
//

#include "MCTS.h"


template <typename StateType>
std::vector<double> MCTS::get_action_probs(StateType& state, int player, double expl_rate) {
    for(int i = 0; i < num_mcts_sims; ++i) {
        search(state, player, true);
    }

    std::string state_rep = utils::board_str_rep(state->get_board(), static_cast<bool>(player), true);
    std::vector counts(ActionRep::get_act_rep(state->get_board()->get_board_len()).size());
    //
    double sum_counts = 0;
    double highest_count = 0;
    int best_act = 0;
    for(int a = 0; a < counts.size(); ++a) {
        auto count_entry = Nsa.find(std::make_tuple(state_rep, a));
        if (count_entry == Nsa.end()) {
            // not found
            counts[a] = 0;
        } else {
            // found
            int& count = count_entry->second;
            counts[a] = count;
            sum_counts += count;
            if (count > highest_count) {
                best_act = a;
                highest_count = count;
            }
        }
    }
    if(sum_counts == 0) {
        return std::vector<double>(0);
    }

    std::vector<double> probs(counts.size(), 0.0);
    if(expl_rate == 0) {
        probs[best_act] = 1;
        return probs;
    }
    sum_counts = 0;
    for(int i = 0; i < counts.size(); ++i) {
        auto val = std::pow(counts[i], 1/expl_rate);
        counts[i] = val;
        sum_counts += val;
    }

    for(int i = 0; i < counts.size(); ++i) {
        probs[i] /= sum_counts;
    }
    return probs;
}

template <typename StateType>
int MCTS::search(StateType& state, int player, bool root) {
    // for the state rep we flip the board if player == 1 and we dont if player == 0!
    // all the enemy hidden pieces wont be printed out -> unknown pieces are also hidden
    // for the neural net
    std::string state_rep = utils::board_str_rep(state->get_board(), static_cast<bool>(player), true);

    auto state_end_found = Es.find(state_rep);
    if(state_end_found == Es.end())
        Es[state_rep] = state.is_terminal();
    else if(state_end_found->second != 404)
        return - state_end_found->second;

    auto state_pi_found = Ps.find(state_rep);
    if(state_pi_found == Ps.end()) {
        auto prediction = nnet_sptr->predict();
    }

    //
    //
    //
    //
}
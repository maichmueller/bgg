#pragma once

#include <unordered_map>
#include <string>
#include <memory>

#include <azpp/logic/Logic.h>
#include "azpp/game/State.h"
#include "azpp/nn/model/NeuralNet.h"
#include "azpp/utils/utils.h"
#include "azpp/nn/model/representation/Representer.h"


class MCTS {

    std::shared_ptr<NetworkWrapper> m_nnet_sptr;
    double m_cpuct;
    int m_num_mcts_sims;

    static double m_EPS;

    // maps for Q-values and visit count (N) per state s and action a
    std::unordered_map<std::tuple<std::string, int>, double> m_Qsa;
    std::unordered_map<std::tuple<std::string, int>, int> m_Nsa;

    // maps for visit counts (N), initial policy (P), terminality (E), valid actions (V) per state s
    std::unordered_map<std::string, int> m_Ns;
    std::unordered_map<std::string, std::vector<float>> m_Ps;
    std::unordered_map<std::string, int> m_Es;
    std::unordered_map<std::string, std::vector<unsigned int>> m_Vs;

    static std::vector<double> _sample_dirichlet(size_t size);

    template<typename StateType, typename ActionRepresenterType>
    double _search(
            StateType &state,
            int player,
            RepresenterBase<
                    StateType,
                    ActionRepresenterType
            > &action_repper,
            bool root = false
    );

    template<typename StateType, typename ActionRepresenterType>
    std::tuple<std::vector<float>, std::vector<unsigned int>, double> _evaluate_new_state(
            StateType &state,
            int player,
            RepresenterBase<
                    StateType,
                    ActionRepresenterType
            > & action_repper
    );


public:

    MCTS(
            std::shared_ptr<NetworkWrapper> nnet_sptr,
            int num_mcts_sims,
            double cpuct = 4
    );

    template<typename StateType, typename ActionRepresenterType>
    std::vector<double> get_action_probabilities(
            StateType &state,
            int player,
            RepresenterBase<StateType, ActionRepresenterType> &action_repper,
            double expl_rate = 1.
    );
};


template<typename StateType, typename ActionRepresenterType>
std::vector<double> MCTS::get_action_probabilities(
        StateType &state,
        int player,
        RepresenterBase<StateType, ActionRepresenterType> &action_repper,
        double expl_rate) {

    for (int i = 0; i < m_num_mcts_sims; ++i) {
        _search(state, player, action_repper, /*root=*/true);
    }

    std::string state_rep = state.get_board()->print_board(static_cast<bool>(player), true);

    std::vector<int> counts(action_repper.get_actions().size());

    double sum_counts = 0;
    double highest_count = 0;
    int best_act = 0;
    for (size_t a = 0; a < counts.size(); ++a) {
        auto count_entry = m_Nsa.find(std::make_tuple(state_rep, a));
        if (count_entry == m_Nsa.end()) {
            // not found
            counts[a] = 0;
        } else {
            // found
            int const &count = count_entry->second;
            counts[a] = count;
            sum_counts += count;
            if (count > highest_count) {
                best_act = a;
                highest_count = count;
            }
        }
    }
    if (sum_counts == 0) {
        return std::vector<double>(0);
    }

    std::vector<double> probabilities(counts.size(), 0.0);
    if (expl_rate == 0) {
        probabilities[best_act] = 1;
        return probabilities;
    }
    sum_counts = 0;
    for (auto &count : counts) {
        auto val = std::pow(count, 1 / expl_rate);
        count = val;
        sum_counts += val;
    }

    for (size_t i = 0; i < counts.size(); ++i) {
        probabilities[i] = counts[i] / sum_counts;
    }
    return probabilities;
}

template<typename StateType, typename ActionRepresenterType>
std::tuple<std::vector<float>, std::vector<unsigned int>, double> MCTS::_evaluate_new_state(
        StateType &state,
        int player,
        RepresenterBase<
                StateType,
                ActionRepresenterType
        > &action_repper) {

    auto board = state.get_board();
    // convert State to torch::tensor representation
    const torch::Tensor state_tensor = action_repper.state_representation(state, player);

    auto[Ps, v] = m_nnet_sptr->predict(state_tensor);

    // DEBUG
    // std::cout << torch::exp(Ps) << "\n";

    Ps = Ps.view(-1); // flatten the tensor as the first dim is the batch size dim

    // mask for invalid actions
    const auto action_mask = action_repper.get_action_mask(*board, player);

    torch::TensorAccessor Ps_acc = Ps.template accessor<float, 1>();
    std::vector<float> Ps_filtered(action_mask.size());
    float Ps_sum = 0;

    // mask invalid actions
    for (size_t i = 0; i < action_mask.size(); ++i) {
        auto temp = Ps_acc[i] * action_mask[i];
        Ps_sum += temp;
        Ps_filtered[i] = temp;
    }
    // normalize the likelihoods
    for (auto &p_val : Ps_filtered) { p_val /= Ps_sum; }
    return std::make_tuple(Ps_filtered, action_mask, v);
}


template<typename StateType, typename ActionRepresenterType>
double MCTS::_search(
        StateType &state,
        int player,
        RepresenterBase<
                StateType,
                ActionRepresenterType
        > &action_repper,
        bool root) {
    // for the state rep we flip the board if player == 1 and we dont if player == 0!
    // all the enemy hidden pieces wont be printed out -> unknown pieces are also hidden
    // for the neural net
    std::string s = state.get_board()->print_board(
            static_cast<bool>(player),
            true
    );

    if (auto state_end_found = m_Es.find(s); state_end_found == m_Es.end())
        m_Es[s] = state.is_terminal();
    else if (state_end_found->second != 404)
        return -1 * state_end_found->second;

    if (auto state_pi_exists = m_Ps.find(s); state_pi_exists == m_Ps.end()) {
        // if the state wasn't found (== end)

        auto[Ps_filtered, action_mask, v] = std::move(_evaluate_new_state(state, player, action_repper));

        // storing these found values for this state for later lookup
        m_Ps[s] = Ps_filtered;
        m_Vs[s] = action_mask;
        m_Ns[s] = 0;

        return -v;
    }

    std::vector<unsigned int> valids = m_Vs[s];
    // DEBUG
//    std::vector<strat_move_t > all_moves(valids.size());
//    std::cout  << utils::board_str_rep<Board, Piece>(*state.get_board(), static_cast<bool>(player), false) << "\n";
//    for(int i = 0; i < valids.size(); ++i) {
//        strat_move_t move = state.action_to_move(i, player);
//        std::cout << "Action: " << std::to_string(i) << "\t" << "(" << move[0][0] << ", " << move[0][1] << ") -> (" << move[1][0] << ", " << move[1][1] << ") \t valid: " << valids[i] << "\n";
//    }
//    const Board * board = state.get_board();
//    int m_shape = board->get_shape();
//    auto action_mask = LogicStratego::get_action_mask(
//            *board,
//            ActionRep::get_act_rep(m_shape),
//            ActionRep::get_act_map(m_shape),
//            player);

    double curr_best = -std::numeric_limits<double>::infinity();
    int best_action = -1;

    std::vector<float> Ps = this->m_Ps[s];

    if (root) {
        std::vector<double> dirichlet = _sample_dirichlet(Ps.size());
        double new_sum_val = 0;
        for (size_t i = 0; i < Ps.size(); ++i) {
            // Check the alphazero paper for adding dirichlet noise to the priors.
            double pi = (0.75 * Ps[i] + 0.25 * dirichlet[i]) * valids[i];
            Ps[i] = pi;
            new_sum_val += pi;
        }
        // Normalize
        for (auto &p : Ps) { p /= new_sum_val; }
    }

    for (size_t a = 0; a < Ps.size(); ++a) {
        if (valids[a]) {
            double u;
            auto s_a = std::make_tuple(s, a);
            if (auto Qs = m_Qsa.find(s_a); Qs != m_Qsa.end()) {
                u = Qs->second + m_cpuct * Ps[a] * sqrt(m_Ns[s]) / (1 + m_Nsa.find(s_a)->second);
            } else {
                u = m_cpuct * Ps[a] * sqrt(m_Ns[s] + m_EPS);
            }
            if (u > curr_best) {
                curr_best = u;
                best_action = a;
            }
        }
    }

    int a = best_action;
    // DEBUG
//    std::cout << "Player: "<< player << "\t"<< "Best action: " << a << "\t";

    typename StateType::move_type move = action_repper.action_to_move(state, a, player);

    // flip the move for player 1
//    if (player) {
//        move = flip_move(move, state.get_board()->get_shape());
//
//    }
    // DEBUG
//    std::cout << "Move: (" << move[0][0] << ", " << move[0][1] << ") -> ("
//              << move[1][0] << ", " << move[1][1] << ")" << "\n";
    state.do_move(move);
    // DEBUG
//    std::cout  << "Board after move done: \n" << utils::board_str_rep<Board, Piece>(*state.get_board(), static_cast<bool>(0), false) << "\n";
//
    double v = _search(state, (player + 1) % 2, action_repper, /*root=*/false);
    state.undo_last_rounds();
    // DEBUG
//    std::cout  << "Board after move undone: \n" << utils::board_str_rep<Board, Piece>(*state.get_board(), static_cast<bool>(player), false) << "\n";

    auto s_a = std::make_tuple(s, a);
    if (auto qs_found = m_Qsa.find(s_a); qs_found != m_Qsa.end()) {
        int n_sa = m_Nsa.at(s_a);
        m_Qsa[s_a] = (n_sa * m_Qsa[s_a] + v) / (n_sa + 1);
        m_Nsa[s_a] += 1;
    } else {
        m_Qsa[s_a] = v;
        m_Nsa[s_a] += 1;
    }

    m_Ns[s] += 1;
    return -v;
}

//
// Created by Michael on 08-Apr-19.
//

#include "MCTS.h"
#include "random"
#include "AgentReinforce.h"
#include "ActionRepresentation.h"

double EPS = 10e-8;



MCTS::MCTS(std::shared_ptr<NetworkWrapper> nnet_sptr, int num_mcts_sims, double cpuct)
        : m_nnet_sptr(std::move(nnet_sptr)),
          m_num_mcts_sims(num_mcts_sims),
          m_cpuct(cpuct),
          m_Qsa(),
          m_Nsa(),
          m_Ns(),
          m_Ps(),
          m_Es(),
          m_Vs()
{}



std::vector<double> MCTS::get_action_probs(GameState &state, int player, double expl_rate) {
    for (int i = 0; i < m_num_mcts_sims; ++i) {
        _search(state, player, true);
    }

    std::string state_rep = utils::board_str_rep<Board, Piece>(
            *state.get_board(),
            static_cast<bool>(player),
            true);
    std::vector<int> counts(ActionRep::get_act_rep(state.get_board()->get_board_len()).size());

    double sum_counts = 0;
    double highest_count = 0;
    int best_act = 0;
    for (int a = 0; a < counts.size(); ++a) {
        auto count_entry = m_Nsa.find(std::make_tuple(state_rep, a));
        if (count_entry == m_Nsa.end()) {
            // not found
            counts[a] = 0;
        } else {
            // found
            int const & count = count_entry->second;
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

    std::vector<double> probs(counts.size(), 0.0);
    if (expl_rate == 0) {
        probs[best_act] = 1;
        return probs;
    }
    sum_counts = 0;
    for (int i = 0; i < counts.size(); ++i) {
        auto val = std::pow(counts[i], 1 / expl_rate);
        counts[i] = val;
        sum_counts += val;
    }

    for (int i = 0; i < counts.size(); ++i) {
        probs[i] = counts[i] / sum_counts;
    }
    return probs;
}

std::tuple<std::vector<float>, std::vector<int>, double> MCTS::_evaluate_new_state(GameState & state, int player) {
    const Board * board = state.get_board();
    int board_len = board->get_board_len();
    // convert GameState to torch::tensor representation
    const torch::Tensor state_tensor = state.torch_represent(player);

    auto [Ps, v] = m_nnet_sptr->predict(state_tensor);

    // DEBUG
    // std::cout << torch::exp(Ps) << "\n";

    Ps = Ps.view(-1); // flatten the tensor as the first dim is the batch size dim

    // mask for invalid actions
    auto action_mask = StrategoLogic::get_action_mask(
            *board,
            ActionRep::get_act_rep(board_len),
            ActionRep::get_act_map(board_len),
            player);

    torch::TensorAccessor Ps_acc = Ps.template accessor<float, 1>();
    std::vector<float> Ps_filtered(action_mask.size());
    float Ps_sum = 0;

    // mask invalid actions
    for(int i = 0; i < action_mask.size(); ++i) {
        auto temp = Ps_acc[i] * action_mask[i];
        Ps_sum += temp;
        Ps_filtered[i] = temp;
    }
    // normalize the likelihoods
    for(int i = 0; i < Ps_filtered.size(); ++i) {
        Ps_filtered[i] /= Ps_sum;
    }
    return std::make_tuple(Ps_filtered, action_mask, v);
}

std::vector<double> sample_dirichlet(size_t size) {
    /** To receive 1 sample vector (x_1, ..., x_K) from a
     * K-dimensional dirichlet(alpha_1, ..., alpha_K)
     * distribution one can draw K samples (y_1, ..., y_K)
     * from a gamma distribution with a density of
     * Gamma(alpha_i, 1) and then set x_i = y_i / sum_j(y_j)
     * Therefore, the following adds DIRICHLET noise to
     * the root priors (not gammma noise!)
     * Check: https://en.wikipedia.org/wiki/Dirichlet_distribution#Random_number_generation
    */

    std::default_random_engine generator{std::random_device()()};
    std::gamma_distribution<double> distribution(0.5, 1.0);

    std::vector<double> gamma_draws(size);

    double gamma_sum = 0;
    for(int i = 0; i < gamma_draws.size(); i++) {
        auto draw = distribution(generator);
        gamma_draws[i] = draw;
        gamma_sum += draw;
    }
    // Normalize to gain Dirichlet noise
    for (int i = 0; i < size; ++i) {gamma_draws[i] = gamma_draws[i] / gamma_sum;}
    return gamma_draws;
}

move_t MCTS::flip_move(move_t move, int board_len) {
    for(auto & move_part: move) {
        for(auto & move_seg : move_part) {
            move_seg = board_len - 1 - move_seg;
        }
    }
    return move;
}

double MCTS::_search(GameState& state, int player, bool root) {
    // for the state rep we flip the board if player == 1 and we dont if player == 0!
    // all the enemy hidden pieces wont be printed out -> unknown pieces are also hidden
    // for the neural net
    std::string s = utils::board_str_rep<Board, Piece>(
            *state.get_board(),
            static_cast<bool>(player),
            true
    );


    if (auto state_end_found = m_Es.find(s); state_end_found == m_Es.end())
        m_Es[s] = state.is_terminal();
    else if (state_end_found->second != 404)
        return -1 * state_end_found->second;

    if (auto state_pi_exists = m_Ps.find(s); state_pi_exists == m_Ps.end()) {
        // if the state wasn't found (== end)

        auto [Ps_filtered, action_mask, v] = std::move(_evaluate_new_state(state, player));

        // storing these found values for this state for later lookup
        // && is fine to be passed here, since standard containers always copy their input
        this->m_Ps[s] = Ps_filtered;
        this->m_Vs[s] = action_mask;
        this->m_Ns[s] = 0;

        return -v;
    }

    std::vector<int> valids = m_Vs[s];
    // DEBUG
//    std::vector<move_t > all_moves(valids.size());
//    std::cout  << utils::board_str_rep<Board, Piece>(*state.get_board(), static_cast<bool>(player), false) << "\n";
//    for(int i = 0; i < valids.size(); ++i) {
//        move_t move = state.action_to_move(i, player);
//        std::cout << "Action: " << std::to_string(i) << "\t" << "(" << move[0][0] << ", " << move[0][1] << ") -> (" << move[1][0] << ", " << move[1][1] << ") \t valid: " << valids[i] << "\n";
//    }
//    const Board * board = state.get_board();
//    int board_len = board->get_board_len();
//    auto action_mask = StrategoLogic::get_action_mask(
//            *board,
//            ActionRep::get_act_rep(board_len),
//            ActionRep::get_act_map(board_len),
//            player);

    double curr_best = - std::numeric_limits<double>::infinity();
    int best_action = -1;

    std::vector<float> Ps = this->m_Ps[s];

    if(root) {
        std::vector<double> dirichlet = sample_dirichlet(Ps.size());
        double new_sum_val = 0;
        for (int i = 0; i < Ps.size(); ++i) {
            // Check the alphazero paper for adding dirichlet noise to the priors.
            double pi = (0.75 * Ps[i] + 0.25 * dirichlet[i]) * valids[i];
            Ps[i] = pi;
            new_sum_val += pi;
        }
        // Normalize
        for (int i = 0; i < Ps.size(); ++i) {Ps[i] /= new_sum_val;}
    }

    for(int a = 0; a < Ps.size(); ++a) {
        if(valids[a]) {
            double u;
            auto s_a = std::make_tuple(s, a);
            if(auto Qs = m_Qsa.find(s_a); Qs != m_Qsa.end()) {
                u = Qs->second + m_cpuct * Ps[a] * sqrt(m_Ns[s]) / (1 + m_Nsa.find(s_a)->second);
            }
            else {
                u = m_cpuct * Ps[a] * sqrt(m_Ns[s] + EPS);
            }
            if(u > curr_best) {
                curr_best = u;
                best_action = a;
            }
        }
    }
    int& a = best_action;
    // DEBUG
//    std::cout << "Player: "<< player << "\t"<< "Best action: " << a << "\t";

    move_t move = state.action_to_move(a, player);

    // flip the move for player 1
    if(player) {
        move = flip_move(move, state.get_board()->get_board_len());

    }
    // DEBUG
//    std::cout << "Move: (" << move[0][0] << ", " << move[0][1] << ") -> ("
//              << move[1][0] << ", " << move[1][1] << ")" << "\n";
    state.do_move(move);
    // DEBUG
//    std::cout  << "Board after move done: \n" << utils::board_str_rep<Board, Piece>(*state.get_board(), static_cast<bool>(0), false) << "\n";
//
    double v = _search(state, (player + 1) % 2, /*root=*/false);
    state.undo_last_rounds();
    // DEBUG
//    std::cout  << "Board after move undone: \n" << utils::board_str_rep<Board, Piece>(*state.get_board(), static_cast<bool>(player), false) << "\n";

    auto s_a = std::make_tuple(s, a);
    if( auto qs_found = m_Qsa.find(s_a); qs_found != m_Qsa.end()) {
        int n_sa = m_Nsa.at(s_a);
        m_Qsa[s_a] = (n_sa * m_Qsa[s_a] + v) / (n_sa + 1);
        m_Nsa[s_a] += 1;
    }
    else {
        m_Qsa[s_a] = v;
        m_Nsa[s_a] += 1;
    }

    m_Ns[s] += 1;
    return -v;
}


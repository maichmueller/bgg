#include <random>

#include "azpp/mcts/MCTS.h"
#include "azpp/agent/AgentReinforce.h"
#include "azpp/nn/model/representation/ActionRepresenter.h"

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



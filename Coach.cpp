//
// Created by michael on 05.06.19.
//

#include "Coach.h"
#include "MCTS.h"
#include "torch_utils.h"
#include "AgentReinforce.h"
#include "Arena.h"

#include "torch/torch.h"

#include <random>
#include "filesystem"



Coach::Coach(std::shared_ptr<Game> game, std::shared_ptr<NetworkWrapper> nnet,
                      std::shared_ptr<NetworkWrapper> opp_nnet, int num_iters, float win_frac,
                      int num_episodes, int num_mcts_sims, int num_iters_train_examples_hist, std::string model_folder,
                      int exploration_rate)
                      : m_game(std::move(game)), m_nnet(std::move(nnet)), m_opp_nnet(std::move(opp_nnet)),
                      m_num_iters(num_iters), m_win_frac(win_frac), m_num_episodes(num_episodes),
                      m_num_iters_train_examples_hist(num_iters_train_examples_hist),
                      m_num_mcts_simulations(num_mcts_sims), m_model_folder(std::move(model_folder)),
                      m_exploration_rate(exploration_rate)
                      {}


std::vector<TrainingTurn> Coach::exec_ep(GameState state) const {

    int ep_step = 0;

    std::vector<TrainingTurn> ep_exs;

    MCTS mcts = MCTS(m_nnet, m_num_mcts_simulations);

    constexpr int null_v = -100;

    while(true) {
        ep_step += 1;
        int expl_rate = static_cast<int>(ep_step < m_exploration_rate);

        int turn = state.get_move_count() % 2;
        std::vector<double> pi = mcts.get_action_probs(state, /*player=*/turn, expl_rate);

        std::default_random_engine generator{std::random_device()()};
        std::discrete_distribution<int> qs_sampler {pi.begin(), pi.end()};

        int action = qs_sampler(generator);
        move_type move = state.action_to_move(action, turn);

        ep_exs.emplace_back(TrainingTurn(*state.get_board(), pi, null_v, turn));

        state.do_move(move);
        int r = state.is_terminal(true, /*turn=*/0);

        if(r != 404) {
            for(auto & train_turn : ep_exs) {
                // since v is always return as -v from the MCTS search (the endvalue
                // as seen from the opponent's side), we will have to adapt the value
                // in each turn to reflect the changing player.
                train_turn.m_v = turn != train_turn.m_player ? r : -r;
            }
            return ep_exs;
        }
    }
}

void Coach::teach(bool from_prev_examples,
                           bool load_current_best,
                           bool skip_first_self_play,
                           bool multiprocess) {
    bool checkpoint_found = false;

    namespace fs = std::filesystem;
    fs::path checkpoint_path;
    if(from_prev_examples) {
        int i = 0;

        while (true) {
            fs::path dir(m_model_folder);
            fs::path file("checkoint_" + std::to_string(i) + ".pth.tar");
            fs::path full_path = dir / file;
            // if file exists, load it
            if (fs::exists(full_path)) {
                checkpoint_found = true;
                checkpoint_path = full_path;
                i += 1;
            } else
                break;
        }
    }

    if(load_current_best || checkpoint_found) {

        //load_train_examples(checkpoint_path);
        //for(auto & example : m_train_examples)
        //    example.convert_board();

        if(fs::exists(fs::path(m_model_folder + "best.pth.tar"))) {
            m_nnet->load_checkpoint(m_model_folder, "best.pth.tar");
        }
    }

    for(int iter = 0; iter < m_num_iters; ++iter) {
        std::vector<TrainingTurn> train_examples{m_train_examples.begin(), m_train_examples.end()};
        if(!skip_first_self_play || iter > 0) {
            for(int episode = 0; episode < m_num_episodes; ++episode) {
                for(auto && example : exec_ep(*(m_game->get_gamestate()))) {
                    example.convert_board();
                    train_examples.emplace_back(std::move(example));
                }
            }
        }
        // TODO: Activate this part once serialization has been solved.
        //if(!skip_first_self_play || iter > 0) {
        //    save_train_examples(iter);
        //}

        if(int nr_exs_to_pop = m_train_examples.size() - m_num_iters_train_examples_hist; nr_exs_to_pop > 0) {
            for(int k = 0; k < nr_exs_to_pop; ++k) {
                m_train_examples.pop_front();
            }
        }

        m_nnet->save_checkpoint(m_model_folder, "temp.pth.tar");
        m_nnet->to(torch_utils::GLOBAL_DEVICE::get_device());
        m_nnet->train(train_examples, /*epochs=*/100, /*batch_size=*/4096);

        m_opp_nnet->load_checkpoint(m_model_folder, "temp.pth.tar");
        auto [res0, res1] = Arena::pit(*m_game, 1000);
        if(res0.wins + res1.wins > 0 && (res0.wins / (res0.wins + res1.wins) < m_win_frac)) {
            std::cout << "Rejecting new model\n";
            m_nnet->load_checkpoint(m_model_folder, "temp.pth.tar");
        }
        else {
            std::cout << "Accepting new model\n";
            m_nnet->save_checkpoint(m_model_folder, "checkpoint_" + std::to_string(iter) + ".pth.tar");
            m_nnet->save_checkpoint(m_model_folder, "best.pth.tar");
        }
    }
}

void Coach::save_train_examples(int iteration) {
// TODO: I need to find a serialization method ._.
	return;
}

void Coach::load_train_examples(std::string examples_fname) {
	return;
}

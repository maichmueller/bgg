#pragma once

#include "string"
#include "memory"
#include "vector"
#include "deque"

#include <azpp/game/Arena.h>
#include "azpp/nn/model/NeuralNet.h"
#include "azpp/mcts/MCTS.h"
#include "azpp/game/Game.h"
#include "azpp/board/Board.h"


template<typename StateType>
struct EvaluatedGameTurn {
    using state_type = StateType;

    state_type m_state;
    torch::Tensor m_board_tensor;
    std::vector<double> m_pi;
    double m_v;
    int m_player;

    bool converted = false;

    EvaluatedGameTurn(const state_type &state, std::vector<double> pi, double v, int player)
            : m_state(state),
              m_board_tensor(),
              m_pi(std::move(pi)),
              m_v(v),
              m_player(player) {}

    template<class ActionRepresenterType>
    void convert_board(
            RepresenterBase<
                    StateType,
                    ActionRepresenterType
            >  & action_repper
            ) {
        /*
         * Conversion function for turning a Board object into a torch tensor.
         * This is needed, because the training turns are stored as original Board
         * objects, so that they can be reused, in case the state representation
         * changes later on, which were to render an already converted state tensor
         * useless.
         */
        if (converted)
            return;
        m_board_tensor = action_repper.state_representation(m_state, 0);
        converted = true;
    }

    const state_type & get_state() const { return m_state; }

    const torch::Tensor & get_tensor() const { return m_board_tensor; }

    std::vector<double> get_policy() const { return m_pi; }

    double get_evaluation() const { return m_v; }

    int get_player() const { return m_player; }

};


template<class GameType, class NetworkType>
class Coach {

public:
    using game_type = GameType;
    using state_type = typename game_type::state_type;
    using evaluated_turn_type = EvaluatedGameTurn<state_type>;
    using move_type = typename state_type::move_type;
    using network_type = NetworkType;

private:

    std::shared_ptr<game_type> m_game;
    std::shared_ptr<network_type> m_nnet;
    std::shared_ptr<network_type> m_opp_nnet;

    std::deque<evaluated_turn_type> m_turns_queue;

    float m_win_frac = 0.55;
    unsigned int m_num_iters = 1000;
    unsigned int m_num_episodes = 1000;
    unsigned int m_num_mcts_simulations = 1000;
    unsigned int m_num_evaluated_turns_hist = 10e5;
    std::string m_model_folder = "checkpoints";
    unsigned int m_exploration_rate = 100;


public:

    Coach(std::shared_ptr<game_type> game,
          const std::shared_ptr<network_type> &nnet,
          int num_iters = 100,
          float win_frac = 0.55,
          int num_episodes = 100,
          int num_mcts_sims = 100,
          int num_iters_train_examples_hist = 10e5,
          std::string model_folder = "checkpoints",
          int exploration_rate = 100);

    template<typename ActionRepresenterType>
    std::vector<evaluated_turn_type> execute_episode(
            state_type &state,
            RepresenterBase<
                    state_type,
                    ActionRepresenterType
            >  & action_repper
    );

    template<typename ActionRepresenterType>
    void teach(
            RepresenterBase<
                    state_type,
                    ActionRepresenterType
            > & action_repper,
            bool from_prev_examples = false,
            bool load_current_best = false,
            bool skip_first_self_play = false,
            bool multiprocess = false
    );

    void save_train_examples(int iteration);

    void load_train_examples(std::string examples_fname);

    auto get_nnet() { return m_nnet; }

    auto get_opp_nnet() { return m_opp_nnet; }

    auto get_game() { return m_game; }

};

template<class GameType, class NetworkType>
Coach<GameType, NetworkType>::Coach(
        std::shared_ptr<game_type> game,
        const std::shared_ptr<network_type> &nnet,
        int num_iters,
        float win_frac,
        int num_episodes,
        int num_mcts_sims,
        int num_iters_train_examples_hist,
        std::string model_folder,
        int exploration_rate)
        : m_game(std::move(game)),
          m_nnet(nnet),
          m_opp_nnet(std::make_shared<network_type>(*nnet)),
          m_win_frac(win_frac),
          m_num_iters(num_iters),
          m_num_episodes(num_episodes),
          m_num_mcts_simulations(num_mcts_sims),
          m_num_evaluated_turns_hist(num_iters_train_examples_hist),
          m_model_folder(std::move(model_folder)),
          m_exploration_rate(exploration_rate) {
    m_nnet->to(GLOBAL_DEVICE::get_device());
    m_opp_nnet->to(GLOBAL_DEVICE::get_device());
}


template<class GameType, class NetworkType>
template<class ActionRepresenterType>
std::vector<typename Coach<GameType, NetworkType>::evaluated_turn_type>
Coach<GameType, NetworkType>::execute_episode(
        state_type &state,
        RepresenterBase<
                state_type,
                ActionRepresenterType
        > & action_repper) {

    unsigned int ep_step = 0;

    std::vector<evaluated_turn_type> ep_examples;

    MCTS mcts = MCTS(m_nnet, m_num_mcts_simulations);

    const int null_v = -100;

    while (true) {
        ep_step += 1;
        auto expl_rate = static_cast<unsigned int>(ep_step < m_exploration_rate);

        int player = state.get_move_count() % 2;
        std::vector<double> pi = mcts.get_action_probabilities(state, player, action_repper, expl_rate);

//        std::cout << "After action probs: " << state.get_board()->size()<< "\n";
        std::default_random_engine generator{std::random_device()()};
        std::discrete_distribution<int> qs_sampler{pi.begin(), pi.end()};

//        unsigned int action_index = qs_sampler(generator);
//        move_type move = action_repper.action_to_move(state, action_index, player);

//        std::cout << "After action to move: " << state.get_board()->size()<< "\n";
        ep_examples.emplace_back(evaluated_turn_type(state, pi, null_v, player));
//
//        std::cout << "Move: (" << move[0][0] << ", " << move[0][1] << ") -> ("
//                  << move[1][0] << ", " << move[1][1] << ")" << "\n";
//        std::cout << "Board before move done: \n"
//                  << state.get_board->print_board(false, false) << "\n";
//        state.do_move(move);
//        std::cout << "Board after move done: \n" << state.get_board->print_board(false, false)
//                  << "\n";

        int r = state.is_terminal(true);

        if (r != 404) {
            for (auto &train_turn : ep_examples) {
                // since v is always returned as -v from the MCTS _search (the end-value
                // as seen from the opponent's side), we will have to adapt the value
                // in each turn to reflect the changing player.
                train_turn.m_v = player != train_turn.m_player ? r : -r;
            }
//            delete mcts;
            return ep_examples;
        }
    }
}


template <class GameType, class NetworkType>
template <typename ActionRepresenterType>
void Coach<GameType, NetworkType>::teach(
        RepresenterBase<
                state_type,
                ActionRepresenterType
        > & action_repper,
        bool from_prev_examples,
        bool load_current_best,
        bool skip_first_self_play,
        bool multiprocess) {

    bool checkpoint_found = false;

    namespace fs = std::filesystem;
    fs::path checkpoint_path;
    if (from_prev_examples) {
        int i = 0;

        while (true) {
            fs::path dir(m_model_folder);
            fs::path file("checkpoint_" + std::to_string(i) + ".pth.tar");
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

    if (load_current_best || checkpoint_found) {

        //load_train_examples(checkpoint_path);
        //for(auto & example : m_train_examples)
        //    example.convert_board();

        if (fs::exists(fs::path(m_model_folder + "best.pth.tar"))) {
            m_nnet->load_checkpoint(m_model_folder, "best.pth.tar");
        }
    }

    for (size_t iter = 0; iter < m_num_iters; ++iter) {

        std::vector<evaluated_turn_type> train_data{m_turns_queue.begin(), m_turns_queue.end()};

        if (!skip_first_self_play || iter > 0) {
            for (size_t episode = 0; episode < m_num_episodes; ++episode) {
                for (auto &&evaluated_turn : execute_episode(
                        *(m_game->get_gamestate()),
                        action_repper)) {
                    evaluated_turn.convert_board(action_repper);
                    train_data.emplace_back(evaluated_turn);
                }
            }
        }

        // TODO: Activate this part once serialization has been solved.
        //if(!skip_first_self_play || iter > 0) {
        //    save_train_examples(iter);
        //}

        if (int nr_exs_to_pop = m_turns_queue.size() - m_num_evaluated_turns_hist; nr_exs_to_pop > 0) {
            for (int k = 0; k < nr_exs_to_pop; ++k) {
                m_turns_queue.pop_front();
            }
        }

        m_nnet->save_checkpoint(m_model_folder, "temp.pth.tar");
        m_nnet->to(GLOBAL_DEVICE::get_device());
        m_nnet->train(train_data, /*epochs=*/100, /*batch_size=*/4096);

        m_opp_nnet->load_checkpoint(m_model_folder, "temp.pth.tar");
        auto[res0, res1] = Arena::pit(*m_game, 1000);
        if (res0.wins + res1.wins > 0 && (res0.wins / (res0.wins + res1.wins) < m_win_frac)) {
            std::cout << "Rejecting new model\n";
            m_nnet->load_checkpoint(m_model_folder, "temp.pth.tar");
        } else {
            std::cout << "Accepting new model\n";
            m_nnet->save_checkpoint(m_model_folder, "checkpoint_" + std::to_string(iter) + ".pth.tar");
            m_nnet->save_checkpoint(m_model_folder, "best.pth.tar");
        }
    }
}


template<class GameType, class NetworkType>
void Coach<GameType, NetworkType>::save_train_examples(int iteration) {
// TODO: I need to find a serialization method ._.
    return;
}

template<class GameType, class NetworkType>
void Coach<GameType, NetworkType>::load_train_examples(std::string examples_fname) {
    return;
}
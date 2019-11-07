#pragma once

#include "string"
#include "memory"
#include "vector"
#include "deque"

#include "nn/model/NeuralNet.h"
#include "mcts/MCTS.h"
#include "game/Game.h"
#include "board/Board.h"


template<typename BoardType>
struct TrainData {
    using board_type = BoardType;

    board_type m_board;
    torch::Tensor m_board_tensor;
    std::vector<double> m_pi;
    double m_v;
    int m_player;

    bool converted = false;

    TrainData(const board_type &board, std::vector<double> pi, double v, int player)
            : m_board(board), m_pi(std::move(pi)), m_v(v), m_player(player), m_board_tensor() {}

    template<class RepresenterType>
    void convert_board() {
        /*
         * Conversion function for turning a Board object into a torch tensor.
         * This is needed, because the training turns are stored as original Board
         * objects, so that they can be reused, in case the state representation
         * changes later on, which were to render an already converted state tensor
         * useless.
         */
        if (converted)
            return;
        m_board_tensor = RepresenterType::state_representation(
                m_board,
                0);
        converted = true;
    }

    board_type *get_board() {return m_board;}

    torch::Tensor *get_tensor() {return m_board_tensor;}

    std::vector<double> *get_policy() {return m_pi;}

    double get_evaluation() {return m_v;}

    int get_player() {return m_player;}

};


template<class GameType, class NeuralNetworkType, class ActionRepType>
class Coach {

public:
    using game_type = GameType;
    using state_type = typename game_type::state_type;
    using train_data_type = TrainData<game_type>;
    using move_type = typename state_type::move_type;
    using neural_network_type = NeuralNetworkType;
    using action_rep_type = ActionRepType;

private:
    action_rep_type m_action_representer;

    float m_win_frac = 0.55;
    int m_num_iters = 100;
    int m_num_episodes = 100;
    int m_num_mcts_simulations = 100;
    int m_num_iters_train_examples_hist = 10e5;
    std::string m_model_folder = "checkpoints";
    int m_exploration_rate = 100;

    std::shared_ptr<game_type> m_game;
    std::shared_ptr<neural_network_type> m_nnet;
    std::shared_ptr<neural_network_type> m_opp_nnet;

    std::deque<train_data_type> m_train_examples;

public:

    Coach(std::shared_ptr<game_type> game,
          const std::shared_ptr<neural_network_type > & nnet,
          const action_rep_type & action_representer,
          int num_iters = 100,
          float win_frac = 0.55,
          int num_episodes = 100,
          int num_mcts_sims = 100,
          int num_iters_train_examples_hist = 10e5,
          std::string model_folder = "checkpoints",
          int exploration_rate = 100);

    std::vector<train_data_type> execute_episode(const state_type &state) const;

    void teach(
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

template<class GameType, class NeuralNetworkType, class ActionRepType>
Coach<GameType, NeuralNetworkType, ActionRepType>::Coach(
        std::shared_ptr<game_type > game,
        const std::shared_ptr<neural_network_type > & nnet,
        const action_rep_type & action_representer,
        int num_iters,
        float win_frac,
        int num_episodes,
        int num_mcts_sims,
        int num_iters_train_examples_hist,
        std::string model_folder,
        int exploration_rate)
        : m_game(std::move(game)),
          m_nnet(nnet),
          m_opp_nnet(std::make_shared<neural_network_type>(*nnet)),
          m_num_iters(num_iters),
          m_win_frac(win_frac),
          m_num_episodes(num_episodes),
          m_num_iters_train_examples_hist(num_iters_train_examples_hist),
          m_num_mcts_simulations(num_mcts_sims),
          m_model_folder(std::move(model_folder)),
          m_exploration_rate(exploration_rate) {
    m_nnet->to_device(GLOBAL_DEVICE::get_device());
    m_opp_nnet->to_device(GLOBAL_DEVICE::get_device());
}


template<class GameType, class NeuralNetworkType, class ActionRepType>
std::vector<typename Coach<GameType, NeuralNetworkType, ActionRepType>::train_data_type>
Coach<GameType, NeuralNetworkType, ActionRepType>::execute_episode(const state_type &state) const {

    unsigned int ep_step = 0;

    std::vector<train_data_type> ep_examples;

    MCTS mcts = MCTS(m_nnet, m_num_mcts_simulations);

    const int null_v = -100;

    while (true) {
        ep_step += 1;
        auto expl_rate = static_cast<unsigned int>(ep_step < m_exploration_rate);

        unsigned int turn = state.get_move_count() % 2;
        std::vector<double> pi = mcts.get_action_probs(state, turn, expl_rate);

//        std::cout << "After action probs: " << state.get_board()->size()<< "\n";
        std::default_random_engine generator{std::random_device()()};
        std::discrete_distribution<int> qs_sampler{pi.begin(), pi.end()};

        unsigned int action = qs_sampler(generator);
        move_type move = state.action_to_move(action, turn);
        if (turn == 1)
            move = MCTS::flip_move(move, state.get_board()->get_shape());
//        std::cout << "After action to move: " << state.get_board()->size()<< "\n";
        ep_examples.emplace_back(train_data_type(*state.get_board(), pi, null_v, turn));

        std::cout << "Move: (" << move[0][0] << ", " << move[0][1] << ") -> ("
                  << move[1][0] << ", " << move[1][1] << ")" << "\n";
        std::cout << "Board before move done: \n"
                  << utils::board_str_rep<Board, Piece>(*state.get_board(), false, false) << "\n";
        state.do_move(move);
        std::cout << "Board after move done: \n" << utils::board_str_rep<Board, Piece>(*state.get_board(), false, false)
                  << "\n";

        int r = state.is_terminal(true, /*turn=*/0);

        if (r != 404) {
            for (auto &train_turn : ep_examples) {
                // since v is always return as -v from the MCTS _search (the endvalue
                // as seen from the opponent's side), we will have to adapt the value
                // in each turn to reflect the changing player.
                train_turn.m_v = turn != train_turn.m_player ? r : -r;
            }
            return ep_examples;
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
        m_nnet->to(GLOBAL_DEVICE::get_device());
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

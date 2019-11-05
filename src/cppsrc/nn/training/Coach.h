#pragma once

#include "string"
#include "memory"
#include "vector"
#include "deque"

#include "nn/model/NeuralNet.h"
#include "game/Game.h"
#include "board/Board.h"


template <typename BoardType>
struct TrainData {
    using board_type = BoardType;

    board_type m_board;
    torch::Tensor m_board_tensor;
    std::vector<double> m_pi;
    double m_v;
    int m_player;

    bool converted = false;

    TrainData(const board_type & board, std::vector<double> pi, double v, int player)
    : m_board(board), m_pi(std::move(pi)), m_v(v), m_player(player), m_board_tensor()
    {}

    template <class RepresenterType>
    void convert_board() {
        /*
         * Conversion function for turning a Board object into a torch tensor.
         * This is needed, because the training turns are stored as original Board
         * objects, so that they can be reused, in case the state representation
         * changes later on, and thus rendering an already converted state tensor
         * useless.
         */
        if(converted)
            return;
        m_board_tensor = RepresenterType::state_representation(
                m_board,
                0);
        converted = true;
    }
    board_type * get_board() {
        return m_board;
    }
    torch::Tensor * get_tensor() {
        return m_board_tensor;
    }
    std::vector<double> * get_policy() {
        return m_pi;
    }
    double get_evaluation() {
        return m_v;
    }
    int get_player() {
        return m_player;
    }

};


template <class GameType>
class Coach {

public:
    using game_type = GameType;
    using state_type = typename game_type::state_type;
    using train_data_type = TrainData<game_type>;
    using move_type = typename state_type::move_type;

private:
    int m_num_iters = 100;
    float m_win_frac = 0.55;
    int m_num_episodes = 100;
    int m_num_mcts_simulations = 100;
    int m_num_iters_train_examples_hist = 10e5;
    std::string m_model_folder = "checkpoints";
    int m_exploration_rate = 100;

    std::shared_ptr<game_type> m_game;
    std::shared_ptr<NetworkWrapper> m_nnet;
    std::shared_ptr<NetworkWrapper> m_opp_nnet;

    std::deque<train_data_type> m_train_examples;

public:

    Coach(std::shared_ptr<game_type> game,
            std::shared_ptr<NetworkWrapper> nnet,
            std::shared_ptr<NetworkWrapper> opp_nnet,
            int num_iters = 100,
            float win_frac = 0.55,
            int num_episodes = 100,
            int num_mcts_sims = 100,
            int num_iters_train_examples_hist = 10e5,
            std::string model_folder = "checkpoints",
            int exploration_rate = 100);

    std::vector<train_data_type> execute_episode(const state_type & state) const;

    void teach(
            bool from_prev_examples=false,
            bool load_current_best=false,
            bool skip_first_self_play=false,
            bool multiprocess=false
    );

    void save_train_examples(int iteration);
    void load_train_examples(std::string examples_fname);

    auto get_nnet() {return m_nnet;}
    auto get_opp_nnet() {return m_opp_nnet;}
    auto get_game() {return m_game;}

};


template <class GameType>
std::vector<Coach<GameType>::train_data_type> Coach<GameType>::execute_episode(const state_type &state) const {

    int ep_step = 0;

    std::vector<train_data_type > ep_examples;

    MCTS mcts = MCTS(m_nnet, m_num_mcts_simulations);

    constexpr int null_v = -100;

    while(true) {
        ep_step += 1;
        int expl_rate = static_cast<int>(ep_step < m_exploration_rate);

        int turn = state.get_move_count() % 2;
        std::vector<double> pi = mcts.get_action_probs(state, /*player=*/turn, expl_rate);

//        std::cout << "After action probs: " << state.get_board()->size()<< "\n";
        std::default_random_engine generator{std::random_device()()};
        std::discrete_distribution<int> qs_sampler {pi.begin(), pi.end()};

        int action = qs_sampler(generator);
        move_type move = state.action_to_move(action, turn);
        if(turn==1)
            move = MCTS::flip_move(move, state.get_board()->get_shape());
//        std::cout << "After action to move: " << state.get_board()->size()<< "\n";
        ep_examples.emplace_back(train_data_type(*state.get_board(), pi, null_v, turn));

        std::cout << "Move: (" << move[0][0] << ", " << move[0][1] << ") -> ("
                  << move[1][0] << ", " << move[1][1] << ")" << "\n";
        std::cout  << "Board before move done: \n" << utils::board_str_rep<Board, Piece>(*state.get_board(), false, false) << "\n";
        state.do_move(move);
        std::cout  << "Board after move done: \n" << utils::board_str_rep<Board, Piece>(*state.get_board(), false, false) << "\n";

        int r = state.is_terminal(true, /*turn=*/0);

        if(r != 404) {
            for(auto & train_turn : ep_examples) {
                // since v is always return as -v from the MCTS _search (the endvalue
                // as seen from the opponent's side), we will have to adapt the value
                // in each turn to reflect the changing player.
                train_turn.m_v = turn != train_turn.m_player ? r : -r;
            }
            return ep_examples;
        }
    }
}
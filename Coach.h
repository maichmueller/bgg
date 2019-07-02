//
// Created by michael on 05.06.19.
//

#ifndef STRATEGO_CPP_COACH_H
#define STRATEGO_CPP_COACH_H

#include "string"
#include "memory"
#include "vector"
#include "deque"

#include "NeuralNetwork.h"
#include "Game.h"
#include "Board.h"
#include "StateRepresentation.h"


struct TrainingTurn {
    Board m_board;
    torch::Tensor m_board_tensor;
    std::vector<double> m_pi;
    double m_v;
    int m_player;

    bool converted = false;

    TrainingTurn(Board board, std::vector<double> pi, double v, int player)
    : m_board(std::move(board)), m_pi(std::move(pi)), m_v(v), m_player(player), m_board_tensor()
    {}
    void convert_board() {
        /*
         * Coversion function for turning a Board object into a torch tensor.
         * This is needed, because the training turns are stored as original Board
         * objects, so that they can be reused, in case the state representation
         * changes later on, and thus rendering an already converted state tensor
         * useless.
         */
        if(converted)
            return;
        m_board_tensor = StateRepresentation::b2s_cond_check(
                m_board,
                StateRepresentation::state_torch_conv_conditions_0.size(),
                m_board.get_board_len(),
                StateRepresentation::state_torch_conv_conditions_0,
                0);
        converted = true;
    }
    Board& get_board() {
        return m_board;
    }
    torch::Tensor& get_tensor() {
        return m_board_tensor;
    }
    std::vector<double>& get_policy() {
        return m_pi;
    }
    double get_evaluation() {
        return m_v;
    }
    int get_player() {
        return m_player;
    }

};


class Coach {

    int m_num_iters = 100;
    float m_win_frac = 0.55;
    int m_num_episodes = 100;
    int m_num_mcts_simulations = 100;
    int m_num_iters_train_examples_hist = 10e5;
    std::string m_model_folder = "checkpoints";
    int m_exploration_rate = 100;

    std::shared_ptr<Game> m_game;
    std::shared_ptr<NetworkWrapper> m_nnet;
    std::shared_ptr<NetworkWrapper> m_opp_nnet;

    std::deque<TrainingTurn> m_train_examples;

public:

    Coach(std::shared_ptr<Game> game,
            std::shared_ptr<NetworkWrapper> nnet,
            std::shared_ptr<NetworkWrapper> opp_nnet,
            int num_iters = 100,
            float win_frac = 0.55,
            int num_episodes = 100,
            int num_mcts_sims = 100,
            int num_iters_train_examples_hist = 10e5,
            std::string model_folder = "checkpoints",
            int exploration_rate = 100);

    std::vector<TrainingTurn> exec_ep(GameState state) const;
    void teach(bool from_prev_examples=false, bool load_current_best=false, bool skip_first_self_play=false,
            bool multiprocess=false);

    void save_train_examples(int iteration);
    void load_train_examples(std::string examples_fname);

};


#endif //STRATEGO_CPP_COACH_H

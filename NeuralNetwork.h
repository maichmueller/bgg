//
// Created by michael on 13.04.19.
//

#ifndef STRATEGO_CPP_NEURALNETWORK_H
#define STRATEGO_CPP_NEURALNETWORK_H


#include <random>
#include <algorithm>

#include "torch/torch.h"
#include "tqdm/tqdm.h"

class NeuralNetwork {

    std::shared_ptr<torch::nn::Module> nnet;
    int board_dim;
    int action_dim;

    std::vector<int> _sample_without_replacement(int pool_len, int sample_len);

public:

    void to_device();
    template<typename Board>
    void train(std::vector<std::tuple<Board, std::vector<double>, int, int>> train_examples,
            int epochs, int batch_size=128);

    template<typename Board>
    std::tuple<std::vector<double>, double> predict(Board& board);

    void save_checkpoint(std::string folder, std::string filename);
    void load_checkpoint(std::string folder, std::string filename);


};


#endif //STRATEGO_CPP_NEURALNETWORK_H

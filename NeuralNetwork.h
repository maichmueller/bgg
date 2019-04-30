//
// Created by michael on 13.04.19.
//

#ifndef STRATEGO_CPP_NEURALNETWORK_H
#define STRATEGO_CPP_NEURALNETWORK_H


#include <random>
#include <algorithm>

#include "torch/torch.h"
#include "tqdm/tqdm.h"
#include "torch_utils.h"


class Convolutional : torch::nn::Module {

    int channels_in;
    std::vector<int> filter_amounts;
    std::vector<int> kernel_sizes;
    std::vector<bool> maxpool_used_vec;
    std::vector<float> droput_prop_per_layer;

    std::vector<torch::nn::Conv2d> conv_layers;

public:

    Convolutional(int channels,
            const std::vector<int> & kernel_sizes_vec = std::vector<int> {3, 3, 3, 3},
            const std::vector<bool> & maxpool_used_vec = std::vector<bool> {false, false, false, false},
            const std::vector<float> & dropout_probs = std::vector<float> {0.0, 0.0, 0.0, 0.0});

    torch::Tensor forward (const torch::Tensor & input);

};


class AlphaZeroInterface : public torch::nn::Module {

public:

    virtual std::tuple<torch::Tensor, torch::Tensor> forward(const torch::Tensor & input) = 0;
};



class NetworkWrapper {

    std::shared_ptr<AlphaZeroInterface> nnet;
    int board_dim;
    int action_dim;

    static std::vector<int> _sample_without_replacement(int pool_len, int sample_len);

public:

    NetworkWrapper(const std::shared_ptr<AlphaZeroInterface> & net, int board_len, int action_len)
    : nnet(net), board_dim(board_len), action_dim(action_len) {}

    void to_device(torch::Device dev);

    static torch::Tensor loss_pi(const torch::Tensor& targets, const torch::Tensor& outputs);
    static torch::Tensor loss_v(const torch::Tensor& targets, const torch::Tensor& outputs);


    void train(std::vector<std::tuple<torch::Tensor, std::vector<double>, int, int>> train_examples,
            int epochs, int batch_size=128);

    std::tuple<torch::Tensor, double> predict(torch::Tensor& board_tensor);

    void save_checkpoint(std::string folder, std::string filename);
    void load_checkpoint(std::string folder, std::string filename);


    /// Forwarding methods for torch::nn::Module within nnet
    auto to(torch::Device device) {nnet->to(device);}



};


#endif //STRATEGO_CPP_NEURALNETWORK_H

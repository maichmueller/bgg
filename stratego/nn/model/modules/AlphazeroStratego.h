#pragma once

#include <torch/torch.h>

#include <azpp/utils.h>
#include <azpp/nn.h>


class StrategoAlphaZero : public AlphaZeroInterface {

    int D_in;
    std::unique_ptr<Convolutional> convo_layers;
    std::unique_ptr<FullyConnected> linear_layers;
    torch::nn::Linear pi_act_layer;
    torch::nn::Linear v_act_layer;

public:

    StrategoAlphaZero(
            int D_in,
            int D_out,
            int nr_lin_layers,
            int start_exponent,
            int channels,
            std::vector<int> filter_sizes,
            std::vector<int> kernel_sizes_vec,
            std::vector<bool> maxpool_used_vec,
            std::vector<float> dropout_probs,
            const torch::nn::Functional &activation_function = torch::nn::Functional(torch::relu)
    );

    std::tuple<torch::Tensor, torch::Tensor> forward(const torch::Tensor &input) override;

    void to_device(torch::Device device);
};


StrategoAlphaZero::StrategoAlphaZero(int D_in, int D_out,
                                     int nr_lin_layers, int start_exponent,
                                     int channels,
                                     std::vector<int> filter_sizes,
                                     std::vector<int> kernel_sizes_vec,
                                     std::vector<bool> maxpool_used_vec,
                                     std::vector<float> dropout_probs,
                                     const torch::nn::Functional &activation_function)
        : D_in(D_in),
          convo_layers(
                  std::move(
                          std::make_unique<Convolutional>
                                  (
                                          channels,
                                          std::move(filter_sizes),
                                          std::move(kernel_sizes_vec),
                                          std::move(maxpool_used_vec),
                                          std::move(dropout_probs),
                                          activation_function
                                  )
                  )
          ),
          pi_act_layer(nullptr), v_act_layer(nullptr) {
    if (nr_lin_layers < 2) {
        throw std::invalid_argument("Less than 2 linear layers requested. Aborting.");
    }

    unsigned int hidden_nodes = 2u << static_cast<unsigned int>(start_exponent - 1);
    unsigned int substitute_d_out = hidden_nodes / (2u << static_cast<unsigned int>(nr_lin_layers - 4));

    linear_layers = std::make_unique<FullyConnected>(
            D_in,
            substitute_d_out,
            nr_lin_layers - 1,
            start_exponent,
            activation_function);

    pi_act_layer = torch::nn::Linear(
            torch::nn::LinearOptions(substitute_d_out, D_out)
    );
    v_act_layer = torch::nn::Linear(
            torch::nn::LinearOptions(substitute_d_out, 1)
    );

}

std::tuple<torch::Tensor, torch::Tensor> StrategoAlphaZero::forward(const torch::Tensor &input) {
    input.to(GLOBAL_DEVICE::get_device());

    torch::Tensor output = convo_layers->forward(input).view({-1, D_in});
    output = linear_layers->forward(output);

    torch::Tensor pi = torch::log_softmax(
            pi_act_layer->forward(output),
            1
    );
    torch::Tensor v = torch::tanh(v_act_layer->forward(output));

    return std::make_tuple(pi, v);
}

void StrategoAlphaZero::to_device(torch::Device device) {
    this->to(device);
    convo_layers->to(device);
    linear_layers->to(device);
    pi_act_layer->to(device);
    v_act_layer->to(device);
}


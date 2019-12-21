#pragma once

#include "torch/torch.h"

class FullyConnected : public torch::nn::Module {

    int m_D_in;
    int m_D_out;
    int m_nr_lin_layers;

    torch::nn::Sequential m_layers;

public:
    FullyConnected(
            int D_in,
            int D_out,
            int nr_lin_layers = 2,
            int start_expo = 8,
            const torch::nn::Functional &activation_function = torch::nn::Functional(torch::relu)
    );

    torch::Tensor forward(const torch::Tensor &input);

    int get_intput_dim() { return m_D_in; }

    int get_output_dim() { return m_D_out; }

    int get_nr_layers() { return m_nr_lin_layers; }
};
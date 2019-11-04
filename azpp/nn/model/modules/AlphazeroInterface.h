#pragma once

#include "torch/torch.h"


class AlphaZeroInterface : public torch::nn::Module {
public:
    virtual std::tuple<torch::Tensor, torch::Tensor> forward(const torch::Tensor & input) = 0;
    virtual void to_device(torch::Device device) = 0;
};
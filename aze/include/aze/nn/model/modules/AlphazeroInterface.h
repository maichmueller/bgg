#pragma once

#include "torch/torch.h"

class AlphaZeroBase: public torch::nn::Module {
  public:
   virtual std::tuple< torch::Tensor, torch::Tensor > forward(const torch::Tensor& input) = 0;
};
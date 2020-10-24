#pragma once

#include "torch/torch.h"

class Convolutional: public torch::nn::Module {
   unsigned int m_channels_in;
   std::vector< unsigned int > m_filter_sizes;
   std::vector< unsigned int > m_kernel_sizes;
   std::vector< bool > m_maxpool_used_vec;
   std::vector< float > m_droput_prop_per_layer;

   torch::nn::Sequential m_layers;

   void _build_layers_after_construction(
      unsigned int channels_in,
      std::vector< unsigned int >& filter_sizes,
      const std::vector< bool >& maxpool_used_vec,
      const torch::nn::Functional& activation_function);

  public:
   Convolutional(
      unsigned int channels_in,
      std::vector< unsigned int > filter_sizes,
      std::vector< unsigned int > kernel_sizes_vec,
      std::vector< bool > maxpool_used_vec,
      std::vector< float > dropout_probs,
      const torch::nn::Functional& activation_function = torch::nn::Functional(torch::relu));

   Convolutional(
      unsigned int channels_in,
      std::vector< unsigned int >&& filter_sizes,
      std::vector< unsigned int >&& kernel_sizes_vec,
      std::vector< bool >&& maxpool_used_vec,
      std::vector< float >&& dropout_probs,
      const torch::nn::Functional& activation_function = torch::nn::Functional(torch::relu));

   unsigned int get_channels_in() { return m_channels_in; }

   torch::Tensor forward(const torch::Tensor& input);
};

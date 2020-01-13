#pragma once

#include "torch/torch.h"

class Convolutional: public torch::nn::Module {
   int m_channels_in;
   std::vector< int > m_filter_sizes;
   std::vector< int > m_kernel_sizes;
   std::vector< bool > m_maxpool_used_vec;
   std::vector< float > m_droput_prop_per_layer;

   torch::nn::Sequential m_layers;

  public:
   Convolutional(
      int channels_in,
      std::vector< int >&& filter_sizes,
      const std::vector< int >&& kernel_sizes_vec,
      const std::vector< bool >&& maxpool_used_vec,
      const std::vector< float >&& dropout_probs,
      const torch::nn::Functional& activation_function =
         torch::nn::Functional(torch::relu));

   template < typename prim_type >
   Convolutional(
      int channels_in,
      const std::vector< int >& filter_sizes,
      const std::vector< int >& kernel_sizes_vec,
      const std::vector< bool >& maxpool_used_vec,
      prim_type dropout_prob_for_all = 0,
      const torch::nn::Functional& activation_function =
         torch::nn::Functional(torch::relu));

   torch::Tensor forward(const torch::Tensor& input);
};
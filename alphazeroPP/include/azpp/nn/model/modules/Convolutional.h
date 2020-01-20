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
      std::vector< unsigned int > & filter_sizes,
      const std::vector< bool >& maxpool_used_vec,
      const torch::nn::Functional& activation_function);

  public:
   Convolutional(
      unsigned int channels_in,
      std::vector< unsigned int > filter_sizes,
      std::vector< unsigned int > kernel_sizes_vec,
      std::vector< bool > maxpool_used_vec,
      std::vector< float > dropout_probs,
      const torch::nn::Functional& activation_function =
         torch::nn::Functional(torch::relu));

   Convolutional(
      unsigned int channels_in,
      std::vector< unsigned int >&& filter_sizes,
      std::vector< unsigned int >&& kernel_sizes_vec,
      std::vector< bool >&& maxpool_used_vec,
      std::vector< float >&& dropout_probs,
      const torch::nn::Functional& activation_function =
         torch::nn::Functional(torch::relu));

   //   template < typename prim_type >
   //   Convolutional(
   //      int channels_in,
   //      const std::vector< int >& filter_sizes,
   //      const std::vector< int >& kernel_sizes_vec,
   //      const std::vector< bool >& maxpool_used_vec,
   //      prim_type dropout_prob_for_all = 0,
   //      const torch::nn::Functional& activation_function =
   //         torch::nn::Functional(torch::relu));

   torch::Tensor forward(const torch::Tensor& input);
};



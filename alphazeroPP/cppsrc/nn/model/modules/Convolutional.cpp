//
// Created by Michael on 04.11.19.
//

#include "azpp/nn/model/modules/Convolutional.h"

#include "azpp/utils/torch_utils.h"

Convolutional::Convolutional(
   int channels_in,
   std::vector< int > &&filter_sizes,
   const std::vector< int > &&kernel_sizes_vec,
   const std::vector< bool > &&maxpool_used_vec,
   const std::vector< float > &&dropout_probs,
   const torch::nn::Functional &activation_function)
    : m_channels_in(channels_in),
      m_filter_sizes(filter_sizes),
      m_kernel_sizes(kernel_sizes_vec),
      m_maxpool_used_vec(maxpool_used_vec),
      m_droput_prop_per_layer(dropout_probs),
      m_layers()
{
   /**
    * Constructor for a pre-defined Convolutional Neural Network.
    * Options allow for including maxpooling and dropout layers
    * in the specified layer index if
    * "maxpool_used_vec[index] = true" and "dropout_prob[index] > 0"
    * respectively
    */

   int nr_conv_layers = m_kernel_sizes.size();

   std::vector< int > zero_padding(nr_conv_layers, 0);
   for(int i = 0; i < nr_conv_layers; ++i) {
      zero_padding[i] = (m_kernel_sizes[i] - 1) / 2;
   }

   filter_sizes.insert(filter_sizes.begin(), channels_in);

   for(int k = 0; k < nr_conv_layers; ++k) {
      // extend the layer sequential by a convolutional
      auto options = torch::nn::Conv2dOptions(
                        filter_sizes[k], filter_sizes[k + 1], m_kernel_sizes[k])
                        .stride(1)
                        .padding(zero_padding[k])
                        .with_bias(false);

      m_layers->push_back(torch::nn::Conv2d(options));

      // check for maxpool and/or dropout layer additions
      if(maxpool_used_vec[k]) {
         m_layers->push_back(torch::nn::Functional(
            torch::max_pool2d,
            /*kernel_size=*/3,
            /*stride=*/2,
            /*padding=*/0,
            /*dilation=*/1,
            false));
      }
      auto p_drop = m_droput_prop_per_layer[k];
      if(p_drop > 0) {
         m_layers->push_back(torch::nn::Functional(
            torch::dropout,
            /*p=*/p_drop,
            true));
      }

      // finish this layer-segment with a layer activation
      m_layers->push_back(activation_function);
   }
}

template < typename prim_type >
Convolutional::Convolutional(
   int channels_in,
   const std::vector< int > &filter_sizes,
   const std::vector< int > &kernel_sizes_vec,
   const std::vector< bool > &maxpool_used_vec,
   prim_type dropout_prob_for_all,
   const torch::nn::Functional &activation_function)
    : Convolutional(
       channels_in,
       filter_sizes,
       kernel_sizes_vec,
       maxpool_used_vec,
       std::vector< float >(4, static_cast< float >(dropout_prob_for_all)),
       activation_function)
{
}

torch::Tensor Convolutional::forward(const torch::Tensor &input)
{
   input.to(GLOBAL_DEVICE::get_device());
   return m_layers->forward(input);
}
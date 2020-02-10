#pragma once

#include <azpp/utils.h>
#include <torch/torch.h>
#include <tqdm/tqdm.h>

#include <algorithm>
#include <filesystem>
#include <random>

#include "azpp/nn/model/modules/AlphazeroInterface.h"
#include "azpp/utils/torch_utils.h"

// for some reason torch decided to change the standard ArrayRef value type
// between linux and mac. Might have to do with different size_t standards on
// linux and mac.
#ifdef __APPLE__
   #define TORCH_ARRAYREF_TYPE long long
#else
   #define TORCH_ARRAYREF_TYPE long
#endif

class NetworkWrapper {
   std::shared_ptr< AlphaZeroInterface > m_network;

   [[nodiscard]] std::vector< TORCH_ARRAYREF_TYPE > _prepend_to_shape(
      const torch::Tensor &tensor, size_t value) const;

   static inline torch::Tensor _loss_pi(
      const torch::Tensor &target_probs, const torch::Tensor &estimated_probs)
   {
      return (target_probs * estimated_probs).sum();
   }

   static inline torch::Tensor _loss_v(
      const torch::Tensor &target, const torch::Tensor &output)
   {
      return (target - output).pow(2).sum();
   }

  public:
   explicit NetworkWrapper(std::shared_ptr< AlphaZeroInterface > network_ptr)
       : m_network(std::move(network_ptr))
   {
   }

   template < typename TrainExampleContainer >
   void train(
      const TrainExampleContainer & train_examples,
      size_t epochs,
      size_t batch_size = 128);

   std::tuple< torch::Tensor, double > evaluate(
      const torch::Tensor &board_tensor);

   void save_checkpoint(std::string const &folder, std::string const &filename);

   void load_checkpoint(std::string const &folder, std::string const &filename);

   /// Forwarding method for torch::nn::Module within network
   void to(torch::Device device) { m_network->to(device); }
};

template < typename TrainDataContainer >
void NetworkWrapper::train(
   const TrainDataContainer & train_examples, size_t epochs, size_t batch_size)
{
   // send model to the right device
   to(GLOBAL_DEVICE::get_device());
   const size_t n_data = train_examples.size();
   const size_t batch_max_size = std::min(batch_size, n_data);
   std::vector<size_t> indices;
   indices.reserve(n_data);
   for(size_t i = 0; i < n_data; ++i) {
      indices.push_back(i);
   }

   auto optimizer = torch::optim::Adam(
      m_network->parameters(),
      torch::optim::AdamOptions(/*learning_rate=*/0.01));

   auto board_tensor_sizes = _prepend_to_shape(
      train_examples[0].get_tensor(), batch_max_size);
   auto pi_tensor_sizes = std::vector< TORCH_ARRAYREF_TYPE >{
      static_cast< TORCH_ARRAYREF_TYPE >(batch_max_size),
      static_cast< TORCH_ARRAYREF_TYPE >(
         train_examples[0].get_policy().size())};
   tqdm bar;
   bar.set_label("Training for " + std::to_string(epochs) + "epochs.");
   for(size_t epoch = 0; epoch < epochs; ++epoch) {
      bar.progress(epoch, epochs);
      // set the network into train mode
      m_network->train();
      std::shuffle(
         indices.begin(),
         indices.end(),
         std::mt19937{std::random_device{}()});

      for(size_t batch_nr = 0; batch_nr < n_data; batch_nr += batch_size) {
         // clear existing gradient residue.
         optimizer.zero_grad();
         // declare the tensors we will need for training.
         torch::TensorOptions options_grad = torch::TensorOptions()
                                                .device(
                                                   GLOBAL_DEVICE::get_device())
                                                .dtype(torch::kFloat)
                                                .requires_grad(false);

         torch::Tensor board = torch::empty(board_tensor_sizes, options_grad);
         torch::Tensor policy = torch::empty(pi_tensor_sizes, options_grad);
         torch::Tensor value = torch::ones(batch_size, options_grad);

         // fill the batch tensors with the batch data
         size_t begin = batch_nr * batch_size;
         for(size_t i = begin; i < std::min(n_data, begin + batch_size); ++i) {

            auto &sample = train_examples[indices[i]];

            board[i] = sample.get_tensor().squeeze(0);
            LOGD2("Sample " + std::to_string(i) + " value", sample.get_evaluation())
            value[i] *= sample.get_evaluation();
            LOGD2("Sample " + std::to_string(i) + " tensor value", value[i])
            for(auto [j, p] = std::make_tuple(0, sample.get_policy().begin()); j < pi_tensor_sizes[1]; ++j, ++p) {
               policy[i][j] = *p;
            }
            LOGD2("Sample " + std::to_string(i) + " poss actions" , torch::where(policy[i] > 0))
         }

         auto [policy_output, value_output] = m_network->forward(board);
//         LOGD2("Pol out sizes", policy_output.sizes())
//         LOGD2("Pol_out", policy_output)
//         LOGD2("Pol sizes", policy.sizes())
//         LOGD2("Pol", policy)
         auto l_pi = _loss_pi(policy, policy_output);
//         LOGD2(value, value_output)
         auto l_v = _loss_v(value, value_output);
         auto total_loss = l_pi + l_v;
         LOGD2("L_PI", l_pi)
         LOGD2("L_V", l_v)
         LOGD2("LOSS", total_loss)
         total_loss.backward();
         optimizer.step();
      }
   }
   bar.finish();
}

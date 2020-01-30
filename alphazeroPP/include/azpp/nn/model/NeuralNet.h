#pragma once

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
      const torch::Tensor &targets, const torch::Tensor &outputs)
   {
      return -(targets * outputs).sum() / targets.size(1);
   }

   static inline torch::Tensor _loss_v(
      const torch::Tensor &targets, const torch::Tensor &outputs)
   {
      return (targets - outputs).pow(2).sum() / targets.size(1);
   }

  public:
   explicit NetworkWrapper(std::shared_ptr< AlphaZeroInterface > network_ptr)
       : m_network(std::move(network_ptr))
   {
   }

   template < typename TrainExampleContainer >
   void train(
      TrainExampleContainer train_examples,
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
   TrainDataContainer train_examples, size_t epochs, size_t batch_size)
{
   // send model to the right device
   to(GLOBAL_DEVICE::get_device());

   auto optimizer = torch::optim::Adam(
      m_network->parameters(),
      torch::optim::AdamOptions(/*learning_rate=*/0.01));

   auto board_tensor_sizes = _prepend_to_shape(
      train_examples[0].get_tensor(), batch_size);
   auto pi_tensor_sizes = std::vector< TORCH_ARRAYREF_TYPE >{
      static_cast< TORCH_ARRAYREF_TYPE >(batch_size),
      static_cast< TORCH_ARRAYREF_TYPE >(
         train_examples[0].get_policy().size())};
   tqdm bar;
   bar.set_label("Training for " + std::to_string(epochs) + "epochs.");
   for(size_t epoch = 0; epoch < epochs; ++epoch) {
      bar.progress(epoch, epochs);
      // set the network into train mode
      m_network->train();
      std::shuffle(
         train_examples.begin(),
         train_examples.end(),
         std::mt19937{std::random_device{}()});

      const size_t n_data = train_examples.size();
      for(size_t b = 0; b < n_data; b += batch_size) {
         // clear existing gradient residue.
         optimizer.zero_grad();
         // declare the tensors we will need for training.
         std::vector< torch::Tensor > board_batch(batch_size);
         std::vector< std::vector< double > > pi_batch(batch_size);
         std::vector< int > v_batch(batch_size);

         torch::TensorOptions options_grad = torch::TensorOptions()
                                                .device(
                                                   GLOBAL_DEVICE::get_device())
                                                .dtype(torch::kFloat)
                                                .requires_grad(true);

         torch::Tensor board = torch::empty(board_tensor_sizes, options_grad);
         torch::Tensor policy = torch::empty(pi_tensor_sizes, options_grad);
         torch::Tensor value = torch::empty(batch_size, options_grad);

         // fill the batch tensors with the batch data
         size_t begin = b * batch_size;
         for(size_t i = begin; b < std::min(n_data, begin + batch_size); ++i) {
            auto &sample = train_examples[i];

            board[i] = sample.get_tensor();
            value[i] = sample.get_evaluation();
            policy[i] = torch::from_blob(
               sample.get_policy().data(), {pi_tensor_sizes[1]}, options_grad);
         }

         auto [policy_output, value_output] = m_network->forward(board);
         auto l_pi = _loss_pi(policy, policy_output);
         auto l_v = _loss_v(value, value_output);
         auto total_loss = l_pi + l_v;

         total_loss.backward();
         optimizer.step();
      }
   }
   bar.finish();
}

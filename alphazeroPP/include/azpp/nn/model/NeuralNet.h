#pragma once

#include <random>
#include <algorithm>
#include <filesystem>

#include <torch/torch.h>
#include <tqdm/tqdm.h>

#include "azpp/nn/model/modules/AlphazeroInterface.h"
#include "azpp/utils/torch_utils.h"


class NetworkWrapper {

    std::shared_ptr<AlphaZeroInterface> m_nnet;

    [[nodiscard]] std::vector<size_t> prepend_to_shape(
            const torch::Tensor &tensor,
            size_t value
    ) const;

    static inline torch::Tensor loss_pi(const torch::Tensor &targets, const torch::Tensor &outputs) {
        return -(targets * outputs).sum() / targets.size(1);
    }

    static inline torch::Tensor loss_v(const torch::Tensor &targets, const torch::Tensor &outputs) {
        return (targets - outputs).pow(2).sum() / targets.size(1);
    }

public:

    explicit NetworkWrapper(
            std::shared_ptr<AlphaZeroInterface> network)
            : m_nnet(std::move(network))
            {}

    template<typename TrainExampleContainer>
    void train(TrainExampleContainer train_examples,
               size_t epochs,
               size_t batch_size = 128);
    std::tuple<torch::Tensor, double> predict(const torch::Tensor &board_tensor);

    void save_checkpoint(std::string const &folder, std::string const &filename);
    void load_checkpoint(std::string const &folder, std::string const &filename);

    /// Forwarding method for torch::nn::Module within nnet
    void to(torch::Device device) { m_nnet->to(device); }

};


template<typename TrainExampleContainer>
void NetworkWrapper::train(
        TrainExampleContainer train_examples,
        size_t epochs,
        size_t batch_size) {
    // send model to the right device
    to(GLOBAL_DEVICE::get_device());

    auto optimizer = torch::optim::Adam(
            m_nnet->parameters(),
            torch::optim::AdamOptions(/*learning_rate=*/0.01));

    auto board_tensor_sizes = prepend_to_shape(train_examples[0].get_tensor(), batch_size);
    auto pi_tensor_sizes = prepend_to_shape(train_examples[0].get_policy(), batch_size);

    tqdm bar;
    for (size_t epoch = 0; epoch < epochs; ++epoch) {
        bar.progress(epoch, epochs);
        // set the nnet into train mode (i.e. demands gradient updates for tensors)
        m_nnet->train();

        for (size_t b = 0; b < train_examples.size(); b += batch_size) {

            // get a randomly drawn sample index batch
            std::vector<TrainExampleContainer> examples_batch(batch_size);
            std::sample(train_examples.begin(), train_examples.end(),
                        examples_batch.begin(),
                        batch_size,
                        std::mt19937{std::random_device{}()});

            std::vector<torch::Tensor> board_batch(batch_size);
            std::vector<std::vector<double>> pi_batch(batch_size);
            std::vector<int> v_batch(batch_size);

            torch::TensorOptions options_nograd = torch::TensorOptions()
                    .device(GLOBAL_DEVICE::get_device())
                    .dtype(torch::kFloat)
                    .requires_grad(false);

            torch::TensorOptions options_grad = torch::TensorOptions()
                    .device(GLOBAL_DEVICE::get_device())
                    .dtype(torch::kFloat)
                    .requires_grad(true);

            // i suppose the board_tensor doesnt need a gradient (as implied in options_int) as it is only used
            // to compute the network output
            torch::Tensor board_tensor = torch::empty(board_tensor_sizes, options_nograd);
            torch::Tensor policy_tensor = torch::empty(pi_tensor_sizes, options_grad);
            torch::Tensor value_tensor = torch::empty(batch_size, options_grad);

            // fill the batch tensors with the respective part of the sample-tuple
            for (const auto &[i, sample] : std::make_tuple(0, examples_batch)) {
                board_tensor[i] = sample.get_tensor();
                policy_tensor[i] = sample.get_policy();
                value_tensor[i] = sample.get_evaluation();
                ++i;
            }

            auto[policy_output, value_output] = m_nnet->forward(board_tensor);
            auto l_pi = loss_pi(policy_tensor, policy_output);
            auto l_v = loss_v(value_tensor, value_output);
            auto total_loss = l_pi + l_v;
            total_loss.requires_grad();

            optimizer.zero_grad();
            total_loss.backward();
            optimizer.step();
        }
    }
    bar.finish();
}

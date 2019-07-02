//
// Created by michael on 13.04.19.
//

#ifndef STRATEGO_CPP_NEURALNETWORK_H
#define STRATEGO_CPP_NEURALNETWORK_H


#include <random>
#include <algorithm>

#include "torch/torch.h"
#include "tqdm/tqdm.h"
#include "torch_utils.h"


class Convolutional : torch::nn::Module {

    int m_channels_in;
    std::vector<int> m_filter_sizes;
    std::vector<int> m_kernel_sizes;
    std::vector<bool> m_maxpool_used_vec;
    std::vector<float> m_droput_prop_per_layer;

    torch::nn::Sequential m_layers;

public:
    Convolutional(int channels_in,
                  std::vector<int>&& filter_sizes,
                  const std::vector<int>&& kernel_sizes_vec,
                  const std::vector<bool>&& maxpool_used_vec,
                  const std::vector<float>&& dropout_probs,
                  const torch::nn::Functional & activation_function=torch::nn::Functional(torch::relu));

    template <typename prim_type>
    Convolutional(int channels_in,
                  const std::vector<int> & filter_sizes,
                  const std::vector<int> & kernel_sizes_vec,
                  const std::vector<bool> & maxpool_used_vec,
                  prim_type dropout_prob_for_all = 0,
                  const torch::nn::Functional & activation_function=torch::nn::Functional(torch::relu));

    torch::Tensor forward (const torch::Tensor & input);

};

class FullyConnected : torch::nn::Module {

    int D_in;
    int D_out;
    int start_exponent;
    int nr_lin_layers;

    torch::nn::Sequential layers;

public:
    FullyConnected()
    : layers(nullptr) {}
    FullyConnected(int D_in, int D_out, int nr_lin_layers=2, int start_expo=8,
                   const torch::nn::Functional & activation_function=torch::nn::Functional(torch::relu));

    torch::Tensor forward (const torch::Tensor & input);

};

class AlphaZeroInterface : public torch::nn::Module {
public:
    virtual std::tuple<torch::Tensor, torch::Tensor> forward(const torch::Tensor & input) = 0;
};

class StrategoAlphaZero : public AlphaZeroInterface {

    int D_in;
    Convolutional convo_layers;
    FullyConnected linear_layers;
    torch::nn::Linear pi_act_layer;
    torch::nn::Linear v_act_layer;

public:

    StrategoAlphaZero(int D_in, int D_out,
                      int nr_lin_layers,
                      int start_exponent,
                      int channels,
                      std::vector<int> filter_sizes,
                      std::vector<int> kernel_sizes_vec,
                      std::vector<bool> maxpool_used_vec,
                      std::vector<float> dropout_probs,
                      const torch::nn::Functional & activation_function=torch::nn::Functional(torch::relu));

    std::tuple<torch::Tensor, torch::Tensor> forward(const torch::Tensor & input) override;

};

class NetworkWrapper {

    std::shared_ptr<AlphaZeroInterface> nnet;
    int board_dim;
    int action_dim;

    static std::vector<int> _sample_without_replacement(int pool_len, int sample_len);

public:

    NetworkWrapper(std::shared_ptr<AlphaZeroInterface> net, int board_len, int action_len)
    : nnet(std::move(net)), board_dim(board_len), action_dim(action_len)
    {}

    void to_device(torch::Device dev);

    static inline torch::Tensor loss_pi(const torch::Tensor& targets, const torch::Tensor& outputs) {
        return -(targets * outputs).sum() / targets.size(1);
    }
    static inline torch::Tensor loss_v(const torch::Tensor& targets, const torch::Tensor& outputs) {
        return (targets - outputs).pow(2).sum() / targets.size(1);
    }

    template <typename TrainExampleContainer>
    void train(TrainExampleContainer train_examples,
            int epochs, int batch_size=128) {
        // send model to the right device
        to_device(torch_utils::GLOBAL_DEVICE::get_device());

        auto optimizer = torch::optim::Adam(nnet->parameters(), torch::optim::AdamOptions(/*learning_rate=*/0.01));

        tqdm bar;
        for(int epoch = 0; epoch < epochs; ++epoch) {
            bar.progress(epoch, epochs);
            // set the nnet into train mode (i.e. demands gradient updates for tensors)
            nnet->train();

            for(int b = 0; b < static_cast<int> (train_examples.size() / batch_size); ++b) {

                // get a randomly drawn sample index batch
                auto sample_ids = _sample_without_replacement(train_examples.size(), batch_size);

                std::vector<torch::Tensor> board_batch(batch_size);
                std::vector<std::vector<double>> pi_batch(batch_size);
                std::vector<int> v_batch(batch_size);

                // fill the batch vectors with the respective part of the sample-tuple
                for(const auto& i : sample_ids) {
                    auto& sample = train_examples[i];
                    board_batch.push_back(sample.get_tensor());
                    pi_batch.push_back(sample.get_policy());
                    v_batch.push_back(sample.get_evaluation());
                }

                torch::TensorOptions options_int = torch::TensorOptions()
                        .device(torch_utils::GLOBAL_DEVICE::get_device())
                        .dtype(torch::kInt64)
                        .requires_grad(false);

                torch::TensorOptions options_float = torch::TensorOptions()
                        .device(torch_utils::GLOBAL_DEVICE::get_device())
                        .dtype(torch::kFloat)
                        .requires_grad(true);


                torch::Tensor board_tensor = torch::from_blob(board_batch.data(), board_batch.size(), options_int);
                torch::Tensor target_pis = torch_utils::tensor_from_vector(pi_batch, options_float);
                torch::Tensor target_vs = torch::from_blob(v_batch.data(), v_batch.size());

                auto nnet_out = nnet->forward(board_tensor);
                auto l_pi = loss_pi(target_pis, std::get<0>(nnet_out));
                auto l_v = loss_v(target_vs, std::get<1>(nnet_out));
                auto total_loss = l_pi + l_v;
                total_loss.requires_grad();

                optimizer.zero_grad();
                total_loss.backward();
                optimizer.step();
            }
        }
        bar.finish();
    }

    std::tuple<torch::Tensor, double> predict(const torch::Tensor& board_tensor);

    void save_checkpoint(std::string const & folder, std::string const & filename);
    void load_checkpoint(std::string const & folder, std::string const & filename);


    /// Forwarding methods for torch::nn::Module within nnet
    auto to(torch::Device device) {nnet->to(device);}



};


#endif //STRATEGO_CPP_NEURALNETWORK_H

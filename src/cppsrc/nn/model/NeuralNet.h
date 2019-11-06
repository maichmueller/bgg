#pragma once

#include <random>
#include <algorithm>
#include <filesystem>

#include "torch/torch.h"
#include "tqdm/tqdm.h"

#include <nn/model/modules/AlphazeroInterface.h>
#include "utils/torch_utils.h"

template<size_t Dims>
class NetworkWrapper {

    std::shared_ptr<AlphaZeroInterface> m_nnet;
    std::array<size_t, Dims> board_shape;
    size_t nr_actions;

    [[nodiscard]] std::vector<size_t> prepend_to_shape(
            const torch::Tensor &tensor,
            size_t value
    ) const;

public:

    NetworkWrapper(
            std::shared_ptr<AlphaZeroInterface> network,
            std::array<size_t, Dims> board_shape,
            size_t nr_actions)
            : m_nnet(std::move(network)),
              board_shape(board_shape),
              nr_actions(nr_actions) {}

    static inline torch::Tensor loss_pi(const torch::Tensor &targets, const torch::Tensor &outputs) {
        return -(targets * outputs).sum() / targets.size(1);
    }

    static inline torch::Tensor loss_v(const torch::Tensor &targets, const torch::Tensor &outputs) {
        return (targets - outputs).pow(2).sum() / targets.size(1);
    }

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

template<size_t Dims>
std::tuple<torch::Tensor, double>
NetworkWrapper<Dims>::predict(const torch::Tensor &board_tensor) {
    m_nnet->eval();

    // We dont want gradient updates here, so we need the NoGradGuard
    torch::NoGradGuard no_grad;
    auto[pi_tensor, v_tensor] = m_nnet->forward(board_tensor);

//    // copy the pi tensor data into a std::vector
//    std::vector<double> pi_vec(pi_tensor.size(1));
//    torch::TensorAccessor pi_acc = pi_tensor.accessor<float, 1>();
//    for(int i = 0; i < pi_tensor.size(1); ++i) {
//        pi_vec[i] = pi_acc[i];
//    }

    return std::make_tuple(pi_tensor, v_tensor.template item<float>());
}


template<size_t Dims>
void NetworkWrapper<Dims>::save_checkpoint(std::string const &folder, std::string const &filename) {
    namespace fs = std::filesystem;
    fs::path dir(folder);
    fs::path file(filename);
    fs::path full_path = dir / file;
    // if directory doesn't exist, create it
    if (!fs::exists(dir)) {
        std::cout << "Checkpoint directory doesn't exist yet. Creating it." << std::endl;
        fs::create_directory(dir);
    }

    torch::save(m_nnet, full_path.string());
}


template<size_t Dims>
void NetworkWrapper<Dims>::load_checkpoint(std::string const &folder, std::string const &filename) {
    namespace fs = std::filesystem;
    fs::path dir(folder);
    fs::path file(filename);
    fs::path full_path = dir / file;
    // if file doesnt exist, raise an error
    if (!fs::exists(full_path)) {
        std::cout << "Checkpoint directory doesn't exists yet. Creating it." << std::endl;
        throw std::invalid_argument("No file found for filename " + filename + ".");
    }

    torch::load(m_nnet, full_path.string());
}

template<size_t Dims>
template<typename TrainExampleContainer>
void NetworkWrapper<Dims>::train(
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

template<size_t Dims>
std::vector<size_t> NetworkWrapper<Dims>::prepend_to_shape(
        const torch::Tensor &tensor,
        size_t value) const {
    // get current shape and initialize +1
    auto sizes = tensor.sizes().vec();
    std::vector<size_t> sizes_out(sizes.size() + 1);
    // size 0 is batch size, rest of the shape needs to be kept from input
    sizes_out[0] = value;
    std::copy(sizes.begin(), sizes.end(), sizes_out.begin() + 1);

    return sizes_out;
}

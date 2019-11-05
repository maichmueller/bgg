#pragma once

#include "torch/torch.h"
#include <random>
#include <algorithm>

#include "tqdm/tqdm.h"
#include "azpp/utils/torch_utils.h"


class NetworkWrapper {

    std::shared_ptr<AlphaZeroInterface> m_nnet;
    int board_dim;
    int action_dim;

    static std::vector<int> _sample_without_replacement(int pool_len, int sample_len);

public:

    NetworkWrapper(std::shared_ptr<AlphaZeroInterface> net, int board_len, int action_len)
            : m_nnet(std::move(net)), board_dim(board_len), action_dim(action_len)
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
               int epochs, int batch_size=128);

    std::tuple<torch::Tensor, double> predict(const torch::Tensor& board_tensor);

    void save_checkpoint(std::string const & folder, std::string const & filename);
    void load_checkpoint(std::string const & folder, std::string const & filename);


    /// Forwarding methods for torch::nn::Module within nnet
    auto to(torch::Device device) {m_nnet->to_device(device);}

};


std::vector<int> NetworkWrapper::_sample_without_replacement(int pool_len, int sample_len) {
    /// We have to first build a vector of all the possible numbers,
    /// since we want to sample WITHOUT replacement. This can only
    /// be achieved by knowing all the numbers at draw time, i.e.
    /// all the numbers allocated.

    // fill the pool with ints from 0 to pool_len
    std::vector<int> pool(pool_len);
    for(int i = 0; i < pool_len; ++i) {
        pool[i] = i;
    }

    std::random_device rd;
    std::mt19937 g(rd());
    // shuffle all the values in the pool uniformly
    std::shuffle(pool.begin(), pool.end(), g);

    // draw sample_len many samples
    std::vector<int> sample(sample_len);
    for(int i = 0; i < sample_len; ++i) {
        sample[i] = pool[i];
    }
    return sample;
}

void NetworkWrapper::to_device(torch::Device dev) {
    m_nnet->to(dev);
}


std::tuple<torch::Tensor, double> NetworkWrapper::predict(const torch::Tensor & board_tensor) {
    m_nnet->eval();

    // We dont want gradient updates here, so we need the NoGradGuard
    torch::NoGradGuard no_grad;
    auto [pi_tensor, v_tensor] = m_nnet->forward(board_tensor);

//    // copy the pi tensor data into a std::vector
//    std::vector<double> pi_vec(pi_tensor.size(1));
//    torch::TensorAccessor pi_acc = pi_tensor.accessor<float, 1>();
//    for(int i = 0; i < pi_tensor.size(1); ++i) {
//        pi_vec[i] = pi_acc[i];
//    }

    return std::make_tuple(pi_tensor, v_tensor.template item<float>());
}

void NetworkWrapper::save_checkpoint(std::string const & folder, std::string const & filename) {
    namespace fs = std::filesystem;
    fs::path dir(folder);
    fs::path file (filename);
    fs::path full_path = dir / file;
    // if directory doesn't exist, create it
    if(!fs::exists(dir)) {
        std::cout << "Checkpoint directory doesn't exist yet. Creating it." << std::endl;
        fs::create_directory(dir);
    }

    torch::save(m_nnet, full_path.string());
}


void NetworkWrapper::load_checkpoint(std::string const &folder, std::string const &filename) {
    namespace fs = std::filesystem;
    fs::path dir(folder);
    fs::path file (filename);
    fs::path full_path = dir / file;
    // if file doesnt exist, raise an error
    if(!fs::exists(full_path)) {
        std::cout << "Checkpoint directory doesn't exists yet. Creating it." << std::endl;
        throw std::invalid_argument("No file found for filename " + filename + ".");
    }

    torch::load(m_nnet, full_path.string());
}

template<typename TrainExampleContainer>
void NetworkWrapper::train(TrainExampleContainer train_examples, int epochs, int batch_size) {
    // send model to the right device
    to_device(GLOBAL_DEVICE::get_device());

    auto optimizer = torch::optim::Adam(
            m_nnet->parameters(),
            torch::optim::AdamOptions(/*learning_rate=*/0.01));

    tqdm bar;
    for(int epoch = 0; epoch < epochs; ++epoch) {
        bar.progress(epoch, epochs);
        // set the nnet into train mode (i.e. demands gradient updates for tensors)
        m_nnet->train();

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
                    .device(GLOBAL_DEVICE::get_device())
                    .dtype(torch::kInt64)
                    .requires_grad(false);

            torch::TensorOptions options_float = torch::TensorOptions()
                    .device(GLOBAL_DEVICE::get_device())
                    .dtype(torch::kFloat)
                    .requires_grad(true);


            torch::Tensor board_tensor = torch::from_blob(board_batch.data(), board_batch.size(), options_int);
            torch::Tensor target_pis = torch_utils::tensor_from_vector(pi_batch, options_float);
            torch::Tensor target_vs = torch::from_blob(v_batch.data(), v_batch.size());

            auto nnet_out = m_nnet->forward(board_tensor);
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

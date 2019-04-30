//
// Created by michael on 13.04.19.
//

#include "NeuralNetwork.h"

std::vector<int> NetworkWrapper::_sample_without_replacement(int pool_len, int sample_len) {
    // fill the pool with ints from 0 to pool_len
    std::vector<int> pool(pool_len);
    for(int i = 0; i < pool_len; ++i) {
        pool[i] = i;
    }

    std::random_device rd;
    std::mt19937 g(rd());
    // shuffle all the values in the pool uniformly
    std::shuffle(pool.begin(), pool.end(), g);

    std::vector<int> sample(sample_len);
    for(int i = 0; i < sample_len; ++i) {
        sample[i] = pool[i];
    }
    return sample;
}

torch::Tensor NetworkWrapper::loss_pi(const torch::Tensor &targets, const torch::Tensor &outputs) {
    return -(targets * outputs).sum() / targets.size(1);
}

torch::Tensor NetworkWrapper::loss_v(const torch::Tensor &targets, const torch::Tensor &outputs) {
    return (targets - outputs).pow(2).sum() / targets.size(1);
}

void NetworkWrapper::to_device(torch::Device dev) {
    nnet->to(dev);
}



void NetworkWrapper::train(std::vector<std::tuple<torch::Tensor, std::vector<double>, int, int>> train_examples, int epochs,
                          int batch_size) {
    // send model to the right device
    to_device(torch_utils::GLOBAL_DEVICE::get_device());

    auto optimizer = torch::optim::Adam(nnet->parameters(), /*lr=*/0.01);

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
                board_batch.push_back(std::get<0>(sample));
                pi_batch.push_back(std::get<1>(sample));
                v_batch.push_back(std::get<2>(sample));
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


std::tuple<torch::Tensor, double> NetworkWrapper::predict(torch::Tensor & board_tensor) {
    nnet->eval();

    // We dont want gradient updates here, so we need the NoGradGuard
    torch::NoGradGuard no_grad;
    auto [pi_tensor, v_tensor] = nnet->forward(board_tensor);

//    // copy the pi tensor data into a std::vector
//    std::vector<double> pi_vec(pi_tensor.size(1));
//    torch::TensorAccessor pi_acc = pi_tensor.accessor<float, 1>();
//    for(int i = 0; i < pi_tensor.size(1); ++i) {
//        pi_vec[i] = pi_acc[i];
//    }


    return std::make_tuple(pi_tensor, v_tensor.item<float>());
}